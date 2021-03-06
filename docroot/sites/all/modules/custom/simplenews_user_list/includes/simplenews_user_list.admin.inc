<?php
/**
 * @file
 * Create newsletter category with subscriptions
 *
 * @ingroup simplenews userlist
 */

/**
 * Create user filters given in the account setting form
 *
 * @param newsletter_tid
 */
function simplenews_user_list_create_user_filters($newsletter_tid = NULL) {
  $widget_type = array();
  $widget_label = array();
  $widget_allowed_values = array();
  $widget_values = array();
  $user_field_ids = array();
  $widget_default = array();
  $widget_required = array();
  $widgets = array();
  
  // Provide user field label, type and it's ids
  // The id's of user field is important id, This id only identifies User filter
  // We have used as key in creating user filter form widget
  $user_field_array = field_info_instances('user');
  $user_field_instances = $user_field_array['user'];
  $user_field_instances_no = count($user_field_instances);
  
  if ($user_field_instances_no != 0) {
    foreach ($user_field_instances as $key => $user_field_instance) {
      $widget_type[$key] = $user_field_instance['widget']['type'];
      $widget_label[$key] = $user_field_instance['label'];
      $widget_required[$key] = $user_field_instance['required'];
      $user_field_info = field_info_field($key);
      if (isset($user_field_info['settings']['allowed_values'])) {
        $widget_allowed_values[$key] = $user_field_info['settings']['allowed_values'];
      }
    }
    
    // Set user fields allowed values to widget according to field ids
    foreach ($widget_allowed_values as $w_key => $w_value) {
      foreach ($w_value as $key => $value) {
        
        // If value source is taxonomy term
        if (is_array($value)) {
          /*
           * if (isset($value['vocabulary'])) { $vocab = taxonomy_vocabulary_machine_name_load($value['vocabulary']); $terms = taxonomy_get_tree($vocab->vid); foreach ($terms as $term) { $widget_values[$w_key]['tid_' . $term->tid] = $term->name; } }
           */
        }
        
        // If value source is default in List type widget like Select List and Checkboxes
        else {
          $widget_values[$w_key][$key] = $value;
        }
      }
    }
    
    // Retrieve already stored filters in simplenews_user_list
    // function will be invoked, during editing Newsletter category
    if ($newsletter_tid != NULL) {
      $users_filters_old = simplenews_user_list_get_user_list($newsletter_tid);
      $user_filters_old_no = count($users_filters_old);
      if ($user_filters_old_no != 0) {
        $users_filters = unserialize($users_filters_old[0]);
        foreach ($users_filters as $w_key => $w_value) {
          if (is_array($w_value)) {
            $widget_default[$w_key] = $w_value;
          }
          else {
            if (strpos($w_value, 'tid_') == FALSE) {
              $widget_default[$w_key] = $w_value;
            }
            /*
             * else { $widget_default[$w_key] = 'tid_' . explode('_', $w_value)[1]; }
             */
          }
        }
      }
    }
    // Create form elemtents according to user fields
    foreach ($widget_type as $key => $value) {
      switch($value) {
        
        // Create Drop down menu
        case 'options_select' :
          
          // According to me -1 key should not be entered by client, as if he do, element will always
          // our code will replace by value 'Any'.
          $widget_values[$key][-1] = 'Any'; 
          ksort($widget_values[$key]);
          
          if (! isset($widget_default[$key])) {
            $widget_default[$key] = array(-1);
          }
          
          $widgets[$key] = array(
              '#type' => 'select',
              '#title' => t($widget_label[$key]),
              '#options' => $widget_values[$key],
              '#default_value' => array(
                  $widget_default[$key]
              ),
          );
          break;
        
        // Create Checkboxes
        case 'options_buttons' :
          
          // In checkboxes, I am removing option of any, as if he want to select 'Any' option
          // He should check all checkboxes
          // $widget_values[$key][-1] = 'Any';
          //  ksort($widget_values[$key]);
          if (! isset($widget_default[$key])) {
            $widget_default[$key] = array(-1);
          }
          $widgets[$key] = array(
              '#type' => 'checkboxes',
              '#title' => t($widget_label[$key]),
              '#options' => $widget_values[$key],
          );
          
          if (isset($widget_default[$key])) {
            $widgets[$key]['#default_value'] = $widget_default[$key];
          }
          break;
        
        // Create textfield
        default:
          if (! isset($widget_default[$key])) {
            $widget_default[$key] = '';
          }
          $widgets[$key] = array(
              '#type' => 'textfield',
              '#title' => t($widget_label[$key]),
              '#size' => 30,
              '#default_value' => $widget_default[$key],
          );
      }
    }
  }
  else {
    $widgets = NULL;
  }
  return $widgets;
}

