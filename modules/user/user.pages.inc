<?php

/**
 * @file
 */
function user_page() {
  global $user;

  if (user_is_logged_in()) {
    $output = '';
    $output .= l('View', 'user') . ' | ' . l('Edit', 'user/' . $user->uid . '/edit');
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
 * User_login_page().
 */
function user_login_page() {
  return xcms_get_form('user_login_form');
}

/**
 * User_logout_page().
 */
function user_logout_page() {
  user_logout();
  xcms_goto();
}

/**
 *
 */
function user_login_form($form, &$form_state) {
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
    form_set_error('name', 'Sorry, invalid username or password. Have you forgotten your password?');
  }
}

/**
 *
 */
function user_login_form_submit($form, &$form_state) {
  user_session($form_state['values']['name']);
  // Todo use redirect callback for goto. - set GET-destination=variable from form?
  xcms_goto();
}

/**
 * User_account_page().
 */
function user_account_page() {
  global $user;
  $output = '';
  $output .= l('View', 'user') . ' | ' . l('Edit', 'user/' . $user->uid . '/edit');
  $output .= xcms_get_form('user_account_form');
  return $output;
}

/**
 *
 */
function user_account_form($form, &$form_state) {
  global $user;

  $form['name'] = array(
    'type' => 'text',
    'title' => 'Username',
    'default_value' => $user->name,
    'description' => 'Spaces are allowed; punctuation is not allowed except for periods, hyphens, apostrophes, and underscores.',
    'required' => TRUE,
  );

  // TODO password match pure javascript html client side
  // http://codepen.io/diegoleme/pen/surIK

  $form['pass'] = array(
    'type' => 'password',
    'title' => 'Password',
    'description' => 'Enter your current password to change the E-mail address or Password.',
    'required' => TRUE,
  );

  $form['pass_confirm'] = array(
    'type' => 'password',
    'title' => 'Confirm Password',
    'description' => 'To change the current user password, enter the new password in both fields.',
    'required' => TRUE,
  );

  $form['mail'] = array(
    'type' => 'text',
    'title' => 'E-mail address',
    'default_value' => $user->mail,
    'description' => 'A valid e-mail address. All e-mails from the system will be sent to this address. The e-mail address is not made public and will only be used if you wish to receive a new password or wish to receive certain news or notifications by e-mail.',
    'required' => TRUE,
  );

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Save',
  );
  return $form;
}

