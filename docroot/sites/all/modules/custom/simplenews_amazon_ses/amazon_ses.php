<?php 

/**
 * Send Request to amazon SES 
 */

//function send_request_amazon_ses($query_action = '', $email = '', $simpleEmailServiceMessage) {
  include 'class.amazonses.php';
  $simpleEmailService = new SimpleEmailService();
  $simpleEmailService->createQueryRequest('VerifyEmailIdentity', 'tkuldeep');
  print_r($simpleEmailService->getRequestParameter());
 // $queryEndPoint = SimpleEmailServiceRequest::queryEndPoint;
 // $http_req = new HttpRequest($queryEndPoint,HTTP_METH_POST);
  //$http_req->setHeaders($ses_request->requestHeaders);
  
 // $http_req->setPostFields($postData);
 // $http_req->send();
  //echo 'Raw Request Message:' . $http_req->getRawRequestMessage() . '\n';
  //echo 'Response Body' . $http_req->getResponseBody() . '\n';
  //echo 'Response Code: ' . $http_req->getResponseCode() . '\n';
  //echo 'Response Message: ' . $http_req->getResponseMessage() . '\n';
  //echo 'Response Header: ';
  //print_r($http_req->getResponseHeader()) . '\n';
  //echo 'Response Info: \n';
  //print_r($http_req->getResponseInfo()) . '\n';
 // echo 'Response Data:\n';
 // print_r($http_req->getResponseData());
  //echo gmdate('D, d M Y H:i:s e');
  
//}