/**
 * Submit handler, called after creating newsletter category.
 * It creates the user subcription list for created newsletter category
 */
function simplenews_user_list_admin_category_form_submit($form, &$form_state) {
  $subcribe_uids = '';
  $user_filter_flag = 0; // Set flag to 0, User filters is not selected
  $newsletter_tid = $form_state['values']['tid']; // Always avaliable, this submit handler will be called after main submit handler
  $newsletter_name = taxonomy_term_load($newsletter_tid)->name;
  $number_of_subscription = 0;
  
  // Retreive user filter from form state array
  $users_filters = array();
  foreach ($form_state['values'] as $w_key => $w_value) {
    $pos = strpos($w_key, 'kabootr_user_filter_');
    
    // Find out key of users filters
    $field_pos = strpos($w_key, 'field');
    $user_field_key = substr($w_key, $field_pos);
    
    if ($pos !== FALSE) {
      if (is_array($w_value)) {
        $selected_options = array_filter($w_value);
        
        // In both cases, wheather we select checkbox or not, we have array, with one difference
        // In case of selected element, value of each element of array will be equal to it's key.
        // In case of not selected element, value will be empty string. 
        
        $selected_options_no = count($selected_options);
        if ($selected_options_no != 0) {
          $users_filters[$user_field_key] = $selected_options;
        }
      }
      elseif (! ($w_value == '')) {
        $users_filters[$user_field_key] = $w_value;
      }
    }
  }
  
  // Save or delete the status user filters and their value into simple_user_list table
  simplenews_user_list_create_list($newsletter_tid, $users_filters);
  // If user filter is selected then get the user details according to filters
  $users_filters_no = count($users_filters);
    if ($users_filters_no != 0) {
    $user_filter_flag = 1; // Set flag to 1, User filters is selected
    
    $user_field_ids = array_keys($users_filters);  // Get user field name and theid field ids
    
    // Prepare sql query from selected users filters
    $get_users_ids_query = new EntityFieldQuery();
    $get_users_ids_query->entityCondition('entity_type', 'user');
    foreach ($users_filters as $user_filter_name => $user_filter) {
      $value_filter = '';
      // Prepare entity field query according to applied user filters
      if (is_array($user_filter)) {
        foreach ($user_filter as $key => $val) {
          $operator = 'IN';
          if (strpos($key, 'tid_') == FALSE) {
            $value_type = 'value';
            $value_filter = $user_filter;
          }
          /* else {
            $value_type = 'tid';
            $value_filter[] = explode('_', $key)[1];
          } */
        }
      }
      else {
        if (strpos($user_filter, 'tid_') == FALSE) {
          $value_type = 'value';
          $operator = '=';
          $value_filter = $user_filter;
          // IF 'Any' option is selected
          if ($value_filter === '-1') {
            $operator = '!=';
          }
        }
        /* else {
          $value_type = 'tid';
          $value_filter = explode('_', $user_filter)[1];
          $operator = "=";
        } */
        
      }
      $get_users_ids_query->fieldCondition($user_filter_name, $value_type, $value_filter, $operator);
    }
    $result = $get_users_ids_query->execute();
    // if no user is found, then show the message and terminate the function, no change
    // in subscription list
    if (count($result) == 0) {
      $user_filter_flag = 1; // Set flag to 1, User filtere are selected, but no valid user is found
      $message = t("No users found for selected filters, There is no change in subscription list of @newsletter Category.", array(
          '@newsletter' => $newsletter_name,
      ));
      drupal_set_message($message, 'warning');
      return '';
    }
    else {
      $old_uids = array();
      $new_uids = array();
      
      // If users are found, then create the category of uid, which have to be
      // Subscribed and Unsubcribed
      if (isset($result['user'])) {
        $new_uids = array_keys($result['user']);
        $number_of_subscription = count($new_uids);
        
        // If already newsletter subscription persent for given newsletter category, retrieve them
        $subscriptions_list = simplenews_get_subscriptions_by_list($newsletter_tid);
        
        $subcribe_uids = $new_uids; // If no newsletter subscription for given newsletter category
        
        if (! count($subscriptions_list) == 0) {
          foreach (simplenews_get_subscriptions_by_list($newsletter_tid) as $value) {
            $old_uids[] = $value->uid;
            $old_mails[$value->uid] = $value->mail;
          }
          
          // Filter the subscribe and unsubscribe uids
          $subcribe_uids = array_diff($new_uids, $old_uids);
          $unsubscribe_uids = array_diff($old_uids, $new_uids);
          
          // Unsubsribe the users,
          if (count($unsubscribe_uids) != 0) {
            $number_of_unsubscription = 0;
            foreach ($unsubscribe_uids as $uid) {
              simplenews_unsubscribe_user($old_mails[$uid], $newsletter_tid, FALSE); // Email will not send, directoly user will be unsubscribe
              $number_of_unsubscription ++;
            }
            $message = t("!number_of_unsubscription users have unsubscribed to @newsletter Category.", array(
                '@newsletter' => $newsletter_name,
                '!number_of_unsubscription' => $number_of_unsubscription
            ));
            drupal_set_message($message, 'warning');
          }
        }
      }
    }
  }
  
  // Subsribe users to newsletter
  $number_of_new_subscription = count($subcribe_uids);
  if (($subcribe_uids !== '') && $number_of_new_subscription !== 0) {
    
    $subscriber = simplenews_user_list_get_subscriber($subcribe_uids);
    foreach ($subcribe_uids as $uid) {
      simplenews_subscribe_user($subscriber[$uid]->mail, $form_state['values']['tid'], FALSE, 'Action', '');
    }
    $message = t("!number_of_subscription users have subscribed to @newsletter Category.", array(
        '@newsletter' => $newsletter_name,
        '!number_of_subscription' => $number_of_subscription
    ));
    drupal_set_message($message, 'status');
  }
  else {
    $message = t("No users filter is selected , There is no change in subscription list of @newsletter Category.", array(
        '@newsletter' => $newsletter_name,
    ));
    drupal_set_message($message, 'warning');
  }
}

