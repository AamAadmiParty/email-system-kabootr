<?php 

/**
 * Implements hook_form,
 * Form for sending request to verify sender identity.
 * @param unknown $form
 * @param unknown $form_state
 * @return Array $form
 */
function amazon_identity_verify_form($form, &$form_state) {
  drupal_add_js(drupal_get_path('module', 'simplenews_amazon_ses') . '/js/amazon_ses_config.js');
  drupal_add_css(drupal_get_path('module', 'simplenews_amazon_ses') . '/css/amazon_ses_config.css');
  
  $form['amazon_ses_service'] = array(
      '#type' => 'fieldset',
      '#title' => t('Subscribe to Amazon SES Service'),
      '#collapsed' => FALSE,
  );

  $form['ajax_region'] = array(
      '#prefix' => '<div id = "amazon-identity-setting-form-wrapper">',
      '#suffix' => '</div>',
  );
  $form['amazon_ses_service']['identity_type'] = array(
      '#type' => 'radios',
      '#title' => t('Please Select Identityfication Type'),
      '#options' => array(
          'EmailAddress' => 'Email Address',
          'Domain' => 'Domain Name'
      ),
      '#attributes' => array('class' => array('container-inline')),
      '#required' => TRUE,
      '#description' => t('To maintain trust between
          ISPs and Amazon SES, Amazon SES needs to ensure that its senders are who they say they are'
      ),
      // ajax attached to show number of installment persent in selected fees type
      /* '#ajax' => array(
       'callback' => 'amazon_identity_verify_form_callback',
          'wrapper' => 'amazon-identity-verify-form-wrapper',
          'effect' => 'fade',
          'method' => 'html',
      ), */
  );
  $form['amazon_ses_service']['identity_domain'] = array(
      '#type' => 'textfield',
      '#title' => t('Fully Qualified Domain Name'),
      '#size' => 60,
      '#description' => t('You must verify each email
                address that will be used as a <b>From</b> or <b>Return-Path</b> address for your messages.<br>
                The entire email address is <b>case-sensitive</b>. <br>
                Until you are granted production access to Amazon SES, you must also verify the email address
                of every recipient except for the recipients provided by the Amazon SES mailbox simulator.<br>
          <b>Important:</b> In order to complete verification, On success full sending this request you will get a token or a TXT 
          record that must be placed in the DNS settings for the domain.'),
      '#prefix' => '<div class = identity-domain-class>',
      '#suffix' => '</div>',
  );
   
  $form['amazon_ses_service']['identity_email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email Address'),
      '#size' => 60,
      '#description' => t('When you verify an entire domain, you are verifying all email addresses from
                        that domain, so you dont need to verify email addresses from that domain individually..<br>
                        Domain names are <b>case-insensitive</b>.
                        <br> <b>Important:</b> Amazon SES only verifies fully qualified domain names (FQDNs). Even if you verify a domain,
                        you have to verify subdomains of that domain. For example, if you want to send email from both
                        example.com and newyork.example.com, you need to verify each of these FQDNs separately.'),
      '#prefix' => '<div class = identity-email-class>',
      '#suffix' => '</div>',
  );
   
  $form['amazon_ses_service']['send_request'] = array(
      '#type' => 'submit',
      '#value' => t('Send Request'),
  );
  return $form; 
}
function amazon_identity_verify_form_callback($form, &$form_state) {
  /* if (isset($form_state['values']['identity_type'])) {
    $identity_type = $form_state['values']['identity_type'];
    switch($identity_type) {
      case 'email' :
        $form['amazon_ses_service']['ajax_region']['identity_email']['#access'] = TRUE;
        break;
      case 'domain' :
        $form['amazon_ses_service']['ajax_region']['identity_domain']['#access'] = TRUE;
        break;
    }
  }
  return $form['amazon_ses_service']['ajax_region']; */
}

/**
 * Form validation callback for amazon identity request form.
 */
function amazon_identity_verify_form_validate($form, &$form_state) {
 if (isset($form_state['values']['identity_type'])) {
   $identity_type = $form_state['values']['identity_type'];
   switch($identity_type) {
   	case 'EmailAddress' :
   	  if ($form_state['values']['identity_email'] == '') {
   	    form_set_error('identity_email', t("The sender's email address is required."));
   	    return '';
   	  }
   	  if (!valid_email_address($form_state['values']['identity_email'])) {
   	    form_set_error('identity_email', t("The sender's email address you supplied is not valid."));
   	  }
   	  break;
   	case 'Domain' :
      if ($form_state['values']['identity_domain'] == '') {
   	    form_set_error('identity_domain', t("Domain name is required."));
   	    return '';
   	  }
   	  break;
   }
 }
}

/**
 * Submit handler for amazon identity request form.
 */
function amazon_identity_verify_form_submit($form, &$form_state) {
    
    // Send Request to amzon SES API
  module_load_include('inc', 'simplenews_amazon_ses', 'includes/simplenews_amazon_ses.mail');
  if (($form_state['clicked_button']['#value']) == 'Send Request') {
    if (isset($form_state['values']['identity_type'])) {
      $identity_type = $form_state['values']['identity_type'];
      $_SESSION['amazon_selected_identity'] = $identity_type;
      switch($identity_type) {
        case 'EmailAddress' :
          $actionParameter['EmailAddress'] = $form_state['values']['identity_email'];
          $result = simplenews_amazon_ses_send_request('VerifyEmailIdentity', $actionParameter);
          break;
        case 'Domain' :
          $actionParameter['Domain'] = $form_state['values']['identity_domain'];
          $result = simplenews_amazon_ses_send_request('VerifyDomainIdentity', $actionParameter);
          break;
      }
    }
    
    // Display the response of calling of Amazon SES API
    switch($result['status']) {
      case KABOOTR_VERIFY_EMAIL_SUCCESS :
        $message = t('A verification mail with further instruction has been sent to !mail By Amazon SES', array(
            '!mail' => $form_state['values']['identity_email']
        ));
        drupal_set_message($message, 'status');
        drupal_goto('admin/config/services/amazon_ses/amazon_identity_list');
        break;
      
      case KABOOTR_VERIFY_DOMAIN_SUCCESS :
        $learn_more = l('Learn More', 'http://docs.aws.amazon.com/ses/latest/DeveloperGuide/verify-domains.html');
        $message = t("In order to complete verification of this domain, you must create a TXT record in the DNS settings for the domain, 
            with the value shown in TXT column.<br> When Amazon Web Services has confirmed that these values are present in the DNS settings for the domain, 
            the Status for the domain will change to <b>verified</b>. This may take up to 72 hours, {$learn_more}.");
        drupal_set_message($message, 'status');
        drupal_goto('admin/config/services/amazon_ses/amazon_identity_list');
        break;
      
      case KABOOTR_AMAZON_REQUEST_EXCEPTION :
        $message = t('Request to <b>!action</b> action of Amazon SES API call has failed, 
                    please check your network connection or try after some time.', array(
            '!action' => "Verify{$identity_type}Identity"
        ));
        drupal_set_message($message, 'error');
        break;
      
      case KABOOTR_AMAZON_REQUEST_FAILURE :
        $message = t('Request to <b>!action</b> action of Amazon SES API call has failed,
                    missing some parameter or Request is not valid.', array(
            '!action' => "Verify{$identity_type}Identity"
        ));
        drupal_set_message($message, 'error');
        break;
    }
  }
}

