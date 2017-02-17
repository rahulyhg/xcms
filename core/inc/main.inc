<?php

/**
 * @file
 * Functions that need to be loaded on every request.
 */

/**
 * Contants
 *  MENU_*
 *  MENU_CALLBACK: hidden.
 */
define('XCMS_VERSION', '1.x-dev');
// define('MENU_CALLBACK', 0);
// define('MENU_NORMAL', 1);.

/**
 * Bootstrap and load system includes.
 */
function xcms_init() {
  global $xcms_db, $conf, $_menu, $base_path;

  // TODO where to put this? also no session_destroy() is in place!!
  // Start session.
  session_start();

  // Set error handler.
  set_error_handler('_xcms_error_handler');

  require ROOT_DIR . '/core/inc/common.inc';
  require ROOT_DIR . '/core/inc/theme.inc';

  // Connect to database.
  // $xcms_db = new PDO('sqlite:sites/default/files/.ht.sqlite3');
  require ROOT_DIR . '/sites/default/settings.php';

  $db = $settings_database;
  $xcms_db = new PDO('mysql:host=' . $db['host'] . ';dbname=' . $db['database'], $db['username'], $db['password']);

  // Load site variables.
  xcms_conf_read();

  // Load all enabled modules.
  xcms_modules_load();

  // Call hook_init.
  xcms_modules_invoke('init');

  // Get menu.
  xcms_menu_get();

  if ($return = xcms_page_handler()) {
    print theme_page(array('content' => $return));
  }

  // TODO where to put this??
  session_destroy();
}

/**
 * Return XCMS version number.
 */
function xcms_version() {
  return XCMS_VERSION;
}

/**
 * Read site configuration from file.
 */
function xcms_conf_read() {
  global $conf, $base_path;
  $conf = db_query('SELECT * FROM variables')->fetchAll(PDO::FETCH_KEY_PAIR);

  // TODO get and merge predefined conf settings / extras from file - load conf settings.php? needed for mysql in future too.
  $conf['base_path'] = issetor($base_path, '/');
}

/**
 * Execute the page callback associated with the current path.
 */
