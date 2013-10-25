<?php
/**
 * @file
 * f_kabootr_left_navigation.panels_default.inc
 */

/**
 * Implements hook_default_panels_mini().
 */
function f_kabootr_left_navigation_default_panels_mini() {
  $export = array();

  $mini = new stdClass();
  $mini->disabled = FALSE; /* Edit this to true to make a default mini disabled initially */
  $mini->api_version = 1;
  $mini->name = 'kabootr_left_navigation';
  $mini->category = '';
  $mini->admin_title = 'Kabootr Left Navigation';
  $mini->admin_description = '';
  $mini->requiredcontexts = array();
  $mini->contexts = array();
  $mini->relationships = array();
  $display = new panels_display();
  $display->layout = 'flexible';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'center' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = '';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-1';
    $pane->panel = 'center';
    $pane->type = 'custom';
    $pane->subtype = 'custom';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'admin_title' => '',
      'title' => '',
      'body' => '<?php
    print \'<div class = "kabootr-menu"><ul>\';
    print \'<li>\' . l(\'AWS Access Keys\', \'admin/kabootr-settings/aws-access-keys\') . \'</li>\';
    print \'<li>\' . l(\'Verify Sender ID\', \'admin/kabootr-settings/aws-verify-ses-sender-id\') . \'</li>\';
    print \'<li>\' . l(\'Sender ID List\', \'admin/kabootr-settings/aws-ses-sender-id-list\') . \'</li>\';
    print \'<li>\' . l(\' DKIM Setup\', \'admin/kabootr-settings/dkim-setup\') . \'</li>\';
    print \'</ul></div>\';
?>',
      'format' => 'php_code',
      'substitute' => TRUE,
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $display->content['new-1'] = $pane;
    $display->panels['center'][0] = 'new-1';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = 'new-1';
  $mini->display = $display;
  $export['kabootr_left_navigation'] = $mini;

  return $export;
}
