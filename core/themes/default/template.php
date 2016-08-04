<?php

/**
 * @file
 */

/**
 *
 */
function default_preprocess_page(&$vars) {
  $vars['nav'] = _default_nav();
}

/**
 *
 */
function _default_nav() {
  $nav_items = array(
    'home' => l('Home', '<front>'),
    'test' => l('Test', 'test'),
    'user' => l('User', 'user'),
  );

  if (user_is_logged_in()) {
    $nav_items['logout'] = l('Log out', 'user/logout');
  }
  else {
    $nav_items['login'] = l('Login', 'user/login');
  }

  return theme_list(array('items' => $nav_items, 'attributes' => array('class' => 'nav-menu')));
}

