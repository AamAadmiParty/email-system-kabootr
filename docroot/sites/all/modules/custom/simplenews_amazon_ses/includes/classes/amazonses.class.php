<?php

/**
 * 
 * @author tkuldeep
 *
 */
Class SimpleEmailService {
  
  private $requestParameters = array(); //Any request parameters.
  private $queryAction = ''; //The action you want to perform on the endpoint, such as sending a message
  private $requestHeaders = array();
  private $postFields = array();

  const queryEndPoint = 'https://email.us-east-1.amazonaws.com'; //The resource the request is acting on.
  const requestContentType = 'application/x-www-form-urlencoded';
  
  // These security credential is depented to AWS account or it should be sepicific
  // sender email or domain
  const accessKeyId = 'AKIAJ4EMNFQWY3IACPEQ';
  const secretAccessKey = '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';

  private function setRequestParameter($key, $value) {
    $this->requestParameters['AWSAccessKeyId'] = self::accessKeyId;
    $this->requestParameters[$key] = $value;
    
  }
  public function getRequestParameter() {
    return $this->requestParameters;
  }
  
  public function getRequestHeaders() {
    return $this->requestHeaders;
  }
  
  public function getPostFields() {
    return $this->postFields;
  }
  
  private function setPostFields($postFields) {
    $this->postFields = $postFields;
    
  }

  // Method for setting request headers
  private function setRequestHeaders($dateValue, $signature) {
      $this->requestHeaders['Content-Type'] = self::requestContentType;
    //  $this->requestHeaders['Content-Length'] = '174';
      $this->requestHeaders['Date'] = $dateValue;
      $accessKeyId = self::accessKeyId;
      $this->requestHeaders['X-Amzn-Authorization'] = "AWS3-HTTPS "
          ."AWSAccessKeyId={$accessKeyId},"
          ."Algorithm=HmacSHA1,Signature={$signature}";
      $this->requestHeaders['Timestamp'] = date('YYYY-MM-DDThh:mm:ssZ');
      //$this->requestHeaders['SignatureVersion'] = $value;
      $this->requestHeaders['Version'] =  '2010-12-01';
  }

  // Create HMAC-SHA Signatures for X-Amzn-Authorization HTTP header.
  private function getSignature($date) {
    $signature = base64_encode(hash_hmac('sha1', $date, self::secretAccessKey, TRUE));
    return $signature;
  }
  
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
  // Add required parameter and header to the Query according to
  // Query action name
  // @param identity can be email or domain name
  
  public function createQueryRequest($queryAction, $actionParameter = array()) {

    // must be in format 'Thu, 26 Sep 2013 14:26:55 +0530'
    $date =  date(DATE_RFC2822);
    $signature = $this->getSignature($date);
    $this->setRequestHeaders($date, $signature);
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
        $result = $this->setIdentityFeedbackForwardingEnabled($actionParameter, TRUE);
        break;
      case 'SetIdentityNotificationTopic' :
        $result = $this->setIdentityNotificationTopic($actionParameter, TRUE);
        break;
      case 'GetIdentityNotificationAttributes' :
        $result = $this->getIdentityNotificationAttributes($actionParameter, TRUE);
        break;
      case 'GetSendStatistics':
        $result = $this->getSendStatistics($actionParameter, TRUE);
        break;
      case 'GetSendQuota':
          $result = $this->getSendQuota($actionParameter, TRUE);
    }
  }
  
  //TODO
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
  // TODO
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
  
  // TODO
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
  
  // TODO
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
  
  // TODO
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
  
  // TODO
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

  // TODO
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
  
  //TODO
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