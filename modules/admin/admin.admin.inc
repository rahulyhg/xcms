<?php

/**
 * @file
 */

/**
 * Page callback for admin_conf_form().
 */
function admin_conf_page() {
  return xcms_get_form('admin_conf_form');
}

/**
 * Admin configuration settings form.
 */
function admin_conf_form($form, &$form_state) {
  global $conf;

  $form['site_name'] = array(
    'type' => 'text',
    'title' => 'Site name',
    'default_value' => issetor($conf['site_name']),
    'required' => TRUE,
  );

  $form['site_mail'] = array(
    'type' => 'text',
    'title' => 'E-mail address',
    'default_value' => issetor($conf['site_mail']),
    'description' => 'The From address in automated e-mails sent from other notifications.',
    'required' => TRUE,
  );

  $form['site_front'] = array(
    'type' => 'text',
    'default_value' => issetor($conf['site_front']),
    'title' => 'Default front page',
    'description' => 'Optionally, specify a relative URL to display as the front page.',
  );

  // TODO scan theme directory for other themes
  // build options list themes_available()??
  $form['current_theme'] = array(
    'type' => 'select',
    'title' => 'Current theme',
    'default_value' => issetor($conf['current_theme']),
    'options' => array('default' => 'default', 'nothing' => 'nothing'),
  );

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Save configuration',
  );

  return $form;
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
 * Submit handler for admin_conf_form().
 */
function admin_conf_form_submit($form, &$form_state) {
  global $conf;
  foreach ($form_state['values'] as $key => $value) {
    if (array_key_exists($key, $conf)) {
      $conf[$key] = $value;
    }
  }
  xcms_conf_write($conf);
  xcms_set_message('The configuration options have been saved.');
}

/**
 * Admin Status page.
 */
function admin_status_page() {
  $rows[] = array('XCMS', xcms_version());
  $rows[] = array('PHP', phpversion() . ' (' . l('phpinfo', 'admin/status/phpinfo') . ')');
  $rows[] = array('PHP memory limit', ini_get('memory_limit'));
  $rows[] = array('Web server', $_SERVER['SERVER_SOFTWARE']);

  return theme('table', array('rows' => $rows));
}

/**
 * Status page for phpinfo().
 */
function admin_phpinfo_page() {
  print phpinfo();
}