/**
 * Retrieve sender identities and their status
 */
function get_verified_list($identity_type = NULL, $bol = TRUE) {
  
  // Headers of table, when Identity type is Domain
  if ($identity_type == 'Domain') {
    $header = array(
        'key' => 'Domain Name',
        'VerificationStatus' => 'Status',
        'DomainRecordSet' => 'Domain Verification Record Set',
    );
  }
  else {
    $header = array(
        'key' => 'Email Address',
        'VerificationStatus' => 'Status'
    );
  }
  
  $form['amazon_list_identities_filter'] = array(
      '#type' => 'fieldset',
      '#title' => t('Identites belongs to'),
      '#collapsed' => FALSE,
      '#attributes' => array('class' => array('container-inline')),
  );
  $form['amazon_list_identities_filter']['select_indentity'] = array(
      '#type' => 'select',
      '#title' => t(''),
      '#options' => array(
          'EmailAddress' => 'Email Address',
          'Domain' => 'Domain',
      ),
  );
  $form['amazon_list_identities_filter']['filter_identity_list'] = array(
      '#type' => 'button',
      '#value' => t('Filter'),
  );
  // Update Option
  $form['amazon_list_identities_update'] = array(
      '#type' => 'fieldset',
      '#title' => t('Verified Idenities'),
      '#collapsed' => FALSE,
      '#attributes' => array('class' => array('container-inline')),
  );
  $form['amazon_list_identities_update']['list_table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => NULL,
      '#empty' => t('No content available.'),
  );
  
  //Get selected identity from session
  if (isset($_SESSION['amazon_selected_identity'])) {
  	$identity_type = $_SESSION['amazon_selected_identity'];
  	$actionParameter['IdentityType'] = $identity_type;
  	unset($_SESSION['amazon_selected_identity']);
  }
  
  if (!isset($_SESSION['amazon_verified_list'])) {
    
    if ($identity_type != NULL) {
      $actionParameter['IdentityType'] = $identity_type;
    }
    
    $_SESSION['amazon_verified_list'] = NULL;
    $result = NULL;
    $actionParameter['MaxItems'] = 10;
    module_load_include('inc', 'simplenews_amazon_ses', 'includes/simplenews_amazon_ses.mail');
    $result = simplenews_amazon_ses_send_request('ListIdentities', $actionParameter);
    // Display the response of calling of Amazon SES API
    if (isset($result['status'])) {
      switch($result['status']) {
        case KABOOTR_AMAZON_REQUEST_EXCEPTION :
          $message = t('Request to <b>!action</b> action of Amazon SES API call has failed, 
                    please check your network connection or try after some time.', array(
              '!action' => "ListIdentities"
          ));
          drupal_set_message($message, 'error');
          $form['exception'] = TRUE;
           return $form;
        
        case KABOOTR_AMAZON_REQUEST_FAILURE :
          $message = t('Request to <b>!action</b> action of Amazon SES API call has failed,
                    missing some parameter or Request is not valid.', array(
              '!action' => "ListIdentities"
          ));
          drupal_set_message($message, 'error');
          $form['exception'] = TRUE;
           return $form;
      }
    }
    unset($actionParameter);
    // This action is throttled at one request per second.
    // Wait for 1 second
    sleep(1);
    if (isset($result['member'])) {
      $actionParameter['Identities'] = $result['member'];
      $result = simplenews_amazon_ses_send_request('GetIdentityVerificationAttributes', $actionParameter);
      $options = $result;
      
      // Display the response of calling of Amazon SES API
      if (isset($result['status'])) {
        switch($result['status']) {
          case KABOOTR_AMAZON_REQUEST_EXCEPTION :
            $message = t('Request to <b>!action</b> action of Amazon SES API call has failed, 
                    please check your network connection or try after some time.', array(
                '!action' => "GetIdentityVerificationAttributes"
            ));
            drupal_set_message($message, 'error');
            $form['exception'] = TRUE;
            return $form;
          
          case KABOOTR_AMAZON_REQUEST_FAILURE :
            $message = t('Request to <b>!action</b> action of Amazon SES API call has failed,
                    missing some parameter or Request is not valid.', array(
                '!action' => "GetIdentityVerificationAttributes"
            ));
            drupal_set_message($message, 'error');
            $form['exception'] = TRUE;
            return $form ;
        }
      }
      unset($options['error']);
      $_SESSION['amazon_verified_list'] = $options;
    }
  }

