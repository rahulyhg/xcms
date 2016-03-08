<?php

/**
 * @file
 */
function user_page() {
  global $user;

  if (user_is_logged_in()) {
    $output = '';
    $output .= l('View', 'user') . ' | ' . l('Edit' , 'user/' . $user->uid . '/edit');
    $output .= '<h3>History</h3>';
    $output .= '<p><strong>Member since</strong></p>';
    $output .= '<p>' . date('d/m/Y', $user->created) . '</p>';
    $output .= '<pre>' . print_r($user, TRUE) . '</pre>';
    return $output;
  }
  else {
    xcms_goto('user/login');
  }
}

/**
 * user_login_page().
 */
function user_login_page() {
  return xcms_get_form('user_login_form');
}

/**
 * user_logout_page().
 */
function user_logout_page() {
  user_logout();
  xcms_goto();
}

/**
 *
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
  if (!user_authenticate($form_state['values']['name'], $form_state['values']['pass'])) {
    form_set_error('name', 'Sorry, invalid username or password. Have you forgotten your password?', $form_state);
  }
}

/**
 *
 */
function user_login_form_submit($form, &$form_state) {
  user_session($form_state['values']['name']);
  // todo use redirect callback for goto
  xcms_goto('user');
}