function xcms_page_handler() {
  global $base_path;

  $return = NULL;

  // Set front page / empty ?q=.
  $path = empty($_GET['q']) ? variable_get('site_front', 'nocontent') : $_GET['q'];

  // remove base path from path for menu query
  $path = trim(str_replace(trim($base_path, '/'), '', $path), '/');

  if ($menu_item = xcms_path_get($path)) {

    // Inc file arg requires full path.
    if (!empty($menu_item['file'])) {
      require_once $menu_item['file'];
    }

    // Check access and page callback.
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
 * Menu access helper function.
 */
function xcms_check_access($access) {
  if (function_exists($access)) {
    return call_user_func($access);
  }
  return $access;
}

/**
 * Xcms_path_get().
 */
function xcms_path_get($path) {
  global $_menu;
  $menu = $_menu;

  // TODO Tabs functions.
  $defaults = array(
    'title' => '',
    'weight' => 0,
    'access' => FALSE,
    'file' => '',
    // 'type' => MENU_ITEM,.
  );

  // Translate placeholder menu % as args (current page).
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

  // Work backwards to search for a valid menu path.
  while ($path && !array_key_exists($path, $menu)) {
    $path = substr($path, 0, strrpos($path, '/'));
  }

  // No suitable path found return NULL.
  if (!array_key_exists($path, $menu)) {
    return;
  }

  xcms_set_title(issetor($menu[$path]['title']));
  return array_merge($defaults, $menu[$path]);
}

/**
 * Xcms_menu_get().
 */
function xcms_menu_get() {
  global $_menu;
  $_menu = xcms_modules_invoke('menu');
}

/**
 * Recusively scan modules directory for .module files.
 *
 * @return array of files with module extension, key as module name
 */
function xcms_modules_list() {
  $modules = &xcms_static(__FUNCTION__);

  if (!isset($modules)) {

    // TODO include those from multisite: sites/*/modules
    $directories = array(
      'core/modules',
      'sites/all/modules',
    );

    $regex = '#(?<!/)\.module$|^[^\.]*$#i';

    $files = new AppendIterator();
    foreach ($directories as $directory) {
      $files->append(new RecursiveIteratorIterator(new RecursiveRegexIterator(new RecursiveDirectoryIterator($directory), $regex)));
    }

    foreach ($files as $file) {
      $module = pathinfo($file->getPathname(), PATHINFO_FILENAME);
      $modules[$module] = $file->getPathname();
    }
  }
  return $modules;
}

/**
 * Include all module found from xcms_modules_list().
 */
function xcms_modules_load() {
  foreach (xcms_modules_list() as $module => $path) {
    include_once ROOT_DIR . '/' . $path;
  }
}

/**
 * Invokes a hook in all enabled modules that implement it.
 */
function xcms_modules_invoke($hook) {
  $return = array();
  foreach (xcms_modules_list() as $module => $path) {
    $function = $module . '_' . $hook;
    if (function_exists($function)) {
      $result = call_user_func($function);
      if (is_array($result)) {
        $return = array_merge($return, $result);
      }
    }
  }
  return $return;
}

/**
 * Recusively scan themes directory for .info files.
 *
 * @return array of files with info extension, key as module name
 */
function xcms_themes_list() {
  $themes = &xcms_static(__FUNCTION__);

  if (!isset($themes)) {

    // TODO include those from multisite: sites/*/themes
    $directories = array(
      'core/themes',
      'sites/all/themes',
    );

    $regex = '#(?<!/)\.info$|^[^\.]*$#i';

    $files = new AppendIterator();
    foreach ($directories as $directory) {
      $files->append(new RecursiveIteratorIterator(new RecursiveRegexIterator(new RecursiveDirectoryIterator($directory), $regex)));
    }

    foreach ($files as $file) {
      $theme = pathinfo($file->getPathname(), PATHINFO_FILENAME);
      $themes[$file->getPath()] = $theme;
    }
  }
  return $themes;
}

/**
 * Sets the title of the current page.
 */
function xcms_set_title($title = NULL) {
  $stored_title = &xcms_static(__FUNCTION__);

  if (isset($title)) {
    $stored_title = $title;
  }

  return $stored_title;
}

/**
 * Check if a variable is set, return default if not.
 */
function issetor(&$var, $default = NULL) {
  return isset($var) ? $var : $default;
}

/**
 * Sets a message to display to the user.
 */
function xcms_set_message($message = NULL, $type = 'status') {
  if ($message) {
    $_SESSION['messages'][$type][] = $message;
  }
  else {
    $output = '';
    if (!empty($_SESSION['messages'])) {
      foreach ($_SESSION['messages'] as $class => $messages) {
        $output .= '<div class="messages ' . $class . '">' . theme('list', array('items' => $messages)) . '</div>';
      }
      $_SESSION['messages'] = array();
    }
    return $output;
  }
}

/**
 * Provides central cached static variable storage.
 */
function &xcms_static($name, $value = NULL) {
  static $data = array();
  if ($value) {
    $data[$name] = $value;
  }
  return $data[$name];
}

/**
 * Returns a form array for a given form ID.
 */
function xcms_get_form($form_id) {
  $form_state = array();
  return xcms_build_form($form_id, $form_state);
}

/**
 * Form builder function.
 */
function xcms_build_form($form_id, &$form_state) {
  $output = '';

  // Set $form_state defaults.
  $form_state += array(
    'values' => issetor($_POST, array()),
  );

  // Form attributes.
  // TODO array_merge form attributes (overrides) from $form[].
  $attributes = array(
    'action' => url($_GET['q']),
    'method' => 'post',
    'id' => strip_class($form_id),
  );

  // Get form array.
  $form = call_user_func_array($form_id, array(array(), &$form_state));

  // Check for default validate and submit handlers.
  $form['validate_callbacks'][] = $form_id . '_validate';
  $form['submit_callbacks'][] = $form_id . '_submit';

  // Form has been submitted, process it.
  if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    // Check form elements for missing required fields.
    foreach ($form as $name => $element) {

      // Set element value from post.
      if (isset($form_state['values'][$name])) {
        $form[$name]['value'] = $form_state['values'][$name];
        unset($form[$name]['default_value']);
      }

      if (isset($element['required']) && empty($form_state['values'][$name])) {
        form_set_error($name, $element['title'] . ' field is required.');

        // Don't process validate callbacks if form required empty.
        $form['validate_callbacks'] = array();
      }
    }

    // Validation callbacks.
    foreach ($form['validate_callbacks'] as $callback) {
      if (function_exists($callback)) {
        call_user_func_array($callback, array($form, &$form_state));
      }
    }

    // Check for form errors. Set by form_set_error()
    $form_errors = xcms_static('form_set_error');
    if (!empty($form_errors)) {
      foreach ($form_errors as $name => $message) {
        xcms_set_message($message, 'error');
        $form[$name]['attributes']['class'][] = 'error';
      }

      // Don't process submit callbacks if form has errors.
      $form['submit_callbacks'] = array();
    }

    // Submit callbacks.
    foreach ($form['submit_callbacks'] as $callback) {
      if (function_exists($callback)) {
        call_user_func_array($callback, array($form, &$form_state));
      }
    }

    // TODO this flow is bad, somehow break the foreach to stop submission and redirect if no validation.
    if (empty($form_errors)) {
      // Redirect to $form['redirect'] or self after submission.
      xcms_goto(empty($form['redirect']) ? $_GET['q'] : $form['redirect']);
    }
  }

  // Print form html.
  $output .= '<form' . attributes($attributes) . '>';

  // Render the form elements.
  foreach ($form as $name => $element) {
    $element['name'] = $name;
    $output .= theme_form_element($element);
  }

  $output .= '</form>';
  return $output;
}

/**
 * Files an error against a form element.
 */
function form_set_error($name = NULL, $message) {
  $form_errors = &xcms_static(__FUNCTION__, array());
  if (isset($name)) {
    $form_errors[$name] = $message;
  }
  return $form_errors;
}

/**
 * Provides custom PHP error handling.
 */
function _xcms_error_handler($error_level, $message, $filename, $line, $context) {
  xcms_set_message('<i>' . $message . '</i>' . ' in <strong>' . $filename . '</strong> on line ' . $line, 'error');
}