if ($identity_type == 'Domain') {
    $learn_more = l('Learn more about verifying Emails in Amazon SES', 'http://docs.aws.amazon.com/ses/latest/DeveloperGuide/verify-email-addresses.html');
    $form['amazon_list_identities_filter']['#description'] = "To complete domain verification, you must add a <b>TXT</b> record with the displayed <b>Name</b> and <b>Value</b> to
                  your domain's DNS settings.<br> <b>Note: </b>Some domain name providers use the term <b>Host</b> instead of
                  <b>Name</b>. If your DNS provider does not allow underscores in record names, you can omit the
                  underscore before amazonses in the TXT record name. <br>
                  When verification is complete, the domain's status in the Amazon SES console will change from
                  <b>Pending</b> to <b>Verified</b>, and you will receive an Amazon SES Domain Verification
                  SUCCESS confirmation email from Amazon Web Services. (Amazon Web Services emails are sent
                  to the email address you used when you signed up for Amazon SES.) {$learn_more}.<br></br>";
  }
  elseif ($identity_type == 'EmailAddress') {
    $learn_more = l('Learn more about verifying domain in Amazon SES', 'http://docs.aws.amazon.com/ses/latest/DeveloperGuide/verify-domains.html');
    $form['amazon_list_identities_filter']['#description'] = "<li>In your email client, open the message from Amazon SES asking you to confirm that you are the owner of this email address.</li>
                  <li>Click the link in the message.</li> 
                  <li><b>Note:</b> The link in the verification message expires 24 hours after your original verification request.</li>
                  <li>The status of the email address in the Amazon SES console will change from <b>Pending</b> to <b>Verified</b>.</li><br></br>";
  }
  
  $form['amazon_list_identities_update']['list_table']['#options'] = $_SESSION['amazon_verified_list'];
  return $form; 
}

