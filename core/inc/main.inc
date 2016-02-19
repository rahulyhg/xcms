<?php

/**
 * @file
 */

/**
 * xcms_init()
 */
function xcms_init() {
  global $conf, $_menu;
  $conf = xcms_conf_read();
  session_start();
  xcms_modules_load();
  xcms_menu_get();
  xcms_page_handler();
}

/**
 * xcms_conf_get()
 */
function xcms_conf_read() {
  $conf = NULL;

  // get site conf file and parse to array
  $conf = json_decode(file_get_contents(ROOT_DIR . '/conf.json'), TRUE);

  // TODO also check if these are set and exits (files)
  // base_path, site_name, current_theme

  // TODO merge ini file here (not blast it)
  // $conf = parse_ini_file(ROOT_DIR . '/conf.ini');
  // TODO, this should go befor ini file, so it can be overridden in the ini
  $conf['base_path'] = rtrim(dirname($_SERVER['PHP_SELF']), '/') . '/';

  return $conf;
}

/**
 * xcms_conf_write()
 */
function xcms_conf_write() {
  // TODO array merge conf and array as parameter - eg for adding variables, variable_set() and syste_settings_form
  global $conf;
  file_put_contents(ROOT_DIR . '/conf.json', json_encode($conf, JSON_PRETTY_PRINT));
}

/**
 * xcms_page_handler()
 */
function xcms_page_handler() {
  $return = NULL;
  $vars = array();

  // front page
  if (empty($_GET['q'])) {
    $path = variable_get('site_front', 'welcome');
  }
  else {
    $path = $_GET['q'];
  }

  if ($menu_item = xcms_path_get($path)) {

    /* TODO access broken
      if (function_exists($menu_item['access']) && !$menu_item['access']()) {
        $return = FALSE;
      }
      */

    // load file inc
    if (!empty($menu_item['file'])) {
      require_once $menu_item['file'];
    }
    xcms_set_title($menu_item['title']);

    // if ($menu_item['access'])

    // TODO args broken
    $menu_item['args'] = array();

    // TODO page callback args
    $return = call_user_func_array($menu_item['callback'], $menu_item['args']);
  }
  else {
    // $return = FALSE;
    $return = 'hi there';
  }

  /// good to go
  if (!empty($return)) {
    $vars['content'] = $return;
  }
  elseif ($return === FALSE) {
    header('HTTP/1.1 403 Forbidden');
    $vars = array('title' => 'Access denied', 'content' => 'You are not allowed to access this page.');
  }
  else {
    header('HTTP/1.1 404 Not Found');
    $vars = array('title' => 'Page not found', 'content' => 'The requested page could not be found.');
  }

  // TODO dont theme page always - eg. for errors, jason, or other where no page rendering is needed.
  print theme_page($vars);
}

/**
 * xcms_path_get()
 */
function xcms_path_get($path) {
  global $_menu;
  print ($path);

  /*
  if (isset($_menu[$path])) {

    $defaults = array(
      'title' => NULL,
      'weight' => 0,
      'access' => TRUE,
      // 'args' => FALSE,
      // 'type' => MENU_ITEM,
    );

    // $_menu = xcms_translate_menu($_menu);

    // TODO do we have to set this?
    $menu = $_menu;

    foreach ($menu as $link => $value) {

      // path arg translation routine
      if (strpos($link, '%')) {
        $parts = explode('/', $link);
        for ($i = 0; $i < count($parts); $i++) {
          if ($parts[$i] == '%') {

            $parts[$i] = arg($i);
          }
        }
        $translated = implode('/', $parts);
        $menu[$translated] = $menu[$link];

        // dsm($menu[$translated]);
        unset($menu[$link]);
      }
    }
    // TODO do we have to set this?
    $_menu = $menu;
  }
  */

  // work backwards to search for a valid menu path
  while ($path && !array_key_exists($path, $_menu)) {
    $path = substr($path, 0, strrpos($path, '/'));
  }

  if (!array_key_exists($path, $_menu)) {
    return;
  }

  return $_menu[$path];
}

/**
 * xcms_menu_get()
 */
function xcms_menu_get() {
  global $_menu;

  // TODO menu list cache to json - requires admin flush
  $_menu = xcms_modules_invoke('menu');
}

