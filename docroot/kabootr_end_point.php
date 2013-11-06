<?php
define('DRUPAL_ROOT', getcwd());
require_once
DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$json = json_decode(file_get_contents("php://input"));

simplenews_amazon_ses_receive_sns($json);
