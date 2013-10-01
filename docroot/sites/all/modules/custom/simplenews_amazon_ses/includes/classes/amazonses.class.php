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
    	case 'GetIdentityVerificationAttributes' :
    	  $result = $this->getIdentityVerificationAttributes('', FALSE, $actionResponse, $responseCode);
    	  break;
    	case 'VerifyEmailIdentity' :
    	  $result = $this->verifyEmailIdentity('', FALSE, $actionResponse, $responseCode);
    	  break;
    	case 'VerifyDomainIdentity' :
    	  $result = $this->verifyDomainIdentity();
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
      case 'GetIdentityVerificationAttributes' :
        $this->getIdentityVerificationAttributes($actionParameter, TRUE);
        break;
      case 'VerifyEmailIdentity' :
        $this->verifyEmailIdentity($actionParameter, TRUE);
        break;
      case 'VerifyDomainIdentity' :
        $this->verifyDomainIdentity($actionParameter, TRUE);
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
      case 'GetIdentityNotificationAttributes' :
        $this->getIdentityNotificationAttributes($actionParameter, TRUE);
        break;
      case 'GetSendStatistics':
        $this->getSendStatistics($actionParameter, TRUE);
        break;
      case 'GetSendQuota':
        $this->getSendQuota($actionParameter, TRUE);
    }
  }
  
  /**
   * Implmetns Query Action GetIdentityVerificationAttributes,This action is throttled at one request per second.
   * @param Array $actionParameter Given a list of identities (email addresses and/or domains), 
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return Array: returns the verification status and (for domain
   *   identities) the verification token for each identity.
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_GetIdentityVerificationAttributes.html  
   */
  private function getIdentityVerificationAttributes($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $n = 1;
      $this->setRequestParameter('Action', 'GetIdentityVerificationAttributes');
      if (isset($actionParameter['identity'])) {
      	$this->setRequestParameter('Identities.member.' . $n, $actionParameter['identity']);
      }
    }
    else {
      if ($responseCode == '200') {
        if (isset($actionResponse->VerificationAttributes->entry)) {
          $result = array();
          $verificationStatus = $actionResponse->VerificationAttributes->entry->value->VerificationStatus;
          switch($verificationStatus) {
            case 'Success' :
              $result['status'] =  KABOOTR_IDENTITY_VERIFICATION_SUCCESS;
              break;
            case 'Pending' :
              $result['status'] =  KABOOTR_IDENTITY_VERIFICATION_PENDING;
              break;
          }
        }
        else {
            $result['status'] =  KABOOTR_IDENTITY_VERIFICATION_NOT;
        }
      }
      return $result;
    }
  }
  
  /**
   * Implmetns Query Action VerifyEmailIdentity,
   * Verifies an email address. This action causes a confirmation email message to be sent to the specified
   * address. This action is throttled at one request per second.
   * @param Array $actionParameter Given a identitiy(email addresse or domain),
   * @param Boolean $request
   * @param string $actionResponse
   * @param string $responseCode
   * @return Array: returns the verification status
   * @link http://docs.aws.amazon.com/ses/latest/APIReference/API_VerifyEmailIdentity.html
   */
  
  private function verifyEmailIdentity($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $this->setRequestParameter('Action', 'VerifyEmailIdentity');
      if (isset($actionParameter['identity'])) {
        $this->setRequestParameter('EmailAddress', $actionParameter['identity']);
      }
    }
    else {
      $result = array();
      if ($responseCode == '200') {
        $result['status'] = KABOOTR_VERIFY_EMAIL_SUCCESS;
        return $result;
      }
    }
  }
  
  /**
   * Implmetns Query Action SendEmail,
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
        $result['status'] = KABOOTR_MAIL_NOT_SENT;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Implmetns Query Action SetIdentityFeedbackForwardingEnabled,
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
   * Implmetns Query Action SetIdentityNotificationTopic,
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
   * Implmetns Query Action GetIdentityNotificationAttributes,
   * This action is throttled at one request per second.
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
   * Implmetns Query Action GetSendStatistics,
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
        $result['status'] = KABOOTR_MAIL_NOT_SENT;
        $result['error'] = TRUE;
        return $result;
      }
    }
  }
  
  /**
   * Implmetns Query Action GetSendQuota,
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
        $result['SentLast24Hours'] = $actionResponse->SentLast24Hours;
        $result['Max24HourSend'] = $actionResponse->Max24HourSend;
        $result['MaxSendRate'] = $actionResponse->MaxSendRate;
        return $result;
      }
      // Error in response
      else {
        $result['Type'] = $actionResponse->Type;
        $result['Code'] = $actionResponse->Code;
        $result['Message'] = $actionResponse->Message;
        $result['status'] = KABOOTR_MAIL_NOT_SENT;
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