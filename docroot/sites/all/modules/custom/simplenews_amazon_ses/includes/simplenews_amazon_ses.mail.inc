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
 Guzzle\Http\Exception\MultiTransferException,
Guzzle\Http\Exception\RequestException;
use Guzzle\Batch\Batch;
use Guzzle\Batch\BatchRequestTransfer;
use Guzzle\Batch\BatchBuilder;
use Guzzle\Common\Event;
use Guzzle\Cache\NullCacheAdapter;



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
    /* $batch = BatchBuilder::factory()
    ->transferRequests(2)
   // ->autoFlushAt(2)
    ->build();
     */
    // BatchRequestTransfer acts as both the divisor and transfer strategy
   /*  $transferStrategy = new BatchRequestTransfer(10);
    $divisorStrategy = $transferStrategy;
    
    $batch = new Batch($transferStrategy, $divisorStrategy); */
    $request_array = '';
    
  	foreach ($actionParameters as $actionParameter) {
  	  $simpleEmailService = new SimpleEmailService();
  	  $simpleEmailService->createQueryRequest($queryAction, $actionParameter);
  	  $queryEndPoint = SimpleEmailService::queryEndPoint;
  	  
  	  // Create a client and provide a base URL
  	  $client = new Client($queryEndPoint);
  	  
  	  $request = $client->post('',$simpleEmailService->getRequestHeaders(), $simpleEmailService->getRequestParameter(), $options = array());

  	  $request->getEventDispatcher()->addListener('request.success', function (Event $e) {
  	    $msid = (string)$e['request']->getHeader('msid');
  	    $queryAction = (string) $e['request']->getHeader('Action');
  	    $response_string = (string) $e['response']->getBody();
  	    $http_response_xml = simplexml_load_string($response_string);
  	    
  	    // If there is error in response
  	    if (isset($http_response_xml->Error)) {
  	      $queryActionResult = 'Error';
  	      $actionResponse = $http_response_xml->Error;
  	    }
  	    else {
  	      // Reterieve query action name from http response object
  	      $queryActionResult = key((array)$http_response_xml);
  	      $actionResponse = $http_response_xml->$queryActionResult;
  	    }
  	    if ($msid == '' || $msid == NULL) {
  	    	$msid = 'msid';
  	    }
  	     $_SESSION['query_response'][$msid]['queryActionResult'] = $queryActionResult;
  	     $_SESSION['query_response'][$msid]['actionResponse'] = $actionResponse;
  	     $_SESSION['query_response'][$msid]['statusCode'] = $e['response']->getStatusCode();
  	  });
        // $batch->add($request);
      $request_array[] = $request;
    }
    
    try {
       $client->send($request_array);
    } catch ( MultiTransferException $e ) {
      
      $result[]['status'] = KABOOTR_AMAZON_REQUEST_EXCEPTION;
      
      $n = 0;
      if ($e->getFailedRequests()) {
        $n = count($e->getFailedRequests());
      }
      if ($n > 0) {
        watchdog('status', 'Number of requests has been failed in Kabootr are' . $n);
      }
      
      $n = 0;
      if ($e->getSuccessfulRequests()) {
        $n = count($e->getSuccessfulRequests());
      }
      if ($n > 0) {
        watchdog('error', 'Number of request has been succeed  in Kabootr are ' . $n);
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
      $response_string = (string) $response->getBody();
      $http_response_xml = simplexml_load_string($response_string);
      
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



