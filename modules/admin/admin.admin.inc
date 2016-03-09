<?php

/**
 * @file
 */

/**
 *
 */
function admin_conf_page() {
  return xcms_get_form('admin_conf_form');
}

/**
 *
 */
function admin_conf_form($form, &$form_state) {
dsm($form_state);

  $form['site_name'] = array(
    'type' => 'text',
    'title' => 'Site name',
    'required' => TRUE,
  );

  $form['site_mail'] = array(
    'type' => 'text',
    'title' => 'E-mail address',
    'description' => 'The From address in automated e-mails sent from other notifications.',
    'required' => TRUE,
  );

  $form['front_page'] = array(
    'type' => 'text',
    'title' => 'Default front page',
    'description' => 'Optionally, specify a relative URL to display as the front page.',
  );

  // TODO scan theme directory for other themes
  // build options list themes_available()??
  $form['current_theme'] = array(
    'type' => 'select',
    'title' => 'Current theme',
    'options' => array('default' => 'default'),
  );

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Save configuration',
  );

  return $form;
}

/**
 *
 */
function admin_status_page() {
  $output = ' ';

  $rows[] = array('XCMS', xcms_version());
  $rows[] = array('PHP', phpversion() . ' (' . l('phpinfo', 'admin/status/phpinfo') . ')');
  $rows[] = array('PHP memory limit', ini_get('memory_limit'));
  $rows[] = array('Web server', $_SERVER['SERVER_SOFTWARE']);

  $output .= theme('table', array('rows' => $rows));

  return $output;
}

/**
 *
 */
function admin_phpinfo_page() {
  print phpinfo();
}

