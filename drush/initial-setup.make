; ----------------
; Generated makefile from http://drushmake.me
; Permanent URL: http://drushmake.me/file.php?token=05c14f5d5e14
; ----------------
;
; This is a working makefile - try it! Any line starting with a `;` is a comment.
  
; Core version
; ------------
; Each makefile should begin by declaring the core version of Drupal that all
; projects should be compatible with.
  
core = 7.x
  
; API version
; ------------
; Every makefile needs to declare its Drush Make API version. This version of
; drush make uses API version `2`.
  
api = 2
  
; Core project
; ------------
; In order for your makefile to generate a full Drupal site, you must include
; a core project. This is usually Drupal core, but you can also specify
; alternative core projects like Pressflow. Note that makefiles included with
; install profiles *should not* include a core project.
  
; Drupal 7.x. Requires the `core` property to be set to 7.x.
projects[drupal][version] = 7

  
  
; Modules
; --------
projects[admin][version] = 2.0-beta3
projects[admin][type] = "module"
projects[admin][subdir] = "contrib"
projects[admin_menu][version] = 3.0-rc4
projects[admin_menu][type] = "module"
projects[admin_menu][subdir] = "contrib"
projects[module_filter][version] = 1.8
projects[module_filter][type] = "module"
projects[module_filter][subdir] = "contrib"
projects[ctools][version] = 1.3
projects[ctools][type] = "module"
projects[ctools][subdir] = "contrib"
projects[date][version] = 2.6
projects[date][type] = "module"
projects[date][subdir] = "contrib"
projects[devel][version] = 1.3
projects[devel][type] = "module"
projects[devel][subdir] = "contrib"
projects[features][version] = 2.0-rc3
projects[features][type] = "module"
projects[features][subdir] = "contrib"
projects[content_taxonomy][version] = 1.0-beta2
projects[content_taxonomy][type] = "module"
projects[content_taxonomy][subdir] = "contrib"
projects[email][version] = 1.2
projects[email][type] = "module"
projects[email][subdir] = "contrib"
projects[flag][version] = 3.2
projects[flag][type] = "module"
projects[flag][subdir] = "contrib"
projects[mimemail][version] = 1.0-alpha2
projects[mimemail][type] = "module"
projects[mimemail][subdir] = "contrib"
projects[simplenews][version] = 1.0
projects[simplenews][type] = "module"
projects[simplenews][subdir] = "contrib"
projects[advanced_help][version] = 1.0
projects[advanced_help][type] = "module"
projects[advanced_help][subdir] = "contrib"
projects[diff][version] = 3.2
projects[diff][type] = "module"
projects[diff][subdir] = "contrib"
projects[entity][version] = 1.2
projects[entity][type] = "module"
projects[entity][subdir] = "contrib"
projects[entityreference][version] = 1.0
projects[entityreference][type] = "module"
projects[entityreference][subdir] = "contrib"
projects[feeds][version] = 2.0-alpha8
projects[feeds][type] = "module"
projects[feeds][subdir] = "contrib"
projects[field_group][version] = 1.2
projects[field_group][type] = "module"
projects[field_group][subdir] = "contrib"
projects[libraries][version] = 2.1
projects[libraries][type] = "module"
projects[libraries][subdir] = "contrib"
projects[menu_block][version] = 2.3
projects[menu_block][type] = "module"
projects[menu_block][subdir] = "contrib"
projects[pathauto][version] = 1.2
projects[pathauto][type] = "module"
projects[pathauto][subdir] = "contrib"
projects[search404][version] = 1.3
projects[search404][type] = "module"
projects[search404][subdir] = "contrib"
projects[smtp][version] = 1.0
projects[smtp][type] = "module"
projects[smtp][subdir] = "contrib"
projects[strongarm][version] = 2.0
projects[strongarm][type] = "module"
projects[strongarm][subdir] = "contrib"
projects[token][version] = 1.5
projects[token][type] = "module"
projects[token][subdir] = "contrib"
projects[rules][version] = 2.3
projects[rules][type] = "module"
projects[rules][subdir] = "contrib"
projects[taxonomy_manager][version] = 1.0
projects[taxonomy_manager][type] = "module"
projects[taxonomy_manager][subdir] = "contrib"
projects[taxonomy_menu][version] = 1.4
projects[taxonomy_menu][type] = "module"
projects[taxonomy_menu][subdir] = "contrib"
projects[views][version] = 3.7
projects[views][type] = "module"
projects[views][subdir] = "contrib"
projects[views_bulk_operations][version] = 3.1
projects[views_bulk_operations][type] = "module"
projects[views_bulk_operations][subdir] = "contrib"

  

; Themes
; --------

  
  
; Libraries
; ---------
; No libraries were included

