<?php

/**
 * Common functions that need to be referenced.
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
  foreach ($attributes as $attribute => & $data) {
    $data = implode(' ', (array) $data);
    $data = $attribute . '="' . check_plain($data) . '"';
  }
  return $attributes ? ' ' . implode(' ', $attributes) : '';
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

/**
 * Encode special characters in plain-text.
 */
function check_plain($text) {
  return htmlspecialchars($text, ENT_QUOTES, 'UTF-8');
}

/**
 * Create a css class safe string.
 */
function strip_class($text) {

  // lower case everything
  $text = strtolower($text);

  // make alphanumeric (removes all other characters)
  $text = preg_replace('/[^a-z0-9_\s-]/', '', $text);

  // clean up multiple dashes or whitespaces
  $text = preg_replace('/[\s-]+/', ' ', $text);

  // convert whitespaces and underscore to dash
  $text = preg_replace('/[\s_]/', '-', $text);
  return $text;
}