/**
 * Display Verified Sender Identites to Admin, 
 * From here admin can delete Sender Identites
 */
function amazon_identity_list_form($form, &$form_state) {
  
  if (!$form_state['rebuild']) {
    unset($_SESSION['amazon_verified_list']);
  }
  
  // If Identity type is selected, verified identies must be again reterieved
  // If filter button is clicked,
  if (isset($form_state['clicked_button']['#value'])) {
    if ($form_state['clicked_button']['#value'] == 'Filter') {
    		unset($_SESSION['amazon_verified_list']);
    }
  }
  
  // If Identity type is selected
  $identity_type = 'EmailAddress';
  if (isset($form_state['values']['select_indentity'])) {
    $identity_type = $form_state['values']['select_indentity'];
  }
  $form = get_verified_list($identity_type);
  if (isset($form['exception'])) {
    return '';
  }
  $form['amazon_list_identities_update']['select_update'] = array(
      '#type' => 'select',
      '#title' => t(''),
      '#options' =>array('DeleteIdentity' => 'Delete Identity'),
  );
  $form['amazon_list_identities_update']['update'] = array(
      '#type' => 'submit',
      '#value' => t('Send Request'),
  );
  
  $form['amazon_list_identities_filter']['select_indentity']['#default_value'] = array($identity_type); 
  
  return $form;
}

/**
 *
 */
function amazon_identity_list_form_validate($form, &$form_state) {
  $selected_identities = array_filter($form_state['values']['list_table']);
  $selected_identities_no = count($selected_identities);
  if ($selected_identities_no >5) {
  	form_set_error('list_table','Please select maximum 5 identities.');
  	return '';
  }
}
/**
 * 
 */
