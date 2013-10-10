<?php

namespace serviceClient;

use Guzzle\Common\Collection;
use Guzzle\Plugin\Oauth\OauthPlugin;
use Guzzle\Service\Client;
use Guzzle\Service\Description\ServiceDescription;


/**
 * A simple Amazon SES Client
 */
class SesClient extends Client
{
    public static function factory($config = array())
    {
        // Provide a hash of default client configuration options
        $default = array('base_url' => 'https://email.us-east-1.amazonaws.com');

        // The following values are required when creating the client
        $required = array(
            'base_url',
            'consumer_key',
            'consumer_secret',
        );

        // Merge in default settings and validate the config
        $config = Collection::fromConfig($config, $default, $required);

        // Create a new Twitter client
        $client = new self($config->get('base_url'), $config);

        print_r($client);
        return $client;
    }
}

?>