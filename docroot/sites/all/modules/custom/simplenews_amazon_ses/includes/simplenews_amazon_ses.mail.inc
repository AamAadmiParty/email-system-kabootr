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
require 'classes/amazonses.class.php';
function simplenews_amazon_ses_mail_spool() {
  module_load_include('inc', 'simplenews', 'includes/simplenews.mail');
  // Send pending messages from database cache.
  $spool_list = simplenews_get_spool($limit = '', $conditions = array());
  if ($spool_list) {

    $sent = array();
    $fail = array();
    $count_success = 0;
    $spool = new SimplenewsSpool($spool_list);
    while ($source = $spool->nextSource()) {
      $result = simplenews_amazon_ses_send_source($source);
      foreach ($spool->getProcessed() as $msid => $row) {
        // Update the status of mail spoll
        
        if ($result['status'] == KABOOTR_AMAZON_REQUEST_EXCEPTION) {
        	drupal_set_message('Exception', 'error');
        	return '';
        }
       // simplenews_update_spool(array($msid), $result);
        if ($result['status'] == SIMPLENEWS_SPOOL_DONE) {
          $count_success++;
          if (! isset($sent[$row->actual_nid])) {
            $sent[$row->actual_nid] = 1;
          }
          else {
            $sent[$row->actual_nid]++;
          }
        }
        elseif ($result['error']) {
          $message = t('Message sending failed to !mail as !message', array(
          	  '!mail' => $source->getRecipient(),
              '!message' => $result['Message'],
          ));
          watchdog('kabootr', $message);
          if (! isset($fail[$row->actual_nid])) {
            $fail[$row->actual_nid] = 1;
          }
          else {
            $fail[$row->actual_nid]++;
          }
        }
      }
      //Update subscriber count.
     // simplenews_amazon_ses_update_subscriber_count($sent);
      
      variable_set('simplenews_last_cron', REQUEST_TIME);
      variable_set('simplenews_last_sent', $count_success);
    } 
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
function simplenews_amazon_ses_send_source(SimplenewsSourceInterface $source) {
  
  $message = array(
     // 'nid'      => $nid,
      'to'       => $source->getRecipient(),
      'from'     => $source->getFromFormatted(),
      'language' => $source->getLanguage(),
      'send'     => TRUE,
      'subject'  => '',
      'body'     => '',
      'return_path' => '',
      'replyto' => '',
      'htmltext' => '',
  );
  
  // Build the e-mail (get subject and body, allow additional headers) by
  $message['subject'] = $source->getSubject();
  $message['htmltext'] = $source->getPlainBody() . "\n" . $source->getPlainFooter();
  $message['attachments'] = $source->getAttachments();
  
  $simpleEmailServiceMessage = new SimpleEmailServiceMessage($message);
  $actionParameter['simpleEmailServiceMessage'] = $simpleEmailServiceMessage;
  $result = simplenews_amazon_ses_send_request('SendEmail', $actionParameter);
 /*  // Log sent result in watchdog.
  if (variable_get('simplenews_debug', FALSE)) {
    if ($message['result']) {
      watchdog('simplenews', 'Outgoing email. Message type: %type<br />Subject: %subject<br />Recipient: %to', array('%type' => $source->getKey(), '%to' => $message['to'], '%subject' => $message['subject']), WATCHDOG_DEBUG);
    }
    else {
      watchdog('simplenews', 'Outgoing email failed. Message type: %type<br />Subject: %subject<br />Recipient: %to', array('%type' => $source->getKey(), '%to' => $message['to'], '%subject' => $message['subject']), WATCHDOG_ERROR);
    }
  } */

  return $result;
}


/**
 *todo 
 */
function simplenews_amazon_ses_send_request($queryAction, $actionParameter) {
  
  $simpleEmailService = new SimpleEmailService();
  $simpleEmailService->createQueryRequest($queryAction, $actionParameter);
  $queryEndPoint = SimpleEmailService::queryEndPoint;
  $http_req = new HttpRequest($queryEndPoint,HTTP_METH_POST);
  $http_req->setHeaders($simpleEmailService->getRequestHeaders());
  $http_req->setPostFields($simpleEmailService->getRequestParameter());
 //dpm($http_req->getHeaders());
//dpm($http_req->getPostFields()); 
  $result = array();
  try {
    $http_req->send();
    $http_response_xml = simplexml_load_string($http_req->getResponseBody());
    dpm($http_req->getResponseBody());
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
    
    // $response['actionResponse'] = $actionResponse;
    // $response['responseCode'] = $http_req->getResponseCode();
    
    $result = $simpleEmailService->getQueryResponse($queryAction, $actionResponse, $http_req->getResponseCode());
  } 
  catch ( HttpException $ex ) {
    $result['status'] = KABOOTR_AMAZON_REQUEST_EXCEPTION;
  }
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





