<?php

/**
 * This class used to make appropriate request, 
 * set request headers and read the response send by amazon 
 * @author tkuldeep
 *
 */
Class SimpleEmailService {
  
  private $requestParameters = array(); //Specific request parameters to action.
  private $queryAction = '';            //The action you want to perform on the endpoint, such as sending a message
  private $requestHeaders = array();    // Common request parameters to all actions

  const queryEndPoint = 'https://email.us-east-1.amazonaws.com'; //The resource the request is acting on.
  const requestContentType = 'application/x-www-form-urlencoded';
  
  // These security credential is depented to AWS account or it should be sepicific
  // sender email or domain, 
  // Access keys will be saved in database or in csv file [Pending] 
  const accessKeyId = 'AKIAJ4EMNFQWY3IACPEQ';
  const secretAccessKey = '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';

  private function setRequestParameter($key, $value) {
    $this->requestParameters[$key] = $value;
  }
  public function getRequestParameter() {
    return $this->requestParameters;
  }
  
  public function getRequestHeaders() {
    return $this->requestHeaders;
  }
  
  // Method for setting request headers
  private function setRequestHeaders($dateValue, $signature) {
      $this->requestHeaders['Content-Type'] = self::requestContentType;
      $this->requestHeaders['Date'] = $dateValue;
      $accessKeyId = self::accessKeyId;
      $this->requestHeaders['X-Amzn-Authorization'] = "AWS3-HTTPS "
          ."AWSAccessKeyId={$accessKeyId},"
          ."Algorithm=HmacSHA1,Signature={$signature}";
      $this->requestHeaders['Timestamp'] = date('YYYY-MM-DDThh:mm:ssZ');
      $this->requestHeaders['Version'] =  '2010-12-01';
      $this->requestHeaders['AWSAccessKeyId'] = self::accessKeyId;
  }

  // Create HMAC-SHA Signatures for X-Amzn-Authorization HTTP header.
  private function getSignature($date) {
    $signature = base64_encode(hash_hmac('sha1', $date, self::secretAccessKey, TRUE));
    return $signature;
  }
  
  /**
   * Parse the result xmlObject response, 
   * @param String  $queryAction Name of query action which is to be called
   * @param XMLObject  $actionResponse Response returned by query
   * @param String $responseCode Http response code
   * @return parsed xmlobject in associative array, key corresponds to response element.
   */
  public function getQueryResponse($queryAction, $actionResponse, $responseCode) {

    $result = '';
    switch($queryAction) {
      case 'DeleteIdentity' :
        $result = $this->deleteIdentity('', FALSE, $actionResponse, $responseCode);
        break;
      case 'GetIdentityDkimAttributes' :
        $result = $this->getIdentityDkimAttributes('', FALSE, $actionResponse, $responseCode);
        break;
      case 'GetIdentityVerificationAttributes' :
        $result = $this->getIdentityVerificationAttributes('', FALSE, $actionResponse, $responseCode);
    	  break;
    	case 'VerifyEmailIdentity' :
    	  $result = $this->verifyEmailIdentity('', FALSE, $actionResponse, $responseCode);
    	  break;
    	case 'VerifyDomainIdentity' :
    	  $result = $this->verifyDomainIdentity('', FALSE, $actionResponse, $responseCode);
    	  break;
    	case 'SendEmail' :
    	  $result = $this->sendEmail('', FALSE, $actionResponse, $responseCode);
    	  break;
      case 'SetIdentityFeedbackForwardingEnabled' :
        $result = $this->setIdentityFeedbackForwardingEnabled('', FALSE, $actionResponse, $responseCode);
        break;
      case 'SetIdentityNotificationTopic' :
        $result = $this->setIdentityNotificationTopic('', FALSE, $actionResponse, $responseCode);
        break;
      case 'GetIdentityNotificationAttributes' :
        $result = $this->getIdentityNotificationAttributes('', FALSE, $actionResponse, $responseCode);
        break;
      case 'GetSendStatistics' :
        $result = $this->getSendStatistics('', FALSE, $actionResponse, $responseCode);
        break;
      case 'GetSendQuota' :
        $result = $this->getSendQuota('', FALSE, $actionResponse, $responseCode);
        break;
      case 'ListIdentities':
        $result = $this->listIdentities('', FALSE, $actionResponse, $responseCode);
        break;
        
    }
    return $result;
  }
  
  /**
   * Add required parameter and header to the Query according to
   * Query action name
   * @param String $queryAction Name of query action which is to be called
   * @param Array $actionParameter Specific parameter related to Query Action
   */
  
  public function createQueryRequest($queryAction, $actionParameter = array()) {

    // must be in format 'Thu, 26 Sep 2013 14:26:55 +0530'
    $date =  date(DATE_RFC2822);
    $signature = $this->getSignature($date);    // get HMAC-SHA Signatures
    $this->setRequestHeaders($date, $signature); // Set common request parameter
    
    switch($queryAction) {
      case 'DeleteIdentity' :
        $this->deleteIdentity($actionParameter, TRUE);
        break;
      case 'GetIdentityDkimAttributes' :
        $this->getIdentityDkimAttributes($actionParameter, TRUE);
        break;
      case 'GetIdentityNotificationAttributes' :
        $this->getIdentityNotificationAttributes($actionParameter, TRUE);
        break;
      case 'GetIdentityVerificationAttributes' :
        $this->getIdentityVerificationAttributes($actionParameter, TRUE);
        break;
      case 'GetSendStatistics' :
        $this->getSendStatistics($actionParameter, TRUE);
        break;
      case 'GetSendQuota' :
        $this->getSendQuota($actionParameter, TRUE);
        break;
      case 'ListIdentities' :
        $this->listIdentities($actionParameter, TRUE);
        break;
      case 'SendEmail' :
        $this->sendEmail($actionParameter, TRUE);
        break;
      case 'SetIdentityFeedbackForwardingEnabled' :
        $this->setIdentityFeedbackForwardingEnabled($actionParameter, TRUE);
        break;
      case 'SetIdentityNotificationTopic' :
        $this->setIdentityNotificationTopic($actionParameter, TRUE);
        break;
      case 'VerifyDomainDkim' :
        $this->verifyDomainDkim($actionParameter, TRUE);
        break;
      case 'VerifyDomainIdentity' :
        $this->verifyDomainIdentity($actionParameter, TRUE);
        break;
      case 'VerifyEmailIdentity' :
        $this->verifyEmailIdentity($actionParameter, TRUE);
        break;
    }
  }
  
  /**
   * Call Query API action DeleteIdentity,
   * Deletes the specified identity (email address or domain) from the list of verified identities.
   * This action is throttled at one request per second.
   * @param Array $actionParameter Given a identitiy(email addresse or domain),
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return Array: Status of Query call
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_DeleteIdentity.html
   */
  private function deleteIdentity($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'DeleteIdentity');
      if (isset($actionParameter['identity'])) {
        $this->setRequestParameter('Identity', $actionParameter['identity']);
      }
    }
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['status'] = KABOOTR_DELETE_IDENTITY_SUCCESS;
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action GetIdentityNotificationAttributes,
   * This action is throttled at one request per second.
   * @param Array $actionParameter Given a list of verified identities (email addresses and/or domains)
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return An array describing identity notification attributes.
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_GetIdentityNotificationAttributes.html
   */
  private function getIdentityDkimAttributes($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $n = 1;
      $this->setRequestParameter('Action', 'GetIdentityDkimAttributes');
      if (isset($actionParameter['Identities'])) {
        foreach ($actionParameter['Identities'] AS $member) {
          $this->setRequestParameter('Identities.member.' . $n, $member);
          $n++;
        }
      }
    }
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $entry = $actionResponse->DkimAttributes->entry;
        $result['key'] = (string) $entry->key;
        $result['DkimEnabled'] = (string) $entry->value->DkimEnabled;
        $result['DkimVerificationStatus'] = (string) $entry->value->DkimVerificationStatus;
        $dkimTokens = $entry->value->DkimTokens->member;
        $i = 0;
        if (strpos($result['key'], '@') != FALSE) {
          $temp_arr = explode('@', $result['key']);
          $domain = $temp_arr[1];
        }
        else {
          $domain = $result['key'];
        }
        foreach ($dkimTokens as $token) {
          $name = (string) $token . '._domainkey.' . $domain;
          $value = (string) $token . '.dkim.amazonses.com';
          $result['member']['row' . $i]['name'] = $name;
          $result['member']['row' . $i]['value'] = $value;
          $result['member']['row' . $i]['type'] = 'CNAME';
          $i ++;
        }
        $result['status'] = '';
        return $result;
      }
      
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action GetIdentityNotificationAttributes,
   * This action is throttled at one request per second.
   * 
   * @param Array $actionParameter Given a list of verified identities (email addresses and/or domains)
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return An array describing identity notification attributes.
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_GetIdentityNotificationAttributes.html
   */
  private function getIdentityNotificationAttributes($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $n = 1;
      $this->setRequestParameter('Action', 'GetIdentityNotificationAttributes');
      if (isset($actionParameter['identity']['member'])) {
        foreach ($actionParameter['identity']['member'] AS $member)
        $this->setRequestParameter('Identities.member.' . $n, $member);
        $n++;
      }
    }
  }

 /**
   * Call Query API action GetIdentityVerificationAttributes,
   * This action is throttled at one request per second.
   * @param Array $actionParameter Given a list of identities (email addresses and/or domains), 
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return Array: The verification status and (for domain
   *   identities) the verification token for each identity.
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_GetIdentityVerificationAttributes.html  
   */
  private function getIdentityVerificationAttributes($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $n = 1;
      $this->setRequestParameter('Action', 'GetIdentityVerificationAttributes');
      if (isset($actionParameter['Identities'])) {
        foreach ($actionParameter['Identities'] as $member) {
          $this->setRequestParameter('Identities.member.' . $n, $member);
          $n++;
        }
      }
    }
    // Parse the http response
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $entries = $actionResponse->VerificationAttributes->entry;
        $i = 0;
        foreach ($entries as $entry) {
          $result['row' . $i]['key'] = (string)$entry->key;
          $value = $entry->value;
          if (isset($value->VerificationStatus)) {
          	$result['row' . $i]['VerificationStatus'] = (string)$value->VerificationStatus;
          }
          // The verification token for a domain identity. Null for email address identities.
          if (isset($value->VerificationToken)) {
            $domain = $result['row' . $i]['key'];
            $domain_record_set = "<div class = ''><b>Name: </b> _amazonses.{$domain} <br>
                                 <b>Type:</b> TXT <br>
                                 <b>Value:</b> {$value->VerificationToken}";
          	$result['row' . $i]['DomainRecordSet'] = $domain_record_set;
          }
          $i++;
        }
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  /**
   * Call Query API action GetSendStatistics,
   * The result is a list of data points, representing the last two weeks of sending activity.
   * Each data point in the list contains statistics for a 15-minute interval.
   * This action is throttled at one request per second.
   * @param Array $actionParameter Given a list of verified identities (email addresses and/or domains)
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return An array containg the user's sending statistics. 
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_GetSendStatistics.html
   */
  
  private function getSendStatistics($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'GetSendStatistics');
    }
    // Parse the http response
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $member = $actionResponse->SendDataPoints->member;
        $result['DeliveryAttempts'] = 0;  //Number of emails that have been enqueued for sending.
        $result['Rejects'] = 0;           //Number of emails rejected by Amazon SES.
        $result['Bounces'] = 0;           //Number of emails that have bounced.
        $result['Complaints'] = 0;        //Number of unwanted emails that were rejected by recipients.
        foreach ($member as $value) {
          $result['DeliveryAttempts'] = $value->DeliveryAttempts + $result['DeliveryAttempts'];
          $result['Rejects'] = $value->Rejects + $result['Rejects'];
          $result['Bounces'] = $value->Bounces + $result['Bounces'];
          $result['Complaints'] = $value->Complaints + $result['Complaints'];
        }
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action GetSendQuota,
   * This action is throttled at one request per second.
   * @param Array $actionParameter Empty array
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return An array containg user's current sending limits.
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_GetSendQuota.html
   */
  private function getSendQuota($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'GetSendQuota');
    }
    // Parse the http response
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $result['SentLast24Hours'] = (string)$actionResponse->SentLast24Hours;
        $result['Max24HourSend'] = (string)$actionResponse->Max24HourSend;
        $result['MaxSendRate'] = (string)$actionResponse->MaxSendRate;
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action ListIdentities,
   * This action is throttled at one request per second.
   * @param Array $actionParameter The type of the identities to list or all
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return A list containing all of the identities (email addresses and domains) for a specific AWS Account,
   *  regardless of verification status.
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_GetSendQuota.html
   */
  private function listIdentities($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'ListIdentities');
      if (isset($actionParameter['IdentityType'])) {
      $this->setRequestParameter('IdentityType', $actionParameter['IdentityType']);
      }
      if (isset($actionParameter['MaxItems'])) {
        $this->setRequestParameter('MaxItems', $actionParameter['MaxItems']);
      }
      if (isset($actionParameter['NextToken'])) {
        $this->setRequestParameter('NextToken', $actionParameter['NextToken']);
      }
    }
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $members = $actionResponse->Identities->member;
        foreach ($members as $member) {
          $result['member'][] = (string)$member;
        }
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action SendEmail,
   * Composes an email message based on input data, and then immediately queues the message for sending.
   * @param Array $actionParameter SimpleEmailServiceMessage object,
   * This object contains Destination, Message, ReplyToAddresses.member.N, ReturnPath, Source
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return Array: returns the verification status
   * @link For more detail http://docs.aws.amazon.com/ses/latest/APIReference/API_SendEmail.html
   */
  
  private function sendEmail($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    // Set query parameter to send http request
    if ($request && isset($actionParameter['simpleEmailServiceMessage'])) {
      $simpleEmailServiceMessage = $actionParameter['simpleEmailServiceMessage'];
      $this->setRequestParameter('Action', 'SendEmail');
      $i = 1;
      if ($simpleEmailServiceMessage->to != NULL) {
        $this->setRequestParameter('Destination.ToAddresses.member.' . $i, $simpleEmailServiceMessage->to);
      }
  
      if ($simpleEmailServiceMessage->replyto != NULL) {
        $this->setRequestParameter('ReplyToAddresses.member.' . $i, $simpleEmailServiceMessage->replyto);
      }
  
      // $this->setRequestParameter('Source', $simpleEmailServiceMessage->from);
      $this->setRequestParameter('Source', 'tkuldeep.singh@innoraft.com');
  
      if ($simpleEmailServiceMessage->returnpath != NULL) {
        $this->setRequestParameter('ReturnPath', $simpleEmailServiceMessage->returnpath);
      }
  
      if ($simpleEmailServiceMessage->subject != NULL && strlen($simpleEmailServiceMessage->subject) > 0) {
        $this->setRequestParameter('Message.Subject.Data', $simpleEmailServiceMessage->subject);
        if ($simpleEmailServiceMessage->subjectCharset != NULL && strlen($simpleEmailServiceMessage->subjectCharset) > 0) {
          $this->setRequestParameter('Message.Subject.Content.Charset', $simpleEmailServiceMessage->subjectCharset);
        }
      }
  
      if ($simpleEmailServiceMessage->messagetext != NULL && strlen($simpleEmailServiceMessage->messagetext) > 0) {
        $this->setRequestParameter('Message.Body', $simpleEmailServiceMessage->messagetext);
        if ($simpleEmailServiceMessage->messageTextCharset != NULL && strlen($simpleEmailServiceMessage->messageTextCharset) > 0) {
          $this->setRequestParameter('Message.Body.Text.Content.Charset', $simpleEmailServiceMessage->messageTextCharset);
        }
      }
  
      if ($simpleEmailServiceMessage->messagehtml != NULL && strlen($simpleEmailServiceMessage->messagehtml) > 0) {
        $this->setRequestParameter('Message.Body.Html.Data', $simpleEmailServiceMessage->messagehtml);
        if ($simpleEmailServiceMessage->messageHtmlCharset != NULL && strlen($simpleEmailServiceMessage->messageHtmlCharset) > 0) {
          $this->setRequestParameter('Message.Body.Html.Charset', $simpleEmailServiceMessage->messageHtmlCharset);
        }
      }
    }
  
    // Parse the http response
    else {
      $result = array();
      if ($responseCode == '200' && isset($actionResponse->MessageId)) {
        $result['status'] = KABOOTR_MAIL_SENT;
        $result['error'] = FALSE;
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action SetIdentityFeedbackForwardingEnabled,
   * Given an identity (email address or domain), enables or disables whether Amazon SES forwards feedback notifications as email. Feedback forwarding may only be disabled when both complaint and bounce topics are set.
   *  This action is throttled at one request per second.
   * @param Array $actionParameter Given identity
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_SetIdentityFeedbackForwardingEnabled.html
   */
  private function setIdentityFeedbackForwardingEnabled($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'SetIdentityFeedbackForwardingEnabled');
      if (isset($actionParameter['identity'])) {
        $this->setRequestParameter('Identity', $actionParameter['identity']);
      }
  
      if (isset($actionParameter['forwardingEnabled'])) {
        $this->setRequestParameter('ForwardingEnabled', $actionParameter['forwardingEnabled']);
      }
    }
  }
  
  /**
   * Call Query API action SetIdentityNotificationTopic,
   * Given an identity (email address or domain), sets the Amazon SNS topic to which Amazon SES will publish bounce and complaint notifications for emails sent with that identity as the Source. Publishing to topics may only be disabled when feedback forwarding is enabled.
   * This action is throttled at one request per second.
   * @param Array $actionParameter Given identity
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_SetIdentityNotificationTopic.html
   */
  private function setIdentityNotificationTopic($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'SetIdentityNotificationTopic');
      if (isset($actionParameter['identity'])) {
        $this->setRequestParameter('Identity', $actionParameter['identity']);
      }
  
      if (isset($actionParameter['snsTopic'])) {
        $this->setRequestParameter('SnsTopic', $actionParameter['snsTopic']);
      }
    }
  
  }
  
  /**
   * Call Query API action VerifyDomainDkim,
   * This action is throttled at one request per second.
   * @param Array $actionParameter Empty array
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return A set of DKIM tokens for a domain,
   * DKIM tokens are character strings that represent your domain's
   * identity. Using these tokens, you will need to create DNS CNAME records that point to DKIM public keys
   * hosted by Amazon SES. 
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_VerifyDomainDkim.html
   */
  private function verifyDomainDkim($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'VerifyDomainDkim');
      if (isset($actionParameter['Domain'])) {
        $this->setRequestParameter('Domain', $actionParameter['Domain']);
      }
    }
    // Parse the http response
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $dkimTokens = $actionResponse->DkimTokens;
        foreach ($dkimTokens AS $member) {
          $result[]['member'] = (string)$member;
        }
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action VerifyEmailIdentity,
   * Verifies an email address. This action causes a confirmation email message to be sent to the specified
   * address. This action is throttled at one request per second.
   * @param Array $actionParameter Given a identitiy(email addresse or domain),
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return Array: the verification status
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_VerifyEmailIdentity.html
   */
  private function verifyEmailIdentity($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'VerifyEmailIdentity');
      if (isset($actionParameter['EmailAddress'])) {
        $this->setRequestParameter('EmailAddress', $actionParameter['EmailAddress']);
      }
    }
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $result['status'] = KABOOTR_VERIFY_EMAIL_SUCCESS;
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Call Query API action VerifyDomaindentity,
   * Verifies a domain. 
   * This action is throttled at one request per second.
   * @param Array $actionParameter Given a identitiy(email addresse or domain),
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return Array: A TXT record that must be placed in the DNS settings for the domain, in order to complete domain
      verification.
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_VerifyDomainIdentity.html
   */
  private function verifyDomainIdentity($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'VerifyDomainIdentity');
      if (isset($actionParameter['Domain'])) {
        $this->setRequestParameter('Domain', $actionParameter['Domain']);
      }
    }
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['error'] = FALSE;
        $result['status'] = KABOOTR_VERIFY_DOMAIN_SUCCESS;
        $result['token'] = (string)$actionResponse->VerificationToken;
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_AMAZON_REQUEST_FAILURE;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
}


final class SimpleEmailServiceMessage {

  // these are public for convenience only
  // these are not to be used outside of the SimpleEmailService class!
  public  $to, $replyto;
  public  $from, $returnpath;
  public   $subject, $messagetext, $messagehtml;
  public   $subjectCharset, $messageTextCharset, $messageHtmlCharset;

  function __construct($message) {
    
    $this->to = $message['to'];
    $this->replyto = $message['replyto'];

    $this->from = $message['from'];
    $this->returnpath = $message['return_path'];

   // $this->subject = $message['subject'];
    $this->subject = 'Subject';
    $this->messagetext = $message['body'];
    $this->messagehtml = $message['htmltext'];

    $this->subjectCharset = NULL;
    $this->messageTextCharset = NULL;
    $this->messageHtmlCharset = NULL;
  }
}