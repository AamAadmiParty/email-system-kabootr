<?php 

class SimpleNotificationService {
  
	private $requestParameters = array(); //Specific request parameters to action.
	private $queryAction = '';            //The action you want to perform on the endpoint, such as sending a message
	private $requestHeaders = array();    // Common request parameters to all actions
	
	const queryEndPoint = 'https://sns.us-east-1.amazonaws.com'; //The resource the request is acting on.
	const requestContentType = 'application/x-www-form-urlencoded';
	
	// These security credential is depented to AWS account or it should be sepicific
	// sender email or domain,
	// Access keys will be saved in database or in csv file [Pending]
	private $accessKeyId = '';
	private $secretAccessKey = '';
	
	public function __construct() {
		$path = drupal_get_path('module', 'simplenews_amazon_ses');
		$file_name = $_SERVER['DOCUMENT_ROOT'] . '/' . $path . "/amazon_credential.txt";
		if (file_exists($file_name)) {
			$string = file_get_contents($file_name);
			$string_array = explode(',', $string);
			$this->accessKeyId = $string_array[0];
			$this->secretAccessKey = $string_array[1];
		}
	}
	
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
		$this->requestHeaders['X-Amzn-Authorization'] = "AWS3-HTTPS "
		."AWSAccessKeyId={$this->accessKeyId},"
		."Algorithm=HmacSHA1,Signature={$signature}";
		$this->requestHeaders['Timestamp'] = date('YYYY-MM-DDThh:mm:ssZ');
		$this->requestHeaders['Version'] =  '2010-12-01';
		$this->requestHeaders['AWSAccessKeyId'] = $this->accessKeyId;
	}
	
	// Create HMAC-SHA Signatures for X-Amzn-Authorization HTTP header.
		private function getSignature($date) {
		$signature = base64_encode(hash_hmac('sha1', $date, $this->secretAccessKey, TRUE));
		return $signature;
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
  	$signature = $this->getSignature($date);     // get HMAC-SHA Signatures
  	$this->setRequestHeaders($date, $signature); // Set common request parameter

  	switch($queryAction) {
  		case 'ConfirmSubscription' :
  			$this->confirmSubscription($actionParameter, TRUE);
  			break;
  	}
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
  		case 'ConfirmSubscription':
  			$result = $this->confirmSubscription('', FALSE, $actionResponse, $responseCode);
  			break;
  		}
  		return $result;
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
  private function confirmSubscription($actionParameter, $request, $actionResponse = '', $responseCode = '0') {
  	if ($request) {
  		$this->setRequestParameter('Action', 'ConfirmSubscription');
  		if (isset($actionParameter['Token'])) {
  			$this->setRequestParameter('Token', $actionParameter['Token']);
  		}
  		if (isset($actionParameter['TopicArn'])) {
  			$this->setRequestParameter('TopicArn', $actionParameter['TopicArn']);
  		}
  	}
  	else {
  		$result = array();
  		if ($responseCode == '200') {
  			$result['error'] = FALSE;
  			$result['status'] = KABOOTR_AMAZON_REQUEST_SUCCESS;
  			$result['subscriptionArn'] = (string)$actionResponse->SubscriptionArn;
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
  
  
  
  private $problemMail = array();
  
  public function getProblemMail() {
  	return $this->problemMail;
  }
  
  private function setProblemMail($problem_emails) {
  	$this->problemMail = $problem_emails;
  }
  // Read Email statics, and give detail of bounces and complain mail
  public function readSendingStatics($jsonResponse) {
  	$obj = json_decode($jsonResponse->Message);
  	$notificationType = $obj->{'notificationType'};
  	$problem_emails = array();
  	$source_email = $obj->{'mail'}->{'source'};
  
  	// When notification type is Bounce
  	if ($notificationType == 'Bounce') {
  		$bounceType = $obj->{'bounce'}->{'bounceType'};
  		$bounceSubType = $obj->{'bounce'}->{'bounceType'};
  
  		// Amazon SES recommend that we have to remove the email addresses that
  		// have returned bounces marked Permanent from your mailing list
  
  		if ($bounceType = 'Permanent') {
  			$bounced_emails = $obj->{'bounce'}->{'bouncedRecipients'}; // May be have more then one email address
  		}
  	}
  
  	// When notification type is Complaint
  	elseif ($notificationType == 'Complaint') {
  		$complainedRecipients = $obj->{'complaint'}->{'complainedRecipients'};
  	}
  	$this->setProblemMail($problem_emails);
  }
}

/* {
  "notificationType":"Bounce",
"bounce":{
"bounceType":"Permanent",
"bounceSubType": "General",
"bouncedRecipients":[
    {
"emailAddress":"recipient1@example.com"
},
{
"emailAddress":"recipient2@example.com"
}
],
"timestamp":"2012-05-25T14:59:38.237-07:00",
"feedbackId":"00000137860315fd-869464a4-8680-4114-98d3-716fe35851f9-
000000"
},
  "mail":{
"timestamp":"2012-05-25T14:59:38.237-07:00",
"messageId":"00000137860315fd-34208509-5b74-41f3-95c5-22c1edc3c924-
000000",
"source":"email_1337983178237@amazon.com",
"destination":[
"recipient1@example.com",
"recipient2@example.com",
"recipient3@example.com",
"recipient4@example.com"
]
}
} */