<?php

/**
 * @file
 */
function xcms_init() {
  global $conf, $_menu;
  $conf = xcms_conf();
  session_start();
  xcms_modules_load();
  xcms_get_menu();
  xcms_page_handler();
}

function xcms_conf() {
  $conf = NULL;

  // TODO Check for missing conf.ini, create sensible defaults.

  // TODO also check if these are set and exits (files)
  // base_path, site_name, current_theme

  // TODO merge ini file here (not blast it)
  $conf = parse_ini_file(ROOT_DIR . '/conf.ini');

  // TODO, this should go befor ini file, so it can be overridden in the ini
  $conf['base_path'] = rtrim(dirname($_SERVER['PHP_SELF']), '/') . '/';

  return $conf;
}

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

  if ($menu_item = xcms_get_item($path)) {

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

    // TODO args broken
    $menu_item['arguments'] = array();

    //
    $return = call_user_func_array($menu_item['callback'], $menu_item['arguments']);
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

  print theme_page($vars);
}

/**
 * xcms_get_item()
 */
function xcms_get_item($path) {
  global $_menu;

  // should never be empty in reality
  /*
  if (empty($_menu)) {
    $_menu = array();
  }
  */

  if (isset($_menu[$path])) {
    $_menu = xcms_translate_menu($_menu);
  }

  while ($path && !array_key_exists($path, $_menu)) {
    $path = substr($path, 0, strrpos($path, '/'));
  }

  if (!array_key_exists($path, $_menu)) {
    return;
  }

  return $_menu[$path];
}

/**
 * xcms_get_menu()
 */
function xcms_get_menu() {
  global $_menu;


  // TODO menu list cache - if necessary
  $menu_item_list = xcms_modules_invoke('menu');

  foreach ($menu_item_list as $path => $menu_item) {

    if (!isset($menu_item['weight'])) {
      $menu_item['weight'] = 0;
    }

    if (isset($menu_item['access'])) {
      // TODO, menu access should be either bool or allow args - rewrite
      if (function_exists($menu_item['access']) && $menu_item['access']()) {
        $_menu[$path] = $menu_item;
      }
    }
    else {
      $_menu[$path] = $menu_item;
    }
  }

  // $_menu = order($_menu, 'weight', 'title');

  // dsm($_menu);
  // return $_menu;
}

/**
 * xcms_translate_menu()
 * TODO can this go into xcms_get_item()?
 */
function xcms_translate_menu($menu) {
  foreach ($menu as $link => $value) {
    if (strpos($link, '%')) {
      $parts = explode('/', $link);
      for ($i = 0; $i < count($parts); $i++) {
        if ($parts[$i] == '%') {
          $parts[$i] = arg($i);
        }
      }
      $translated = implode('/', $parts);
      $menu[$translated] = $menu[$link];
      unset($menu[$link]);
    }
  }
  return $menu;
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

  // $primary_links = menu_primary_links();
  // $tabs = theme_links(menu_get_local_tasks());

  //if ( .. is front) body_classes [] = is_front
  // $body_classes = array();
  //if (is_admin()) {
  //  $body_classes[] = 'is_admin';
  //}
  //$body_classes = implode(' ', $body_classes);

  // TODO $tabs
  // TODO $body_classes

  // TODO hook_page for theme hooks.php
  // so variables can be modified from hook call

  extract($vars);
  require_once ROOT_DIR . '/themes/' . $conf['current_theme'] . '/page.php';
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

