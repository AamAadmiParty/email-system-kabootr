<?php 

class SimpleNotificationService {
  
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