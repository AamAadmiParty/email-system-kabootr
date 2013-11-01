<?php

/**
 * @file
 * kabootr_notification_reports.views.inc().
 *
 * @author
 * Gaurav Goyal
 *
 * @desc
 * Alters the views created by simplenews module, so that the newly created
 * fields for kabootr notification can be integrated with views too.
 */

/**
 * Implements hook_views_data_alter().
 */
function kabootr_notification_reports_views_data_alter(&$data) {
  // if simplenews_newsletter table is integrated with views
  if (isset($data['simplenews_newsletter'])) {
    // add kabootr bounce count column
    $data['simplenews_newsletter']['bounces_count'] = array(
      'title' => t('Kabootr Bounce Count'),
      'help' => t('Shows the Kabootr Email Bounce Count'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
    );
    // add kabootr spam count column
    $data['simplenews_newsletter']['complain_count'] = array(
      'title' => t('Kabootr Spam Count'),
      'help' => t('Shows the Kabootr Email Spam Count'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
    );
    // add kabootr unsubscribe count
    $data['simplenews_newsletter']['unsubscriber_count'] = array(
      'title' => t('Kabootr Unsubscribe Count'),
      'help' => t('Shows the Kabootr Email Unsubscribe Count'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
    );
  }
}