<?php

/**
 * @file
 */
function user_login_form() {
  // Global $user;.
  $form['name'] = array(
    'type' => 'text',
    'title' => 'Username',
    'description' => 'Enter your ' . variable_get('site_name', 'XCMS') . ' username.',
    'required' => TRUE,
  );

  $form['pass'] = array(
    'type' => 'password',
    'title' => 'Password',
    'description' => 'Enter the password that accompanies your username.',
    'required' => TRUE,
  );

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Login',
  );

  return $form;
}

/**
 *
 */
function user_login_form_validate($form, &$form_state) {
  if ($form_state['values']['name'] == 'chris') {
    form_set_error('name', 'my error message', $form_state);
  }
}

/**
 *
 */
function user_login_form_submit($form, &$form_state) {
  dsm('form submit !!');
}

/**
 * Test_page_inc().
 */
function test_page_inc() {
  $output = '';
  $output .= xcms_get_form('user_login_form');

  /*
  $table_vars = array(
  'header' => array('this', 'that', 'the other'),
  'rows' => array(
  '0' => array('this', 'that', 'the other'),
  ),
  'attributes' => array('id' => 'funky-table'),
  );

  $output .= theme_table($table_vars);
   */

  return $output;
}

/**
 * Test_page_list().
 */
function test_page_list() {
  $list_items = array(
    '1' => 'Parent',
    'sub1' => array(
      '1' => 'child 1',
      '2' => 'child 2',
      'sub1' => array(
        '1' => lipsum(1),
        '2' => 'child 2',
      ),
    ),
  );

  return ('<ul>' . theme_list(array('items' => $list_items)) . '</ul>');
}

/**
 * @file
 */
function form_test_form() {
  // Global $user;.
  $form['name'] = array(
    'type' => 'text',
    'title' => 'Username',
    'value' => '',
    'description' => 'Enter your ' . variable_get('site_name', 'XCMS') . ' username.',
    'required' => TRUE,
  );

  $form['pass'] = array(
    'type' => 'password',
    'title' => 'Password',
    'description' => 'Enter the password that accompanies your username.',
    'required' => TRUE,
  );

  $form['textarea_of_doom'] = array(
    'type' => 'textarea',
    'title' => 'textarea',
    'attributes' => array(
      'rows' => 10,
      'class' => array('bazinga'),
    ),
  );

  $form['select'] = array(
    'type' => 'select',
    'title' => 'select',
    'options' => array(
      'test' => 'test',
      'test2' => 'test2',
    ),
    'attributes' => array(
      'class' => array('bazinga'),
    ),
  );

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Login',
  );

  return $form;
}
