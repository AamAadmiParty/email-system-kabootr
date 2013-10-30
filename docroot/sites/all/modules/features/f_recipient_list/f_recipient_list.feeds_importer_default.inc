<?php
/**
 * @file
 * f_recipient_list.feeds_importer_default.inc
 */

/**
 * Implements hook_feeds_importer_default().
 */
function f_recipient_list_feeds_importer_default() {
  $export = array();

  $feeds_importer = new stdClass();
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'recipient_list';
  $feeds_importer->config = array(
    'name' => 'Recipient List',
    'description' => 'Import users to site',
    'fetcher' => array(
      'plugin_key' => 'FeedsFileFetcher',
      'config' => array(
        'allowed_extensions' => 'txt csv tsv xml opml',
        'direct' => FALSE,
        'directory' => 'public://feeds',
        'allowed_schemes' => array(
          0 => 'public',
        ),
      ),
    ),
    'parser' => array(
      'plugin_key' => 'FeedsCSVParser',
      'config' => array(
        'delimiter' => ',',
        'no_headers' => 0,
      ),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsUserProcessor',
      'config' => array(
        'roles' => array(
          3 => 0,
        ),
        'status' => '1',
        'defuse_mail' => 0,
        'mappings' => array(
          0 => array(
            'source' => 'Name',
            'target' => 'field_user_name',
            'unique' => FALSE,
          ),
          1 => array(
            'source' => 'Email',
            'target' => 'mail',
            'unique' => 1,
          ),
          2 => array(
            'source' => 'Address',
            'target' => 'field_user_address',
            'unique' => FALSE,
          ),
          3 => array(
            'source' => 'City',
            'target' => 'field_user_city',
            'unique' => FALSE,
          ),
          4 => array(
            'source' => 'District',
            'target' => 'field_user_district',
            'unique' => FALSE,
          ),
          5 => array(
            'source' => 'State',
            'target' => 'field_user_state',
            'unique' => FALSE,
          ),
          6 => array(
            'source' => 'Country',
            'target' => 'field_country',
            'unique' => FALSE,
          ),
          7 => array(
            'source' => 'Gender',
            'target' => 'field_user_gender',
            'unique' => FALSE,
          ),
          8 => array(
            'source' => 'Age',
            'target' => 'field_user_age',
            'unique' => FALSE,
          ),
          9 => array(
            'source' => 'Mobile',
            'target' => 'field_user_mobile',
            'unique' => FALSE,
          ),
          10 => array(
            'source' => 'Vidhan Sabha',
            'target' => 'field_user_vidhansabha',
            'unique' => FALSE,
          ),
          11 => array(
            'source' => 'Lok Sabha',
            'target' => 'field_user_loksabha',
            'unique' => FALSE,
          ),
          12 => array(
            'source' => 'Source Of Data',
            'target' => 'field_user_source_of_data',
            'unique' => FALSE,
          ),
          13 => array(
            'source' => 'Date',
            'target' => 'field_user_date:start',
            'unique' => FALSE,
          ),
          14 => array(
            'source' => 'Contacts',
            'target' => 'field_user_contacts',
            'unique' => FALSE,
          ),
          15 => array(
            'source' => 'Email',
            'target' => 'name',
            'unique' => 1,
          ),
        ),
        'update_existing' => '0',
        'input_format' => 'plain_text',
        'skip_hash_check' => 0,
        'bundle' => 'user',
      ),
    ),
    'content_type' => '',
    'update' => 0,
    'import_period' => '-1',
    'expire_period' => 3600,
    'import_on_create' => 1,
    'process_in_background' => 0,
  );
  $export['recipient_list'] = $feeds_importer;

  return $export;
}
