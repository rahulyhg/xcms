<?php

/**
 * @file
 * Common functions that need to be referenced.
 */

/**
 * Xcms_goto().
 */
function xcms_goto($url = NULL) {
  header('Location: ' . url($url));
  exit;
}

/**
 * Sets a persistent variable.
 */
function variable_set($key, $value) {
  global $xcms_db, $conf;

  // TODO replace this with a future db_merge() type function, try keep the logic in php (cleaner)
  // db_merge('variable')->key(array('name' => $name))->fields(array('value' => serialize($value)))->execute();
  $query = $xcms_db->query('SELECT count(key) FROM variables WHERE key = :key LIMIT 1');
  $query->execute(array(':key' => $key));

  if ($query->fetchColumn()) {
    $query = $xcms_db->prepare('UPDATE variables SET value = :value WHERE key = :key');
  }
  else {
    $query = $xcms_db->prepare('INSERT INTO variables (key, value) VALUES (:key, :value)');
  }
  $query->execute(array(':key' => $key, ':value' => $value));

  $conf[$key] = $value;
}

/**
 * Return a persistent variable.
 */
function variable_get($key, $default = NULL) {
  global $conf;
  return issetor($conf[$key], $default);
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
 * Arg().
 */
function arg($index = 0, $path = NULL) {
  if (empty($path)) {
    $path = $_GET['q'];
  }

  $args = explode('/', $path);
  if (array_key_exists($index, $args)) {
    return $args[$index];
  }
}

/**
 * TODO rewrite this as a module later.
 */
function xsm($input) {
  xcms_set_message('<pre>' . print_r($input, TRUE) . '</pre>');
}

/**
 * Sorts arrays with weight, eg. Menu, CSS and JavaScript resources.
 *
 * @usage uasort($css, 'xcms_weight_sort');
 */
function xcms_weight_sort($a, $b) {
  if ($a['weight'] < $b['weight']) {
    return - 1;
  }
  elseif ($a['weight'] > $b['weight']) {
    return 1;
  }
  else {
    return 0;
  }
}

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

/**
 * Verifies the syntax of the given e-mail address.
 */
function valid_email_address($mail) {
  return (bool) filter_var($mail, FILTER_VALIDATE_EMAIL);
}

/**
 *
 */
function format_date($timestamp) {
  // TODO add user / site time zone - currently only local.
  return date(variable_get('date_format', 'Y-m-d'), $timestamp);
}

