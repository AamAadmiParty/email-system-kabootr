<?php 

/**
 * Send simplenews newsletters from the spool.
 *
 * Individual newsletter emails are stored in database spool.
 * Sending is triggered by cron or immediately when the node is saved.
 * If sending is successful the message is marked as send in the spool.
 *
 * @return
 *   Returns the amount of sent mails.
 *
 */

require_once 'classes/amazonses.class.php';
$path = drupal_get_path('module', 'simplenews_amazon_ses');
require_once $path . '/vendor/autoload.php';  
use \Guzzle\Http\Client,
Guzzle\Http\Exception\RequestException;
use Guzzle\Batch\Batch;
use Guzzle\Batch\BatchBuilder;
use Guzzle\Common\Event;
use Guzzle\Batch\Exception\BatchTransferException;



/**
 * Send mail form simplenews spool mails, via amazon SES
 * @return string
 */
function simplenews_amazon_ses_mail_spool() {
  module_load_include('inc', 'simplenews', 'includes/simplenews.mail');
  // Send pending messages from database cache.
  $spool_list = simplenews_get_spool(40, $conditions = array());
  if ($spool_list) {

    $sent = array();
    $fail = array();
    $count_success = 0;
    
    // Set true, when all sending mail ara attached to batch queue
    $send_mail_flag = FALSE;
    $spool = new SimplenewsSpool($spool_list);
    $result = simplenews_amazon_ses_send_source($spool);

    foreach ($result as $msid => $row) {
      
      // Handle http request exception
      if ($row['status'] == KABOOTR_AMAZON_REQUEST_EXCEPTION) {
        return '';
      }
      
      // Update the status of mail spool
      simplenews_update_spool(array($msid), $row);
      if ($row['status'] == SIMPLENEWS_SPOOL_DONE) {
        $count_success ++;
        if (! isset($sent[$spool_list[$msid]->actual_nid])) {
          $sent[$spool_list[$msid]->actual_nid] = 1;
        }
        else {
          $sent[$spool_list[$msid]->actual_nid] ++;
        }
      }
      elseif ($row['error']) {
        $message = t('Message sending failed to !mail as !message', array(
            '!mail' => $spool_list[$msid]->mail,
            '!message' => $result['Message']
        ));
        watchdog('kabootr', $message);
        if (! isset($fail[$spool_list[$msid]->actual_nid])) {
          $fail[$spool_list[$msid]->actual_nid] = 1;
        }
        else {
          $fail[$spool_list[$msid]->actual_nid] ++;
        }
      }
    }
   
    // Update subscriber count.
    simplenews_amazon_ses_update_subscriber_count($sent);
    
    // Set the status of last cron run of sending mail
    variable_set('simplenews_last_cron', REQUEST_TIME);
    variable_set('simplenews_last_sent', $count_success);
  } 
}

/**
 * Send a node to an email address.
 *
 * @param $source
 *   The source object.s
 *
 * @return boolean
 *   TRUE if the email was successfully delivered; otherwise FALSE.
 *
 * @ingroup source
 */
function simplenews_amazon_ses_send_source(SimplenewsSpool $spool = NULL) {
  
  $actionParameters = '';
  $message = array(
      'msid'     => '',  // Used for updating mail status in simplenews letter, so that mail could not be sent twice
      'to'       => '',
      'from'     => '',
      'language' => '',
      'send'     => TRUE,
      'subject'  => '',
      'body'     => '',
      'return_path' => '',
      'replyto' => '',
      'htmltext' => '',
  );
  
   while ($source = $spool->nextSource()) {
     foreach ($spool->getProcessed() as $msid => $row) {
       
       // Build the e-mail
       $message['to'] = $source->getRecipient();
       $message['from'] = $source->getFromFormatted();
       $message['language'] = $source->getLanguage();
       $message['subject'] = $source->getSubject();
       $message['htmltext'] = $source->getPlainBody() . "\n" . $source->getPlainFooter();
       $message['attachments'] = $source->getAttachments();
       $message['msid'] = $row->msid;
       $simpleEmailServiceMessage = new SimpleEmailServiceMessage($message);
       $actionParameters[]['simpleEmailServiceMessage'] = $simpleEmailServiceMessage;
     }
    }
  $result = simplenews_amazon_ses_send_request('SendEmail', $actionParameters);
  return $result;
}


