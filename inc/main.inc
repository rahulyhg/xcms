<?php

/**
 * @file
 * Functions that need to be loaded on every request.
 */

/**
 * Contants
 * MENU_*
 *  MENU_CALLBACK: hidden.
 */
// define('MENU_CALLBACK', 0);
// define('MENU_NORMAL', 1);.

/**
 * Bootstrap and load system includes.
 */
function xcms_init() {
  global $conf, $_menu;
  require ROOT_DIR . '/inc/common.inc';
  require ROOT_DIR . '/inc/theme.inc';
  session_start();
  xcms_conf_read();
  xcms_modules_load();
  xcms_menu_get();
  if ($return = xcms_page_handler()) {
    print theme_page(array('content' => $return));
  }
}

/**
 * Xcms_conf_read().
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
 * Xcms_conf_write().
 */
function xcms_conf_write($conf) {
  file_put_contents(ROOT_DIR . '/conf.json', json_encode($conf, JSON_PRETTY_PRINT));
}

/**
 * Execute the page callback associated with the current path.
 */
function xcms_page_handler() {
  $return = NULL;

  // Set front page / empty ?q=.
  $path = empty($_GET['q']) ? variable_get('site_front', 'nocontent') : $_GET['q'];

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
 * Xcms_check_access().
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

  xcms_set_title($menu[$path]['title']);
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
    $regex = '#(?<!/)\.module$|^[^\.]*$#i';
    $files = new RecursiveIteratorIterator(new RecursiveRegexIterator(new RecursiveDirectoryIterator('modules'), $regex));
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
      $return = array_merge($return, $function());
    }
  }
  return $return;
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
 * Return a persistent variable.
 */
function variable_get($name, $default) {
  global $conf;
  return isset($conf[$name]) ? $conf[$name] : $default;
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
        $output .= '<div class="messages ' . $class . '">' . implode('<br />', $messages) . '</div>';
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
 *
 */
function xcms_build_form($form_id, &$form_state) {
  $output = '';

  // Set $form_state defaults.
  $form_state += array(
    'values' => issetor($_POST, array()),
    'redirect' => NULL,
    'form_id' => $form_id,
    'errors' => array(),
  );

  // Form attributes.
  // TODO array_merge form attributes (overrides) from $form[].
  $attributes = array(
    'action' => url($_GET['q']),
    'method' => 'post',
    'id' => strip_class($form_id),
  );

  // Get form array.
  $form = call_user_func($form_id);

  // Check for default validate and submit handlers.
  $form['validate_callbacks'][] = $form_id . '_validate';
  $form['submit_callbacks'][] = $form_id . '_submit';

  // Form has been submitted, process it.
  if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    // Check form elements for missing required fields.
    foreach ($form as $name => $element) {
      if (isset($element['required']) && empty($form_state['values'][$name])) {
        form_set_error($name, $element['title'] . ' field is required.', $form_state);

        // Don't process validate callbacks if form required empty.
        unset($form['validate_callbacks']);
      }
    }

    // Validation callbacks.
    foreach ($form['validate_callbacks'] as $callback) {
      if (function_exists($callback)) {
        call_user_func_array($callback, array($form, &$form_state));
      }
    }

    // Check for form errors. Set by form_set_error()
    if (!empty($form_state['errors'])) {
      foreach ($form_state['errors'] as $name => $message) {
        xcms_set_message($message, 'error');
        $form['name']['attributes']['class'][] = 'error';
      }

      // Don't process submit callbacks if form has errors.
      unset($form['submit_callbacks']);
    }

    // Submit callbacks.
    foreach ($form['submit_callbacks'] as $callback) {
      if (function_exists($callback)) {
        call_user_func_array($callback, array($form, &$form_state));
      }
    }
  }

  // Print form html.
  $output .= '<form' . attributes($attributes) . '>';

  // Render the form elements.
  foreach ($form as $name => $element) {
    $element['name'] = $name;

    // TODO, check this works and we can override it? this just here to satisfy validation form testing.
    $element['default_value'] = issetor($form_state['values'][$name]);

    $output .= theme_form_element($element);
  }

  $output .= '</form>';
  return $output;
}

/**
 * Files an error against a form element.
 * TODO don't want to have to parse the form_state, can we do this with static?
 */
function form_set_error($name, $message, &$form_state) {
  $form_state['errors'][$name] = $message;
}

/**
 *
 */
// Function xcms_validate_form($form_id, &$form, &$form_state) {}.

