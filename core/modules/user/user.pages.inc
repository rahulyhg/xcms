<?php

/**
 * @file
 */
function user_page() {
  global $user;

  if (user_is_logged_in()) {
    $output = '';
    $output .= '<h3>History</h3>';
    $output .= '<p><strong>Member since</strong></p>';
    $output .= '<p>' . date('d/m/Y', $user->created) . '</p>';
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
  $output .= xcms_get_form('user_account_form');
  return $output;
}

/**
 *
 */
function user_account_form($form, &$form_state) {
  $account = user_load(arg(1));

  $form['name'] = array(
    'type' => 'text',
    'title' => 'Username',
    'default_value' => $account->name,
    'description' => 'Spaces are allowed; punctuation is not allowed except for periods, hyphens, apostrophes, and underscores.',
    'required' => TRUE,
  );

  // TODO password match pure javascript html client side
  // http://codepen.io/diegoleme/pen/surIK
  $form['pass'] = array(
    'type' => 'password',
    'title' => 'Password',
    'description' => 'Enter your current password to change the E-mail address or Password.',
  );

  $form['pass_confirm'] = array(
    'type' => 'password',
    'title' => 'Confirm Password',
    'description' => 'To change the current user password, enter the new password in both fields.',
  );

  $form['mail'] = array(
    'type' => 'text',
    'title' => 'E-mail address',
    'default_value' => $account->mail,
    'description' => 'A valid e-mail address. All e-mails from the system will be sent to this address. The e-mail address is not made public and will only be used if you wish to receive a new password or wish to receive certain news or notifications by e-mail.',
    'required' => TRUE,
  );

  $form['timezone'] = array(
    'type' => 'select',
    'title' => 'Time zone',
    'description' => 'Select the desired local time and time zone. Dates and times throughout this site will be displayed using this time zone.',
    'options' => tz_options(),
    'default_value' => $account->timezone,
  );

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Save',
  );
  return $form;
}

/**
 *
 */
function user_account_form_submit($form, &$form_state) {
  $account = user_load(arg(1));
  $edit = array(
    'name' => $form_state['values']['name'],
    'mail' => $form_state['values']['mail'],
    'timezone' => $form_state['values']['timezone'],
  );
  user_save($account, $edit);
  xcms_set_message('The changes have been saved.');
}