/**
 * Send the request to amazon ses, which directly call Amazon SES API
 * After sending request, it parse the http response returned by Amazon API call
 * @param $qyeryAction Name of Action which is to be called,
 *        $actionParameter Additonal parameter needed to complete the call Amazon SES API
 * @return Parsed http response, used to display to user or to take system appropriate aciton 
 */
function simplenews_amazon_ses_send_request($queryAction, $actionParameters) {
  $result = NULL;
  
  if ($queryAction === 'SendEmail') {
    $queryEndPoint = SimpleEmailService::queryEndPoint;
    $request_array = array();
    
    // Create batch using BatchBuilder
    $batch = BatchBuilder::factory()
    ->transferRequests(40)
    ->autoFlushAt(1)
    ->build();
      
      // Create a client and provide a base URL
    $client = new Client($queryEndPoint);
    $_SESSION['time'] = 0;
    foreach ($actionParameters as $actionParameter) {
      $simpleEmailService = new SimpleEmailService();
      $simpleEmailService->createQueryRequest($queryAction, $actionParameter);
      
      $request = $client->post('', $simpleEmailService->getRequestHeaders(), $simpleEmailService->getRequestParameter(), $options = array());
      $request->getEventDispatcher()->addListener('request.success', function (Event $e) {
        $msid = (string) $e['request']->getHeader('msid');
        $queryAction = (string) $e['request']->getHeader('Action');
        $response_xml = (string) $e['response']->getBody();
        $http_response_xml = simplexml_load_string($response_xml);
        
        // Reterieve query action name from http response object
        $queryActionResult = key((array) $http_response_xml);
        $actionResponse = $http_response_xml->$queryActionResult;
        if ($msid == '' || $msid == NULL) {
          $msid = 'msid';
        }
        $response_code = $e['response']->getStatusCode();
        $_SESSION['query_response'][$msid]['queryActionResult'] = $queryActionResult;
        $_SESSION['query_response'][$msid]['actionResponse'] = $actionResponse;
        $_SESSION['query_response'][$msid]['statusCode'] = $response_code;

        // Create log file
        if ($response_code === 200) {
          $string = $e['request']->getPostFields()->__toString();
          $to_address = simplenews_amazon_ses_get_post_field($string, 'Destination.ToAddresses.member.1');
          $to_address = urldecode($to_address);
          $mail_sending_time = date('l jS \of F Y h:i:s A');
          $message_id = (string) $http_response_xml->SendEmailResult->MessageId;
          $request_id = (string) $http_response_xml->ResponseMetadata->RequestId;
          $message = "[{$mail_sending_time}] [Sending Mail Success] [Request Id : {$request_id}], [Message Id : {$message_id}] {$to_address}\n";
          
          $file = 'mail.log';
          // Write the contents to the file,
          // using the FILE_APPEND flag to append the content to the end of the file
          // and the LOCK_EX flag to prevent anyone else writing to the file at the same time
          file_put_contents($file, $message, FILE_APPEND | LOCK_EX);
        }
      });
      
      try {
        $batch->add($request);
      } catch ( BatchTransferException $e ) {
        $result[]['status'] = KABOOTR_AMAZON_REQUEST_EXCEPTION;
        $request_string = $e->getBatch()[0]->getPostFields()->__toString();
        $to_address = simplenews_amazon_ses_get_post_field($request_string, 'Destination.ToAddresses.member.1');
        $to_address = urldecode($to_address);
        
        $response_xml = (string) $e->getBatch()[0]->getResponse()->getBody();
        $http_response_xml = simplexml_load_string($response_xml);
        $mail_sending_time = date('l jS \of F Y h:i:s A');
        $error_message = (string) $http_response_xml->Error->Message;
        $request_id = (string) $http_response_xml->RequestId;
        $message = "[{$mail_sending_time}] [Sending Email Failed] [Request Id : {$request_id}], [Error Message: {$error_message}] {$to_address}\n";

        // Write the contents to the file,
        // using the FILE_APPEND flag to append the content to the end of the file
        // and the LOCK_EX flag to prevent anyone else writing to the file at the same time
        file_put_contents($file, $message, FILE_APPEND | LOCK_EX);
      }
    }
  	  
       
    if (isset($_SESSION['query_response'])) {
      $query_response = $_SESSION['query_response'];
      foreach ($query_response as $msid => $value) {
        $queryActionResult = $value['queryActionResult'];
        $length = strpos($queryActionResult, 'Result');
        $queryAction = substr($queryActionResult, 0, $length);
    
        if ($msid == 'msid') {
          $result = $simpleEmailService->getQueryResponse($queryAction, $value['actionResponse'], $value['statusCode']);
        }
        else {
          $result[$msid] = $simpleEmailService->getQueryResponse($queryAction, $value['actionResponse'], $value['statusCode']);
        }
      }
      unset($_SESSION['query_response']);
    }
    
  }
  else {
    $simpleEmailService = new SimpleEmailService();
    $simpleEmailService->createQueryRequest($queryAction, $actionParameters);
    $queryEndPoint = SimpleEmailService::queryEndPoint;
    
    // Create a client and provide a base URL
    $client = new Client($queryEndPoint);
    $request = $client->post('', $simpleEmailService->getRequestHeaders(), $simpleEmailService->getRequestParameter(), $options = array());
    
    try {
      $response = $request->send();
      $response_xml = (string) $response->getBody();
      $http_response_xml = simplexml_load_string($response_xml);
      
      // If there is error in response
      if (isset($http_response_xml->Error)) {
        $queryActionResult = 'Error';
        $actionResponse = $http_response_xml->Error;
      }
      else {
        $queryActionResult = $queryAction . 'Result';
        $actionResponse = $http_response_xml->$queryActionResult;
      }
      $result = $simpleEmailService->getQueryResponse($queryAction, $actionResponse, $response->getStatusCode());
    } catch ( RequestException $e ) {
        $result['status'] = KABOOTR_AMAZON_REQUEST_EXCEPTION;
    }
  }
  
   // Flush the queue and retrieve the flushed items
   /* $arrayOfTransferredRequests = $batch->flush();
   foreach ($arrayOfTransferredRequests as $v) {
     dpm($v->getResponse()->getInfo()['total_time']);
   } */
  
 /*  $http_req = new HttpRequest($queryEndPoint,HTTP_METH_POST);
  $http_req->setHeaders($simpleEmailService->getRequestHeaders());
  $http_req->setPostFields($simpleEmailService->getRequestParameter());
  $result = array();
  try {
    $http_req->send();
    $http_response_xml = simplexml_load_string($http_req->getResponseBody());
    //dpm($http_req->getResponseBody());
    //dpm($http_req->getResponseInfo());
    // If there is error in response
    if (isset($http_response_xml->Error)) {
      $queryActionResult = 'Error';
      $actionResponse = $http_response_xml->Error;
    }
    else {
      $queryActionResult = $queryAction . 'Result';
      $actionResponse = $http_response_xml->$queryActionResult;
    }
    $result = $simpleEmailService->getQueryResponse($queryAction, $actionResponse, $http_req->getResponseCode());
  } 
  catch ( HttpException $ex ) {
    $result['status'] = KABOOTR_AMAZON_REQUEST_EXCEPTION;
  }
 */  
  return $result;
}


