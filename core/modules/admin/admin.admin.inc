<?php

/**
 * @file
 */

/**
 * Admin overview page.
 */
function admin_overview_page() {
  global $_menu;
  $links = array();
  foreach ($_menu as $path => $menu_item) {
    if (strpos($path, current_path()) !== FALSE && $path != current_path() && xcms_check_access($menu_item['access'])) {
      $links[] = array('title' => $menu_item['title'], 'href' => $path);
    }
  }
  return theme('links', array('links' => $links, 'attributes' => array('class' => array('admin-overview'))));
}

/**
 * Page callback for admin_settings_form().
 */
function admin_settings_page() {
  return xcms_get_form('admin_settings_form');
}

/**
 * Admin configuration settings form.
 */
function admin_settings_form($form, &$form_state) {
  $form['site_name'] = array(
    'type' => 'text',
    'title' => 'Site name',
    'default_value' => variable_get('site_name'),
    'required' => TRUE,
  );
  $form['site_mail'] = array(
    'type' => 'text',
    'title' => 'E-mail address',
    'default_value' => variable_get('site_mail'),
    'description' => 'The From address in automated e-mails sent from other notifications.',
    'required' => TRUE,
  );
  $form['site_front'] = array(
    'type' => 'text',
    'default_value' => variable_get('site_front'),
    'title' => 'Default front page',
    'description' => 'Optionally, specify a relative URL to display as the front page.',
  );
  // TODO scan theme directory for other themes
  // build options list themes_available()??
  $form['current_theme'] = array(
    'type' => 'select',
    'title' => 'Current theme',
    'default_value' => variable_get('current_theme'),
    'options' => xcms_themes_list(),
  );
  $form['date_format'] = array(
    'type' => 'text',
    'default_value' => variable_get('date_format'),
    'title' => 'Date format',
    'description' => 'Date format eg: "d/m/Y H:i". see ' . l('PHP date()', 'http://php.net/manual/en/function.date.php'),
  );
  $form['timezone'] = array(
    'type' => 'select',
    'title' => 'Default time zone',
    'description' => 'Select the default time zone. Dates and times throughout this site will be displayed using this time zone.',
    'options' => tz_options(),
    'default_value' => variable_get('timezone'),
  );
  return system_settings_form($form);
}

/**
 * Validation handler for admin_conf_form().
 */
function admin_conf_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['site_mail'])) {
    form_set_error('site_mail', 'The e-mail address <i>' . $form_state['values']['site_mail'] . '</i> is not valid.');
  }
}

/**
 * Reset cache link.
 */
function admin_cache_page() {
  variable_set('cache_timestamp', time());
  xcms_set_message('Flushed all caches.');
  xcms_goto('admin/config');
}

/**
 * Admin Status page.
 */
function admin_status_page() {
  $rows['xcms_version'] = array('XCMS', xcms_version());
  $rows['php_version'] = array('PHP', phpversion() . ' (' . l('phpinfo', 'admin/status/phpinfo') . ')');
  $rows['php_memory_limit'] = array('PHP memory limit', ini_get('memory_limit'));
  $rows['web_server'] = array('Web server', $_SERVER['SERVER_SOFTWARE']);
  return theme('table', array('rows' => $rows));
}

/**
 * Status page for phpinfo().
 */
function admin_phpinfo_page() {
  print phpinfo();
}

