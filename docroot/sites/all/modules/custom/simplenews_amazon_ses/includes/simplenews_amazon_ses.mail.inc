<?php 


/**
 * @file
* Amazon SES Mail system
* Used by drupal core system
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
use Guzzle\Cache\NullCacheAdapter;

/**
 * Create system which will send mail using amazon ses api.
 */
class AmazonSesMailSystem extends DefaultMailSystem {
	
	/**
	 * Send an e-mail message, using Drupal variables and default settings.
	 *
	 * @param $message
	 *   A message array, as described in hook_mail_alter().
	 * @return
	 *   TRUE if the mail was successfully accepted, otherwise FALSE.
	 */
	public function mail(array $message) {
		$message['messageText'] = NULL;
		$message['messageHtml'] = $message['body'];
		$message['characterSet'] = 'UTF-8';
		$message['returnPath'] = $message['headers']['Return-Path'];
//		$message['returnPath'] = NULL;
		$message['replyTo'] = NULL;
		unset($message['headers']);
		unset($message['body']);
		$simpleEmailServiceMessage = new SimpleEmailServiceMessage($message);
		$actionParameters[]['simpleEmailServiceMessage'] = $simpleEmailServiceMessage;
		$result = simplenews_amazon_ses_send_request('SendEmail', $actionParameters);
		return $result;
	}
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
    foreach ($actionParameters as $actionParameter) {
      $simpleEmailService = new SimpleEmailService();
      $simpleEmailService->createQueryRequest($queryAction, $actionParameter);
      
      $request = $client->post('', $simpleEmailService->getRequestHeaders(), $simpleEmailService->getRequestParameter(), $options = array());
      $request->getEventDispatcher()->addListener('request.success', function (Event $e) {
        $queryAction = (string) $e['request']->getHeader('Action');
        $response_xml = (string) $e['response']->getBody();
        $http_response_xml = simplexml_load_string($response_xml);
        
        // Reterieve query action name from http response object
        $queryActionResult = key((array) $http_response_xml);
        $actionResponse = $http_response_xml->$queryActionResult;
        $response_code = $e['response']->getStatusCode();
      
        // Create log file
        if ($response_code === 200) {
          $string = $e['request']->getPostFields()->__toString();
          $to_address = simplenews_amazon_ses_get_post_field($string, 'Destination.ToAddresses.member.1');
          $to_address = urldecode($to_address);
          $mail_sending_time = date('l jS \of F Y h:i:s A');
          $message_id = (string) $http_response_xml->SendEmailResult->MessageId;
          $request_id = (string) $http_response_xml->ResponseMetadata->RequestId;
          $message = "[{$mail_sending_time}] [Sending Mail Success] [Request Id : {$request_id}], [Message Id : {$message_id}] {$to_address}\n";

          
          // Write the contents to the file,
          // using the FILE_APPEND flag to append the content to the end of the file
          // and the LOCK_EX flag to prevent anyone else writing to the file at the same time
          $file = 'mail.log';
          file_put_contents($file, $message, FILE_APPEND | LOCK_EX);
        }
      });
      
      try {
        $batch->add($request);
        $result = KABOOTR_MAIL_SENT;
      } catch ( BatchTransferException $e ) {
        $result = KABOOTR_MAIL_NOT_SENT;
        $batch_obj = $e->getBatch();
        $request_string = $batch_obj[0]->getPostFields()->__toString();
        $to_address = simplenews_amazon_ses_get_post_field($request_string, 'Destination.ToAddresses.member.1');
        $to_address = urldecode($to_address);
        
        $mail_sending_time = date('l jS \of F Y h:i:s A');
        $request_id = '';
        if ($batch_obj[0]->getResponse() == NULL) {
        	$error_message = t('Request to <b>!action</b> action of Amazon SES API call has failed, 
                    please check your network connection or try after some time.', array(
              '!action' => "SendEmail"
          ));
          drupal_set_message($error_message, 'error');
        }
        else {
          $response_xml = (string) $batch_obj[0]->getResponse()->getBody();
          $http_response_xml = simplexml_load_string($response_xml);
          $error_message = (string) $http_response_xml->Error->Message;
          $request_id = (string) $http_response_xml->RequestId;
        }
        $message = "[{$mail_sending_time}] [Sending Email Failed] [Request Id : {$request_id}], [Error Message: {$error_message}] {$to_address}\n";
 
        $file = 'mail.log';
        // Write the contents to the file,
        // using the FILE_APPEND flag to append the content to the end of the file
        // and the LOCK_EX flag to prevent anyone else writing to the file at the same time
        file_put_contents($file, $message, FILE_APPEND | LOCK_EX);
      }
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
        $errorMessage = $e->getMessage();
        if (strpos($errorMessage, '403')) {
          $path = drupal_get_path('module', 'simplenews_amazon_ses');
          $file_name = $_SERVER['DOCUMENT_ROOT'] . '/' . $path . "/amazon_credential.txt";
          $credential_link = l('Update Your AWS Credential','amazon_ses/amazon_credential');
          drupal_set_message("You AWS credentials are not matching, please update them {$credential_link}", 'error');
        }
        else {
          $result['status'] = KABOOTR_AMAZON_REQUEST_EXCEPTION;
        }
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
