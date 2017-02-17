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
function variable_set($name, $value) {
  global $conf;

  // Check for existing name.
  $variable_exists = db_query('SELECT count(name) FROM variables WHERE name = :name', array(':name' => $name))->fetchColumn();

  // Key exists do update.
  if ($variable_exists) {
    db_query('UPDATE variables SET value = :value WHERE name = :name', array(':value' => $value, ':name' => $name));
  }
  // Key does not exist do insert.
  else {
    db_query('INSERT INTO variables (name, value) VALUES (:name, :value)', array(':value' => $value, ':name' => $name));
  }

  // Update global conf with new value.
  $conf[$name] = $value;
}

/**
 * Return a persistent variable.
 */
function variable_get($name, $default = NULL) {
  global $conf;
  return issetor($conf[$name], $default);
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
    return '/' . trim($conf['base_path'] . '/' . $path, '/');
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
function xsm($input = NULL) {
  xcms_set_message('<pre>' . print_r($input, TRUE) . '</pre>');
}

/**
 * Sorts arrays with weight, eg. Menu, CSS and JavaScript resources.
 *
 * @usage uasort($css, 'xcms_weight_sort');
 */
function xcms_weight_sort($a, $b) {
  if ($a['weight'] < $b['weight']) {
    return -1;
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
  return date(variable_get('date_format', 'Y-m-d h:i'), $timestamp);
}

/**
 * Executes an arbitrary query string against the active database.
 *
 * Fetch next (or a single) row as a stdClass object.
 * $record = $result->fetchObject();
 *
 * Retrieve a 2-column result set as an associative array of field 1 => field 2.
 * $record = $result->fetchAll(PDO::FETCH_KEY_PAIR);
 *
 * Fetch data from specific column from next row
 * Defaults to first column if not specified as argument
 * Grabs the title from the next row
 * $data = $result->fetchColumn(1);
 *
 * Retrieve all records into an indexed array of stdClass objects.
 * $result->fetchAll(PDO::FETCH_OBJ);
 *
 * Retrieve all records into an indexed associative array.
 * $result->fetchAll(PDO::FETCH_ASSOC);
 */
function db_query($sql, $args = array()) {
  global $xcms_db;
  $query = $xcms_db->prepare($sql);
  $query->execute($args);
  return $query;
}

/**
 * Return array of timezones for form select options.
 */
function tz_options() {
  $tz = timezone_identifiers_list();
  return array_combine($tz, $tz);
}
