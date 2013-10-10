<?php

$path = drupal_get_path('module', 'simplenews_amazon_ses');
require  $path . '/vendor/autoload.php';
//use \Guzzle\Http\Client;
use Guzzle\Service\Builder\ServiceBuilder;

function send_request_guzzle() {
  $path = drupal_get_path('module', 'simplenews_amazon_ses');
  // Create a service builder and provide client configuration data
  $builder = ServiceBuilder::factory($path . '/includes/classes/testClient.json');
  
  // Get the client from the service builder by name
  $amazon_ses = $builder->get('amazon_ses');
  print_r($amazon_ses);
  // Create a client and provide a base URL
  //$client = new Client('https://google.com');

  //$request = $client->get();
  //echo $request->getUrl();

  // You must send a request in order for the transfer to occur
 // $response = $request->send();

 // echo $response->getBody();

}