function amazon_identity_list_form_submit($form, &$form_state) {
  $selected_identities = array_filter($form_state['values']['list_table']);
  module_load_include('inc', 'simplenews_amazon_ses', 'includes/simplenews_amazon_ses.mail');
  $sleep_flag = 0;
  if (($form_state['clicked_button']['#value']) == 'Send Request') {
    foreach ($selected_identities as $value) {
      // Give interval of 1 second in each request
      if ($sleep_flag) {
      	sleep(1); // Sleep for one seconde
      }
      $sleep_flag = 1;
      $actionParameter['identity'] = $form_state['complete form']['amazon_list_identities_update']['list_table']['#options'][$value]['key'];
      $result = simplenews_amazon_ses_send_request('DeleteIdentity', $actionParameter); // This request is throttled per second
                                                                                          
      // Display the response of calling of Amazon SES API
      if (isset($result['status'])) {
        switch($result['status']) {
          case KABOOTR_DELETE_IDENTITY_SUCCESS :
            $message = t('Identity !identity is deleted successfully from Amazon.', array(
                '!identity' => $actionParameter['identity']
            ));
            drupal_set_message($message, 'status');
            break;
          
          case KABOOTR_AMAZON_REQUEST_EXCEPTION :
            $message = t('Request to <b>!action</b> action of Amazon SES API call has failed, 
                    please check your network connection or try after some time.', array(
                '!action' => "DeleteIdentity"
            ));
            drupal_set_message($message, 'error');
            break;
          
          case KABOOTR_AMAZON_REQUEST_FAILURE :
            $message = t('Request to <b>!action</b> action of Amazon SES API call has failed,
                    missing some parameter or Request is not valid.', array(
                '!action' => "DeleteIdentity"
            ));
            drupal_set_message($message, 'error');
            break;
        }
      }
    }
  }
}

 /**
  * Display Sending Quota and Statistics data
  */
function amazon_get_mail_statistics_form($form, $form_state) { 
  module_load_include('inc', 'simplenews_amazon_ses', 'includes/simplenews_amazon_ses.mail');
  $result_quota = simplenews_amazon_ses_send_request('GetSendQuota', $actionParameter = array());
  if (isset($result_quota['status'])) {
    switch($result_quota['status']) {
      case KABOOTR_AMAZON_REQUEST_EXCEPTION :
        $message = t('Request to <b>!action</b> action of Amazon SES API call has failed, 
                    please check your network connection or try after some time.', array(
            '!action' => "GetSendQuota"
        ));
        drupal_set_message($message, 'error');
        return '';
      case KABOOTR_AMAZON_REQUEST_FAILURE :
        $message = t('Request to <b>!action</b> action of Amazon SES API call has failed,
                    missing some parameter or Request is not valid.', array(
            '!action' => "GetSendQuota"
        ));
        drupal_set_message($message, 'error');
        return '';
    }
  }
  
  $form['sending_limits_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Your Amazon SES Sending Limits'),
      '#collapsed' => FALSE,
  );
  $form['sending_limits_fieldset']['sending_limit_data'] = array(
      '#prefix' => "<div class = sending-limit><b>Sending Quota:</b>
      	Send {$result_quota['Max24HourSend']} emails per 24 hour period <br>
      	<b>Sent Mail:</b> {$result_quota['SentLast24Hours']} <br>
      	<b>Max Send Rate:</b>	{$result_quota['MaxSendRate']} Email/second"
  );
  
  sleep(1);
  $result_statistics = simplenews_amazon_ses_send_request('GetSendStatistics', $actionParameter = array());
  if (isset($result_statistics['status'])) {
    switch($result_statistics['status']) {
    	case KABOOTR_AMAZON_REQUEST_EXCEPTION:
    	   $message = t('Request to <b>!action</b> action of Amazon SES API call has failed, 
                    please check your network connection or try after some time.', array('!action' => "GetSendStatistics"));
        drupal_set_message($message, 'error');
        return '';
    	case KABOOTR_AMAZON_REQUEST_FAILURE:
    	  $message = t('Request to <b>!action</b> action of Amazon SES API call has failed,
                    missing some parameter or Request is not valid.',array('!action' => "GetSendStatistics"));
        drupal_set_message($message, 'error');
        return '';
    }
  }
  $form['sending_statistics_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Your Amazon SES Metrics'),
      '#collapsed' => FALSE,
  );
  $form['sending_statistics_fieldset']['sending_statistics_data'] = array(
      '#prefix' => "<div class = sending-limit><b>Total number of mails sent:</b>
      {$result_statistics['DeliveryAttempts']} in last two weeks <br>
      <b>Total number of Bounces Mail:</b>
      {$result_statistics['Bounces']} in last two weeks <br>
      <b>Total number of Complaints mail:</b>
      {$result_statistics['Complaints']} in last two weeks <br>
      <b>Total number of Rejected mail:</b>
      {$result_statistics['Rejects']} in last two weeks <br>"
      );
  return $form;
} 