/**
 * xcms_modules_load()
 */
function xcms_modules_load() {
  // global $conf;
  // $modules = db_query('select', 'module', array('field' => 'status', 'value' => 1));
  $modules = array(
    'system',
  );

  foreach ($modules as $module) {
    include_once (ROOT_DIR . '/core/modules/' . $module . '/' . $module . '.module');
  }
}

/**
 * xcms_modules_invoke()
 */
function xcms_modules_invoke($hook) {
  // $modules = db_query('select', 'module', array('field' => 'status', 'value' => 1));
  $return = array();

  // TODO, load modules from ini
  $modules = array(
    'system',
  );

  foreach ($modules as $module) {
    $function = $module . '_' . $hook;
    if (function_exists($function)) {
      $return = array_merge($return, $function());
    }
  }
  return $return;
}

/**
 * xcms_set_title()
 */
function xcms_set_title($title = NULL) {
  static $stored_title;
  if (isset($title)) {
    $stored_title = $title;
  }
  return $stored_title;
}

/**
 * variable_get()
 */
function variable_get($name, $default) {
  global $conf;
  return isset($conf[$name]) ? $conf[$name] : $default;
}

/**
 * xcms_set_message()
 */
function xcms_set_message($message = NULL, $type = 'status') {
  if ($message) {
    $_SESSION['messages'][$type][] = $message;
  }
  else {
    // TODO rewrite / tidy this code if it can be done - see aspirations code?
    $output = '';
    if (!empty($_SESSION['messages'])) {
      foreach ($_SESSION['messages'] as $class => $messages) {
        $output .= '<div class="messages ' . $class . '">';
        foreach ($messages as $text) {
          $output .= $text . '<br />';
        }
        $output .= '</div>';
      }
      $_SESSION['messages'] = array();
    }
    return $output;
  }
}

/**
 * theme_page()
 */
function theme_page(&$vars) {
  global $conf;
  if (empty($vars['title'])) {
    $vars['title'] = strip_tags(xcms_set_title());
  }
  $vars['directory'] = $conf['base_path'] . 'themes/' . $conf['current_theme'];
  $vars['head_title'] = $conf['site_name'] . ' | ' . $vars['title'];

  // TODO, fix this front_page needs to be variable
  $vars['front_page'] = $conf['base_path'];
  $vars['messages'] = xcms_set_message();

  // TODO $tabs
  // $tabs = theme_links(menu_get_local_tasks());

  // TODO $body_classes
  //if ( .. is front) body_classes [] = is_front
  // $body_classes = array();
  //if (is_admin()) {
  //  $body_classes[] = 'is_admin';
  //}
  //$body_classes = implode(' ', $body_classes);

  // TODO hook_page for theme hooks.php
  // so variables can be modified from hook call

  extract($vars);
  require_once ROOT_DIR . '/themes/' . $conf['current_theme'] . '/page.php';
}

/**
 * issetor()
 */
function issetor(&$var, $default = NULL) {
  return isset($var) ? $var : $default;
}

/**
 * arg()
 */
function arg($index = 0) {
  $args = explode('/', $_GET['q']);
  if (array_key_exists($index, $args)) {
    return $args[$index];
  }
}

// TODO rewrite this as a module later
function dsm($input) {
  xcms_set_message('<pre>' . print_r($input, TRUE) . '</pre>');
}

/**
 * order_cmp()
 * TODO this was added to satisfy xcms_get_menu() do we need it - different way?
 */

/*
function order_cmp($a, $b = NULL) {
  static $keys;
  if ($b === NULL) {
    return $keys = $a;
  }
  foreach ($keys as $k) {
    if (@$k[0] == '!') {
      $k = substr($k, 1);
      if (@$a[$k] !== @$b[$k]) {
        return strcmp(@$b[$k], @$a[$k]);
      }
    }
    elseif (@$a[$k] !== @$b[$k]) {
      return strcmp(@$a[$k], @$b[$k]);
    }
  }
  return 0;
}
*/

/**
 * order()
 * TODO this was added to satisfy xcms_get_menu() do we need it - different way?
 */
/*
function order($array) {
  $keys = func_get_args();
  array_shift($keys);
  order_cmp($keys);
  uasort($array, 'order_cmp');
  return $array;
}
*/

