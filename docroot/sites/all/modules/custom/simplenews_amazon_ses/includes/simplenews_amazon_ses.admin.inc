<?php 

/**
 *
 */
function amazon_email_setting_form($form, &$form_state) {
 $form['amazon_ses_service'] = array(
     '#type' => 'fieldset',
     '#title' => t('Subscribe to Amazon SES Service'),
     '#collapsed' => FALSE,
    // '#prefix' => '<div class="amazon-services-wrapper">',
    // '#suffix' => '</div>',
      
 );
 
 $form['amazon_ses_service']['identity'] = array(
     '#type' => 'textfield',
     '#title' => t('Email Address'),
     '#size' => 60,
 );
/*  $form['amazon_ses_service']['SnsTopic'] = array(
     '#type' => 'textfield',
     '#title' => t('SNS Topic'),
     '#size' => 60,
     '#access' => FALSE,
 );*/
 $form['amazon_ses_service']['services'] = array(
     '#type' => 'radios',
     '#options' => get_amazon_ses_service(),
     '#title' => t('Please select service'),
     '#required' => TRUE,
     /* '#ajax' => array(
         'callback' => 'show_amazon_service_form',
         'wrapper' => 'amazon-services-wrapper',
         'method' => 'append',
         'effect' => 'fade',
     
     ), */
 );
 
 $form['amazon_ses_service']['send_request'] = array(
     '#type' => 'submit', 
     '#value' => t('Send Request'),
 );
 
 return $form;
}

/**
 * TODO
 */
function show_amazon_service_form(&$form, &$form_state) {
  return $form;
}
/**
 * Form validation callback for amazon email setting form.
 */
function amazon_email_setting_form_validate($form, &$form_state) {
    // Check for valid email address.
    if (!valid_email_address($form_state['values']['identity'])) {
      form_set_error('from_address', t("The sender's email address you supplied is not valid."));
    }
}

/**
 * Submit handler for amazon email setting form.
 */
function amazon_email_setting_form_submit($form, &$form_state) {
  module_load_include('inc', 'simplenews_amazon_ses', 'includes/simplenews_amazon_ses.mail');
  if (isset($form_state['clicked_button']['#value'])) {
    $service = $form_state['values']['services'];
    $actionParameter = array();
    $actionParameter['identity'] = $form_state['values']['identity'];
    if (strpos('_', $service)) {
    	$service_arr = explode('_', $service);
    	$service = $service_arr[0];
    	if ($service == 'SetIdentityNotificationTopic') {
    	  $actionParameter['NotificationType'] = $service_arr[1];
    	}
    }
    $result = simplenews_amazon_ses_send_request($service, $actionParameter);
    switch ($result['status']) {
      case KABOOTR_VERIFY_EMAIL_SUCCESS:
        $message = t('A verification mail with further instruction has been sent to !mail By Amazon SES', array('!mail' => $form_state['values']['sender_email']));
        drupal_set_message($message, 'status');
        break;
        case KABOOTR_AMAZON_REQUEST_EXCEPTION:
          drupal_set_message('Exception','error');
          break;
    }
  }
}

/**
 * 
 */
function amazon_set_identity_notification_topic_form() {
  
}
/**
 * Amazon SES services
 */
 function get_amazon_ses_service() {
   $services = array(
       'VerifyEmailIdentity' => 'Verfiy Sender Email Address',
       'SetIdentityNotificationTopic_Complaint' => 'Set Identity Notification Topic for receiving <b>Complain</b> Feedback',
       'SetIdentityNotificationTopic_Bounce' => 'Set Identity Notification Topic for receiving <b>Bounce</b> Feedback',
   );
   return $services;
 }

 /**
  * TODO
  */
function get_send_statistics_form($form, $form_state) { 
  module_load_include('inc', 'simplenews_amazon_ses', 'includes/simplenews_amazon_ses.mail');
  $result = simplenews_amazon_ses_send_request('GetSendStatistics', $actionParameter = array());
  sleep(2);
  $result_1 = simplenews_amazon_ses_send_request('GetSendQuota', $actionParameter = array());
  dpm($result);
  dpm($result_1);
} 