/**
 * Form for Enablling DKIM for sender's Identity
 */
function amazon_domain_dkim_enable_form($form, &$form_state) {
  
  // Form will be rebuild, when we click on Get Token, then there is no need of again sending request to
  // get verified identies, it will be reterieved form session
  if (!$form_state['rebuild']) {
  	unset($_SESSION['amazon_verified_list']);
  }
  
  // If Identity type is selected, verified identies must be again reterieved
  // If filter button is clicked,
  if (isset($form_state['clicked_button']['#value'])) {
  	if ($form_state['clicked_button']['#value'] == 'Filter') {
  		unset($_SESSION['amazon_verified_list']);
  	}
  } 
  $identity_type = 'Domain';
  if (isset($form_state['values']['select_indentity'])) {
    $identity_type = $form_state['values']['select_indentity'];
  }
 
  // Retrieve verified list
  $form = get_verified_list($identity_type);
  if (isset($form['exception'])) {
  	return '';
  }
  $form['amazon_list_identities_filter']['select_indentity']['#default_value'] = array($identity_type);
  
  $form['amazon_list_identities_update']['select_update'] = array(
      '#type' => 'select',
      '#title' => t(''),
      '#options' => array(
          'DkimAttributes' => 'Get CNAME record'
      ),
  );
  $form['amazon_list_identities_update']['update'] = array(
      '#type' => 'button',
      '#value' => t('Send Request'),
  );
  
  $form['amazon_list_identities_update']['#title'] = t('DKIM Settings');
  $form['amazon_list_identities_update']['#description'] = t("DomainKeys Identified Mail (DKIM) is a standard that allows senders to sign
                     their email messages and ISPs to use those signatures to verify that those messages are legitimate and have not been
                     modified by a third party in transit.<br>
                     To set up DKIM, you must update your domain's DNS settings with the CNAME record information. For obtaining CNAME record
                     select Domain or Email addrees then click on <b>Send Request</b>.
                     <br></br>");
  
  $form['amazon_list_identities_update']['list_table']['#multiple'] = FALSE;
  $form['amazon_list_identities_update']['list_table']['#input'] = FALSE;
  
  $form['amazon_list_identities_update']['update']['#ajax'] = array(
  	'callback' => 'amazon_domain_dkim_enable_callback',
      'wrapper' => 'domain-dkim-wrapper',
      'effect' => 'fade',
      'method' => 'html',
  );
  $header = array(
      'name' => 'Name',
      'type' => 'Type',
      'value' => 'Value',
  );
  $form['div'] = array(
      '#prefix' => '<div id = "domain-dkim-wrapper">',
      '#suffix' => '</div>',
  );
  $form['div']['domain_dkim_info'] = array(
      '#prefix' => '',
      '#suffix' => '',
  );
  $form['div']['domain_dkim_token_table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => NULL,
      '#empty' => t('No content available.'),
      '#access' => FALSE,
  );
  return $form;
}

/**
 * Ajax callback for domain_dkim_enable
 */
function amazon_domain_dkim_enable_callback($form, &$form_state) {
  return $form['div'];
}

/**
 *
 */
function amazon_domain_dkim_enable_form_validate($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == 'Send Request') {
    if ($form_state['values']['list_table'] == '' ||$form_state['values']['list_table'] == NULL) {
      form_set_error('list_table','You must select one identity for generating DKIM tokens');
    }
  }
}
