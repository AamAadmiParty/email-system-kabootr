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

/**
 * Send mail form simplenews spool mails, via amazon SES
 * @return string
 */
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
        
        // Handle http request exception
        if ($result['status'] == KABOOTR_AMAZON_REQUEST_EXCEPTION) {
        	drupal_set_message('Please try again', 'error');
        	return '';
        }
        // Update the status of mail spool
        simplenews_update_spool(array($msid), $result);
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
      simplenews_amazon_ses_update_subscriber_count($sent);
      
      // Set the status of last cron run of sending mail
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
  return $result;
}


/**
 * Send the request to amazon ses, which directly call Amazon SES API
 * After sending request, it parse the http response returned by Amazon API call
 * @param $qyeryAction Name of Action which is to be called,
 *        $actionParameter Additonal parameter needed to complete the call Amazon SES API
 * @return Parsed http response, used to display to user or to take system apporpriate aciton 
 */
function simplenews_amazon_ses_send_request($queryAction, $actionParameter) {
  
  $simpleEmailService = new SimpleEmailService();
  $simpleEmailService->createQueryRequest($queryAction, $actionParameter);
  $queryEndPoint = SimpleEmailService::queryEndPoint;
  $http_req = new HttpRequest($queryEndPoint,HTTP_METH_POST);
  $http_req->setHeaders($simpleEmailService->getRequestHeaders());
  $http_req->setPostFields($simpleEmailService->getRequestParameter());

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
    $result = $simpleEmailService->getQueryResponse($queryAction, $actionResponse, $http_req->getResponseCode());
  } 
  catch ( HttpException $ex ) {
    drupal_set_message($ex, 'error');
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