/**
 * Update subscriber count.
 */
function simplenews_amazon_ses_update_subscriber_count($sent) {
   foreach ($sent as $nid => $count) {
    db_update('simplenews_newsletter')
      ->condition('nid', $nid)
      ->expression('sent_subscriber_count', 'sent_subscriber_count + :count', array(
        ':count' => $count
    ))
    ->execute();
  }
}

/**
 * Handle the bounced and compaint mails,
 * take the appropriate action like unsubcribe or delete completely form database
 * 
 */
function simplenews_amazon_ses_control_sending_mail() {
  require 'classes/amazonsns.class.php';
  module_load_include('inc', 'simplenews', 'includes/simplenews.mail');
  $simpleNotificationService = new SimpleNotificationService();
  // Find out bounced or complained mails
  $simpleNotificationService->readSendingStatics($jsonResponse);
  $problem_mail = $simpleNotificationService->getProblemMail();
  // We have to decide problem mail has to be unsubribed form all newsletter category or existing
  // or delete that user in both cases
  // 1. Complaint
  // 2. Bounces
 // simplenews_unsubscribe_user($mail, $tid);
}

/**
 * Convert  Guzzle\Http\QueryString to array
 */
function simplenews_amazon_ses_get_post_field($field_data, $field_name = NULL) {
  $temp = explode('&', $field_data);
  foreach ($temp as $value) {
    $post_field = explode('=', $value);
    $post_fields[$post_field[0]] = $post_field[1];
  }
  
  if ($field_name != NULL) {
    if (isset($post_fields[$field_name])) {
      return $post_fields[$field_name];
    }
    else {
      return '';
    }
  }
  
  else {
    return $post_fields;
  }
  
}
