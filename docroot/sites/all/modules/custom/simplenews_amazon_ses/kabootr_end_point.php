<?php

require_once 'includes/classes/amazonses.class.php';

$json = json_decode(file_get_contents("php://input"));

print $json;
print 'tk';
//Handle A Subscription Request Programmatically
if($json->Type = "SubscriptionConfirmation") {
	$actionParameters['Token'] = $json->Token;
	$actionParameters['TopicArn'] = $json->TopicArn;
	$result = simplenews_amazon_ses_send_request('ConfirmSubscription', $actionParameters);
}