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
  $view->name = 'kabootr_reports';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Kabootr Reports';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Kabootr Reports';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Simplenews issue: Term ID */
  $handler->display->display_options['relationships']['tid']['id'] = 'tid';
  $handler->display->display_options['relationships']['tid']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['relationships']['tid']['field'] = 'tid';
  $handler->display->display_options['relationships']['tid']['required'] = TRUE;
  /* Relationship: Simplenews category: Term ID */
  $handler->display->display_options['relationships']['tid_1']['id'] = 'tid_1';
  $handler->display->display_options['relationships']['tid_1']['table'] = 'simplenews_category';
  $handler->display->display_options['relationships']['tid_1']['field'] = 'tid';
  $handler->display->display_options['relationships']['tid_1']['relationship'] = 'tid';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Newsletter Title';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: All taxonomy terms */
  $handler->display->display_options['fields']['term_node_tid']['id'] = 'term_node_tid';
  $handler->display->display_options['fields']['term_node_tid']['table'] = 'node';
  $handler->display->display_options['fields']['term_node_tid']['field'] = 'term_node_tid';
  $handler->display->display_options['fields']['term_node_tid']['label'] = 'Newsletter Category';
  $handler->display->display_options['fields']['term_node_tid']['limit'] = TRUE;
  $handler->display->display_options['fields']['term_node_tid']['vocabularies'] = array(
    'newsletter' => 'newsletter',
    'tags' => 0,
  );
  /* Field: Simplenews issue: Subscriber count */
  $handler->display->display_options['fields']['sent_subscriber_count']['id'] = 'sent_subscriber_count';
  $handler->display->display_options['fields']['sent_subscriber_count']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['fields']['sent_subscriber_count']['field'] = 'sent_subscriber_count';
  $handler->display->display_options['fields']['sent_subscriber_count']['label'] = 'Kabootr Sent Count';
  /* Field: Simplenews issue: Kabootr Bounce Count */
  $handler->display->display_options['fields']['bounces_count']['id'] = 'bounces_count';
  $handler->display->display_options['fields']['bounces_count']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['fields']['bounces_count']['field'] = 'bounces_count';
  /* Field: Simplenews issue: Kabootr Spam Count */
  $handler->display->display_options['fields']['complain_count']['id'] = 'complain_count';
  $handler->display->display_options['fields']['complain_count']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['fields']['complain_count']['field'] = 'complain_count';
  /* Field: Simplenews issue: Kabootr Unsubscribe Count */
  $handler->display->display_options['fields']['unsubscriber_count']['id'] = 'unsubscriber_count';
  $handler->display->display_options['fields']['unsubscriber_count']['table'] = 'simplenews_newsletter';
  $handler->display->display_options['fields']['unsubscriber_count']['field'] = 'unsubscriber_count';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'simplenews' => 'simplenews',
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'kabootr/reports';
  $export['kabootr_reports'] = $view;

  return $export;
}
