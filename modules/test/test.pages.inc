<?php

/**
 * @file
 */
function user_login_form() {
  // global $user;

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

/**
 * test_page_inc()
 */
function test_page_inc() {
  // return lipsum(10);
  $output = '';
  /*
  $element = array(
    'type' => 'text',
    'title' => 'Test Input',
    'markup' => 'asda',
  );

  $output = theme_form_element($element);
  */

  $output .= xcms_get_form('user_login_form');

  $table_vars = array(
    'header' => array('this', 'that', 'the other'),
    'rows' => array(
      '0' => array('this', 'that', 'the other'),
    ),
  );

  $output .= theme_table($table_vars);

  return $output;
}

/**
 * test_page_list()
 */
function test_page_list() {

  /*
  $list_items = array(
    '1' => lipsum(1),
    '2' => lipsum(1),
    '3' => 'parent',
    '4' => array(
      '41' => lipsum(1),
      '42' => lipsum(1),
      '3' => 'parent',
      '4' => array(
        '41' => lipsum(1),
        '42' => lipsum(1),
        '43' => lipsum(1),
      ),
    ),
  );
  */

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

  /*
  $list_items = array();
  $list_items['test'] = lipsum(1);
  $list_items['test']['second'] = lipsum(1);
  */

  // return theme_list_test($list_items);
}