/**
 * Create or Update user list
 */
function simplenews_user_list_create_list($newsletter_tid, $users_filters) {
  if (count($users_filters) != 0) {
    
    // Check if user list is already exist to given newsletter category
    $sub_query = db_select('simplenews_user_list', 'sul')
    ->fields('sul', array('tid'))
    ->condition('tid', $newsletter_tid)
    ->execute()
    ->rowCount();
    
    // If there exist already user list, it update the filters
    
    if ($sub_query !== 0) {
    	 $query = db_update('simplenews_user_list')
      ->fields(array(
          'user_filters' => serialize($users_filters),
      ))
      ->condition('tid', $newsletter_tid)
      ->execute();
    }
     
    // If user list is not exist, it creates new one
    else {
      db_insert('simplenews_user_list')
      ->fields(array(
      'tid' => $newsletter_tid,
      'user_filters' => serialize($users_filters),
      ))
      ->execute();
    }
   }
  
  // If no filter is selected, then delete user list
  else {
    db_delete('simplenews_user_list')
    ->condition('tid', $newsletter_tid)
    ->execute();
  }
}

/**
 * Provide complete list of subscribers who are activated
 * 
 * @return Associative array of subscribers having uid and mail, uis is used as key of array.
 */
function simplenews_user_list_get_subscriber($subcribe_uids) {
  $query = db_select('simplenews_subscriber', 'sn');
  $query->fields('sn', array('uid','mail'))
  ->condition('sn.activated', 1)
  ->condition('uid', $subcribe_uids, 'IN');
  $query_result = $query
  ->execute()
  ->fetchAllAssoc('uid');
  
  return $query_result;
}

/**
 * Provide user fiter list of particular newsletter category
 * 
 * @param $newsletter_tid
 * @return Array of users filters in serilized object
 */
function simplenews_user_list_get_user_list($newsletter_tid) {
  $query = db_select('simplenews_user_list', 'sul')
  ->fields('sul', array('user_filters'))
  ->condition('tid', $newsletter_tid);
  $query_result = $query
  ->execute()
  ->fetchCol(0);
  
  return $query_result;
}