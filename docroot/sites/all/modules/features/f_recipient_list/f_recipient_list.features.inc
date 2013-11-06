<?php
/**
 * @file
 * f_recipient_list.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function f_recipient_list_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "feeds" && $api == "feeds_importer_default") {
    return array("version" => "1");
  }
}
