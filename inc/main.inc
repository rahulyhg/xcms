<?php

/**
 * @file
 */

/**
 * Contants
 * MENU_*
 *  MENU_CALLBACK: hidden
 */
//define('MENU_CALLBACK', 0);
//define('MENU_NORMAL', 1);

/**
 * xcms_init()
 */
function xcms_init() {
  global $conf, $_menu;
  session_start();
  xcms_conf_read();
  xcms_modules_load();
  xcms_menu_get();
  if ($return = xcms_page_handler()) {
    // TODO: theme page should be a template function - see d7
    print theme_page($return);
  }
}

/**
 * xcms_conf_get()
 */
function xcms_conf_read() {
  global $conf;
  $conf_file = ROOT_DIR . '/conf.json';
  if (file_exists($conf_file)) {
    $conf = json_decode(file_get_contents($conf_file), TRUE);
  }
  else {
    require_once ROOT_DIR . '/inc/install.inc';
    $conf = xcms_install($conf_file);
  }
}

/**
 * xcms_conf_write()
 */
function xcms_conf_write($conf) {
  file_put_contents(ROOT_DIR . '/conf.json', json_encode($conf, JSON_PRETTY_PRINT));
}

/**
 * xcms_page_handler()
 */
function xcms_page_handler() {
  $return = NULL;

  // set front page / empty ?q=
  $path = empty($_GET['q']) ? variable_get('site_front', 'nocontent') : $_GET['q'];

  if ($menu_item = xcms_path_get($path)) {

    // inc file arg requires full path
    if (!empty($menu_item['file'])) {
      require_once $menu_item['file'];
    }

    // check access and page callback
    if (xcms_check_access($menu_item['access'])) {
      return call_user_func($menu_item['callback']);
    }
    else {
      header('HTTP/1.1 403 Forbidden');
      xcms_set_title('403 Forbidden');
      return 'You are not allowed to access this page.';
    }
  }

  header('HTTP/1.1 404 Not Found');
  xcms_set_title('404 Not Found');
  return 'The requested page could not be found.';
}

/**
 * xcms_check_access()
 */
function xcms_check_access($access) {
  if (function_exists($access)) {
    return call_user_func($access);
  }
  return $access;
}

/**
 * xcms_path_get()
 */
function xcms_path_get($path) {
  global $_menu;
  $menu = $_menu;

  // TODO Tabs functions
  $defaults = array(
    'title' => '',
    'weight' => 0,
    'access' => FALSE,
    'file' => '',
    // 'type' => MENU_ITEM,
  );

  // translate placeholder menu % as args (current page)
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

  // work backwards to search for a valid menu path
  while ($path && !array_key_exists($path, $menu)) {
    $path = substr($path, 0, strrpos($path, '/'));
  }

  // no suitable path found return NULL
  if (!array_key_exists($path, $menu)) {
    return;
  }

  xcms_set_title($menu[$path]['title']);
  return array_merge($defaults, $menu[$path]);
}

/**
 * xcms_menu_get()
 */
function xcms_menu_get() {
  global $_menu;
  $_menu = xcms_modules_invoke('menu');
}

/**
 * xcms_modules_list()
 */
function xcms_modules_list() {
  $modules = array(
    'system',
    'test',
  );
  return $modules;
}

/**
 * xcms_modules_load()
 */
function xcms_modules_load() {
  foreach (xcms_modules_list() as $module) {
    include_once (ROOT_DIR . '/modules/' . $module . '/' . $module . '.module');
  }
}

/**
 * xcms_modules_invoke()
 */
function xcms_modules_invoke($hook) {
  $return = array();
  foreach (xcms_modules_list() as $module) {
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
    $output = '';
    if (!empty($_SESSION['messages'])) {
      foreach ($_SESSION['messages'] as $class => $messages) {
        $output .= '<div class="messages ' . $class . '">' . implode('<br />', $messages) . '</div>';
      }
      $_SESSION['messages'] = array();
    }
    return $output;
  }
}

/**
 * theme_page()
 */
function theme_page($content) {
  global $conf;

  $title = strip_tags(xcms_set_title());
  $vars = array(
    'title' => $title,
    'directory' => $conf['base_path'] . 'themes/' . $conf['current_theme'],
    'head_title' => $conf['site_name'] . ' | ' . $title,
    'front_url' => url('<front>'),
    'messages' => xcms_set_message(),
  );

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

// usage: return ('<ul>' . theme_list($list_items) . '</ul>');

/**
 * theme_list()
 */
function theme_list($list_items, $options = array()) {
  $options += array(
    'type' => 'ul',
    'attributes' => array(),
  );

  $output = '';
  foreach ($list_items as $item) {
    if (next($list_items)) {
      $next_item = current($list_items);
    }
    if (is_array($item)) {
      $output .= '<ul>' . theme_list($item) . '</ul></li>';
    }
    elseif (is_array($next_item)) {
      $output .= '<li>' . $item;
    }
    else {
      $output .= '<li>' . $item . '</li>';
    }
  }
  return $output;
}

/*
function theme_list_test($list_items, $options = array()) {

  $options += array(
    'type' => 'ul',
    'attributes' => array(),
  );
  $close_li = FALSE;
  $output = '<' . $options['type'] . attributes($options['attributes']) . '>';
  foreach ($list_items as $key => $item) {
    if (is_array($item)) {
      $output .= theme_list($item);
    }
    else {
      $output .= '<li>' . $item . '</li>';
    }
  }
  $output .= '</' . $options['type'] . '>';
  return $output;
}

function theme_list($list_items, $options = array()) {
  $options += array(
    'type' => 'ul',
    'attributes' => array(),
  );
  $close_li = FALSE;
  $output = '<' . $options['type'] . attributes($options['attributes']) . '>';
  foreach ($list_items as $key => $item) {
    if (is_array($item)) {
      $output .= theme_list($item);
    }
    else {
      // TODO THIS works but IS BROKEN HTML!!
      $output .= '</li>';
      $output .= '<li>' . $item;
    }
  }
  $output .= '</' . $options['type'] . '>';
  return $output;
}
*/

/**
 * xcms_goto()
 */
function xcms_goto($url) {
  header('Location: ' . url($url));
  exit();
}

/**
 * l()
 */
function l($title, $path, $options = array()) {
  // TODO options attributes()
  return '<a href="' . url($path) . '">' . $title . '</a>';
}

/**
 * url()
 */
function url($path = NULL) {
  global $conf;

  if ($path == '<front>') {
    $path = NULL;
  }

  if (strpos($path, ':') !== FALSE) {
    // external url
    return $path;
  }
  else {
    // internal path
    return $conf['base_path'] . trim($path, '/');
  }
}

/**
 * attributes()
 */
function attributes($attributes) {
  $output = ' ';
  foreach ($attributes as $key => $value) {
    $output .= $key . '="' . $value . '" ';
  }
  return $output;
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
