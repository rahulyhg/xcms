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
function dsm($input) {
  xcms_set_message('<pre>' . print_r($input, TRUE) . '</pre>');
}

/**
 * Sorts arrays with weight, eg. Menu, CSS and JavaScript resources.
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

