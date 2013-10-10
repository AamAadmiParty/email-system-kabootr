<?php

$services = array(
    'includes' => array(
        'SesClient.php',
    ),
    'services' => array(
          'amazon_ses' => array(
              'class' => 'serviceClient\SesClient',
              'params' => array(
                  'consumer_key' => 'foo_key',
                  'consumer_secret' => 'foo_secret',
              )
          ),
      ),
);

return $services;