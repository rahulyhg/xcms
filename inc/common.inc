<?php

/**
 * @file
 * Common functions that need to be referenced.
 */

/**
 * Xcms_goto().
 */
function xcms_goto($url) {
  header('Location: ' . url($url));
  exit();
}

/**
 * L().
 */
function l($title, $path, $options = array()) {
  $options['attributes']['href'] = url($path);

  // Set active class.
  if ($path == $_GET['q']) {
    $options['attributes']['class'][] = 'active';
  }

  return '<a' . attributes($options['attributes']) . '>' . $title . '</a>';
}

/**
 * Url().
 */
function url($path = NULL) {
  global $conf;

  if ($path == '<front>') {
    $path = NULL;
  }

  if (strpos($path, ':') !== FALSE) {
    // External url.
    return $path;
  }
  else {
    // Internal path.
    return $conf['base_path'] . trim($path, '/');
  }
}

/**
 * Attributes().
 */
function attributes($attributes) {
  foreach ($attributes as $attribute => & $data) {
    $data = implode(' ', (array) $data);
    $data = $attribute . '="' . check_plain($data) . '"';
  }
  return $attributes ? ' ' . implode(' ', $attributes) : '';
}

/**
 * Issetor().
 */
function issetor(&$var, $default = NULL) {
  return isset($var) ? $var : $default;
}

/**
 * Arg().
 */
function arg($index = 0) {
  $args = explode('/', $_GET['q']);
  if (array_key_exists($index, $args)) {
    return $args[$index];
  }
}

/**
 * TODO rewrite this as a module later.
 */
function dsm($input) {
  xcms_set_message('<pre>' . print_r($input, TRUE) . '</pre>');
}

/**
 * Order_cmp()
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
 * Order()
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

  // Lower case everything.
  $text = strtolower($text);

  // Make alphanumeric (removes all other characters).
  $text = preg_replace('/[^a-z0-9_\s-]/', '', $text);

  // Clean up multiple dashes or whitespaces.
  $text = preg_replace('/[\s-]+/', ' ', $text);

  // Convert whitespaces and underscore to dash.
  $text = preg_replace('/[\s_]/', '-', $text);
  return $text;
}

