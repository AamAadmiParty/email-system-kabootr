<?php
/**
 * @file
 * f_kabootr_reports.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function f_kabootr_reports_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'kabootr_reporting';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'simplenews_subscriber';
  $view->human_name = 'Kabootr Reporting';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Kabootr Reports';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'tid' => 'tid',
    'kabootr_bounce_count' => 'kabootr_bounce_count',
    'kabootr_spam_count' => 'kabootr_spam_count',
    'kabootr_unsubscribe_count' => 'kabootr_unsubscribe_count',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'tid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'kabootr_bounce_count' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'kabootr_spam_count' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'kabootr_unsubscribe_count' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Simplenews subscription: Term ID */
  $handler->display->display_options['relationships']['tid']['id'] = 'tid';
  $handler->display->display_options['relationships']['tid']['table'] = 'simplenews_subscription';
  $handler->display->display_options['relationships']['tid']['field'] = 'tid';
  $handler->display->display_options['relationships']['tid']['required'] = TRUE;
  /* Field: Taxonomy term: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'taxonomy_term_data';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'tid';
  /* Field: Simplenews issue: Kabootr Bounce Count */
  $handler->display->display_options['fields']['bounces_count']['id'] = 'bounces_count';
  $handler->display->display_options['fields']['bounces_count']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['fields']['bounces_count']['field'] = 'bounces_count';
  $handler->display->display_options['fields']['bounces_count']['relationship'] = 'tid';
  /* Field: Simplenews issue: Kabootr Spam Count */
  $handler->display->display_options['fields']['complain_count']['id'] = 'complain_count';
  $handler->display->display_options['fields']['complain_count']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['fields']['complain_count']['field'] = 'complain_count';
  $handler->display->display_options['fields']['complain_count']['relationship'] = 'tid';
  /* Field: Simplenews issue: Kabootr Unsubscribe Count */
  $handler->display->display_options['fields']['unsubscriber_count']['id'] = 'unsubscriber_count';
  $handler->display->display_options['fields']['unsubscriber_count']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['fields']['unsubscriber_count']['field'] = 'unsubscriber_count';
  $handler->display->display_options['fields']['unsubscriber_count']['relationship'] = 'tid';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'kabootr/reports';
  $export['kabootr_reporting'] = $view;

  return $export;
}
