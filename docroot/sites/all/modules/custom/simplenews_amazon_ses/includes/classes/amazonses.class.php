<?php

Class SimpleEmailService {
  
  private $requestParameters = array(); //Any request parameters.
  private $queryAction = ''; //The action you want to perform on the endpoint, such as sending a message
  private $requestHeaders = array();
  private $postFields = array();

  const queryEndPoint = 'https://email.us-east-1.amazonaws.com'; //The resource the request is acting on.
  const requestContentType = 'application/x-www-form-urlencoded';
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
    	case 'SendEmail':
    	  $result = $this->sendEmail('', FALSE, $actionResponse, $responseCode);
    }
    return $result;
  }
  // Add required parameter and header to the Query according to
  // Query action name
  
  public function createQueryRequest($queryAction,$email = '', $simpleEmailServiceMessage = NULL) {

    // must be in format 'Thu, 26 Sep 2013 14:26:55 +0530'
    $date =  date(DATE_RFC2822);
    $signature = $this->getSignature($date);
    $this->setRequestHeaders($date, $signature);
    
    switch($queryAction) {
      case 'GetIdentityVerificationAttributes' :
        $this->getIdentityVerificationAttributes($email, TRUE);
        break;
      case 'VerifyEmailIdentity' :
        $this->verifyEmailIdentity($email, TRUE);
        break;
      case 'VerifyDomainIdentity' :
        $this->verifyDomainIdentity();
        break;
      case 'SendEmail':
        $this->sendEmail($simpleEmailServiceMessage, TRUE);
    }
  }
  
  //TODO
  private function getIdentityVerificationAttributes($email, $request, $actionResponse = '', $responseCode = '0') {
    if ($request) {
      $n = 1;
      $this->setRequestParameter('Action', 'GetIdentityVerificationAttributes');
      $this->setRequestParameter('Identities.member.' . $n, $email);
    }
    else {
      if ($responseCode == '200') {
        if (isset($actionResponse->VerificationAttributes->entry)) {
          $verificationStatus = $actionResponse->VerificationAttributes->entry->value->VerificationStatus;
          switch($verificationStatus) {
            case 'Success' :
              return KABOOTR_IDENTITY_VERIFICATION_SUCCESS;
            case 'Pending' :
              return KABOOTR_IDENTITY_VERIFICATION_PENDING;
          }
        }
        else {
            return KABOOTR_IDENTITY_VERIFICATION_NOT;
        }
      }
    }
  }
  // TODO
  private function verifyEmailIdentity($email) {
    $this->setRequestParameter('Action', 'VerifyEmailIdentity');
    $this->setRequestParameter('EmailAddress', $email);
  }
  
  // TODO
  private function sendEmail($simpleEmailServiceMessage, $request, $actionResponse = '', $responseCode = '0') {
    // Set query parameter to send http request
    
    if ($request) {
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