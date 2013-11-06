<?php
/**
 * @file
 * f_kabootr_left_navigation.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function f_kabootr_left_navigation_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "panels_mini" && $api == "panels_default") {
    return array("version" => "1");
  }
}
