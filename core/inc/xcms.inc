<?php

/**
 * @file
 * Functions that need to be loaded on every request.
 */

/**
 * Contants
 *  MENU_*
 *  MENU_CALLBACK: hidden.
 *  MENU_DEFAULT_LOCAL_TASK: tab active
 *  MENU_LOCAL_TASK: tab inactive
 *  MENU_LOCAL_ACTION: action link.
 */
define('XCMS_VERSION', '1.x-dev');
define('MENU_NORMAL', 0);
define('MENU_CALLBACK', 1);
define('MENU_LOCAL_TASK', 2);
define('MENU_LOCAL_ACTION', 3);

/**
 * Bootstrap and load system includes.
 */
function xcms_init() {
  global $xcms_db, $conf, $base_path;

  // Start session.
  session_start();

  // Set error handler.
  set_error_handler('_xcms_error_handler');

  // TODO; if settings.php does not exit, don't load any modules. just bootstrap main
  // Connect to database.
  require ROOT_DIR . '/site/settings.php';
  $db = $settings_database;

  // TODO; if is_array($db), use mysql, else use sqlite
  // $xcms_db = new PDO('sqlite:sites/default/files/.ht.sqlite3');.
  $xcms_db = new PDO('mysql:host=' . $db['host'] . ';dbname=' . $db['database'], $db['username'], $db['password']);

  // Display PDO errors to screen via (_xcms_error_handler).
  $xcms_db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

  // Load site variables.
  $conf = db_query('SELECT * FROM variables')->fetchAll(PDO::FETCH_KEY_PAIR);

  // Load all enabled modules.
  xcms_modules_load();

  // Call hook_init.
  xcms_modules_invoke('init');

  // Get menu.
  xcms_menu_get();

  // Print page output.
  if ($content = xcms_page_handler()) {
    print theme_page(array('content' => $content));
  }
}

/**
 * Return XCMS version number.
 */
function xcms_version() {
  return XCMS_VERSION;
}

/**
 * Execute the page callback associated with the current path.
 */
function xcms_page_handler() {
  $return = NULL;

  // Initialize the $_GET['q'] variable to the proper normal path.
  $request_path = request_path();

  if (empty($request_path)) {
    $path = xcms_path_normal(variable_get('site_front', 'nocontent'));
  }
  else {
    $path = xcms_path_normal($request_path);
  }
  $_GET['q'] = $path;

  // Get the menu item and process request.
  if ($menu_item = xcms_path_get($path)) {

    // Inc file arg requires full path.
    if (!empty($menu_item['file'])) {
      require_once $menu_item['file'];
    }

    // Check access and page callback.
    if (xcms_check_access($menu_item['access'])) {
      xcms_set_title(issetor($menu_item['title']));
      return call_user_func_array($menu_item['callback'], $menu_item['arguments']);
    }
    else {
      xcms_access_denied();
    }
  }
  xcms_not_found();
}

/**
 * The requested URL path.
 */
function request_path() {
  global $base_path;
  $get_q = issetor($_GET['q']);
  return trim(str_replace(trim($base_path, '/'), '', $get_q), '/');
}

/**
 * The base URL for this site.
 */
function base_url() {
  $is_https = isset($_SERVER['HTTPS']) && strtolower($_SERVER['HTTPS']) == 'on';
  return ($is_https ? 'https' : 'http') . '://' . $_SERVER['HTTP_HOST'];
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
  global $_menu, $_local_tasks;
  $menu = $_menu;
  $_local_tasks = array();
  $local_parent = $path;

  // Translate placeholder menu % as args (current page).
  foreach ($menu as $link => $menu_item) {
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

    // If current page is tab add to local tasks.
    if (isset($_menu[$link]['type']) && $_menu[$link]['type'] == MENU_LOCAL_TASK) {
      $_local_tasks[$translated] = $menu[$translated];
    }
  }

  // Work backwards to find parent (default) local path.
  while ($local_parent && array_key_exists($local_parent, $_local_tasks)) {
    // TODO: this is not quite right, needs to return local_parent is returning parent of local parent.
    $local_parent = substr($local_parent, 0, strrpos($local_parent, '/'));
  }

  // Remove tabs from local tasks that are not for current menu route.
  foreach ($_local_tasks as $key => $value) {
    if (strpos($key, $local_parent) !== 0) {
      unset($_local_tasks[$key]);
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

  return $menu[$path];
}

/**
 * Get entire menu router.
 */
function xcms_menu_get() {
  global $_menu;

  $menu_item_defaults = array(
    'title' => '',
    'weight' => 0,
    'access' => FALSE,
    'callback' => NULL,
    'arguments' => array(),
    'access' => FALSE,
    'file' => '',
    'type' => MENU_NORMAL,
  );

  $_menu = xcms_modules_invoke('menu');

  foreach ($_menu as $path => $menu_item) {
    $_menu[$path] += $menu_item_defaults;
  }

  uasort($_menu, 'xcms_weight_sort');
}

/**
 * Recusively scan modules directory for .module files.
 *
 * @return array of files with module extension, key as module name
 */
function xcms_modules_list() {
  $modules = &xcms_static(__FUNCTION__);
  if (!isset($modules)) {
    $directories = array(
      'core/modules',
      'site/modules',
    );
    $regex = '#(?<!/)\.module$|^[^\.]*$#i';
    $files = new AppendIterator();
    foreach ($directories as $directory) {
      $files->append(new RecursiveIteratorIterator(new RecursiveRegexIterator(new RecursiveDirectoryIterator($directory), $regex)));
    }
    foreach ($files as $file) {
      $module = pathinfo($file->getPathname(), PATHINFO_FILENAME);
      // Check that the parent dir name is the same as the module info.
      if ($module == basename(pathinfo($file->getPathname(), PATHINFO_DIRNAME))) {
        $modules[$module] = $file->getPathname();
      }
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
  $args = func_get_args();
  unset($args[0]);
  foreach (xcms_modules_list() as $module => $path) {
    $function = $module . '_' . $hook;
    if (function_exists($function)) {
      $result = call_user_func_array($function, $args);
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
    $directories = array(
      'core/themes',
      'site/themes',
    );
    $regex = '#(?<!/)\.info$|^[^\.]*$#i';
    $files = new AppendIterator();
    foreach ($directories as $directory) {
      $files->append(new RecursiveIteratorIterator(new RecursiveRegexIterator(new RecursiveDirectoryIterator($directory), $regex)));
    }
    foreach ($files as $file) {
      $theme_info = xcms_get_theme_info($file->getPath());
      $themes[$file->getPath()] = $theme_info['name'];
    }
  }
  return $themes;
}

/**
 * Get information on a theme.
 *
 * @return array of information from theme.info file.
 */
function xcms_get_theme_info($directory) {
  $info_array = array();
  $theme_realpath = realpath($directory);
  $theme_infofile = $theme_realpath . '/' . basename($directory) . '.info';

  if (file_exists($theme_infofile)) {
    $info_array = parse_ini_file($theme_infofile);
  }

  $info_array += array(
    'name' => NULL,
    'stylesheets' => array(),
    'scripts' => array(),
    'settings' => array(),
  );

  return $info_array;
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

  // Get form array.
  $form = call_user_func_array($form_id, array(array(), &$form_state));

  // Merge form default attributes.
  $form['attributes'] = issetor($form['attributes'], array());
  $form['attributes'] += array(
    'method' => 'post',
    'id' => strip_class($form_id),
  );

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

    if (empty($form_errors)) {
      if (!empty($_GET['destination'])) {
        xcms_goto($_GET['destination']);
      }

      if (!empty($form_state['redirect'])) {
        xcms_goto($form_state['redirect']);
      }

      // Refresh page to avoid re-post.
      xcms_goto(current_path());
    }
  }

  // Print form html.
  $output .= '<form' . attributes($form['attributes']) . '>';

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

/**
 * Xcms goto.
 */
function xcms_goto($url = NULL) {
  header('Location: ' . url($url, array('absolute' => TRUE)));
  exit;
}

/**
 * Sets a persistent variable.
 */
function variable_set($name, $value) {
  global $conf;

  // Update global conf with new value.
  $conf[$name] = $value;

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
}

/**
 * Return a persistent variable.
 */
function variable_get($name, $default = NULL) {
  global $conf;
  return issetor($conf[$name], $default);
}

/**
 * Url.
 */
function url($path = NULL, $options = array()) {
  global $base_path;

  if ($path == '<front>') {
    $path = NULL;
  }

  // External url.
  if (strpos($path, ':') !== FALSE) {
    return $path;
  }

  // Internal path.
  else {
    $request = parse_url($path);

    // Path alias if there is one.
    $path = xcms_path_alias($request['path']);

    if (isset($request['query'])) {
      $path .= '?' . $request['query'];
    }

    $url = '/' . trim($base_path . '/' . $path, '/');

    if (empty($options['absolute'])) {
      return $url;
    }

    // Return absolute URL.
    return base_url() . $url;
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
    $path = current_path();
  }

  $args = explode('/', $path);
  if (array_key_exists($index, $args)) {
    return $args[$index];
  }
}

/**
 * Current_path().
 */
function current_path() {
  return $_GET['q'];
}

/**
 * Given a path alias, return the internal path it represents.
 */
function xcms_path_normal($path) {
  return xcms_path_lookup($path, 'normal');
}

/**
 * Given a internal path, return the path alias (opposite of xcms_normal_path).
 */
function xcms_path_alias($path) {
  return xcms_path_lookup($path, 'alias');
}

/**
 * Either a system path, an aliased path, or FALSE if no path was found.
 */
function xcms_path_lookup($path, $type = 'normal') {
  $paths = &xcms_static(__FUNCTION__);
  if (!isset($paths) || $type == 'refresh') {
    $paths = db_query('SELECT path, alias FROM paths')->fetchAll(PDO::FETCH_KEY_PAIR);
  }

  // Create a new variable, so array flip won't destroy the static variable.
  $path_list = $paths;

  if ($type == 'normal') {
    $path_list = array_flip($path_list);
  }

  return isset($path_list[$path]) ? $path_list[$path] : $path;
}

/**
 * Returns the path to a system item (module, theme, etc.).
 */
function xcms_get_path($type, $name) {
  $function = 'xcms_' . $type . 's_list';
  if (function_exists($function)) {
    $list = call_user_func($function);
    return array_key_exists($name, $list) ? dirname($list[$name]) : FALSE;
  }
  return FALSE;
}

/**
 * Xcms not found.
 */
function xcms_not_found() {
  header($_SERVER['SERVER_PROTOCOL'] . ' 404 Not Found');
  exit;
}

/**
 * Xcms access denied.
 */
function xcms_access_denied() {
  header($_SERVER['SERVER_PROTOCOL'] . ' 403 Forbidden');
  exit;
}

/**
 * Show debug information.
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
  $text = preg_replace('/[^a-z0-9_\/\s-]/', '', $text);

  // Clean up multiple dashes or whitespaces.
  $text = preg_replace('/[\s-\/]+/', ' ', $text);

  // Convert whitespaces, underscore and forward slashes to dash.
  $text = preg_replace('/[\s_\/]/', '-', $text);
  return $text;
}

/**
 * Verifies the syntax of the given e-mail address.
 */
function valid_email_address($mail) {
  return (bool) filter_var($mail, FILTER_VALIDATE_EMAIL);
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
 * Generates themed output.
 */
function theme($hook, $vars = array()) {
  $function = 'theme_' . $hook;
  if (function_exists($function)) {
    return call_user_func($function, $vars);
  }
}

/**
 * L.
 */
function l($title, $path, $options = array()) {

  // html => TRUE
  if (empty($options['html'])) {
    $title = check_plain($title);
  }

  if (isset($options['query'])) {
    $path .= '?' . http_build_query($options['query']);
  }

  $options['attributes']['href'] = url($path);

  // Set active class.
  if ($path == current_path()) {
    $options['attributes']['class'][] = 'active';
  }

  return '<a' . attributes($options['attributes']) . '>' . $title . '</a>';
}

/**
 * Theme page.
 */
function theme_page($vars) {
  $directory = variable_get('site_theme', 'core/themes/default');
  $theme_info = xcms_get_theme_info($directory);

  // Load stylesheets.
  foreach ($theme_info['stylesheets'] as $weight => $stylesheet) {
    xcms_add_css($directory . '/' . $stylesheet, array('weight' => $weight));
  }

  // Load scripts.
  foreach ($theme_info['scripts'] as $weight => $script) {
    xcms_add_js($directory . '/' . $script, array('weight' => $weight));
  }

  // System variables, can't be changed by vars / hook.
  $tabs = xcms_get_local_tasks();
  $title = check_plain(xcms_set_title());
  $is_front = current_path() == variable_get('site_front');

  // Common variables, can be changed in preprocess.
  $vars += array(
    'title' => $title,
    'head_title' => $title . ' | ' . check_plain(variable_get('site_name', 'XCMS')),
    'front_url' => url('<front>'),
    'canonical' => url($is_front ? '' : current_path(), array('absolute' => TRUE)),
    'shortlink' => url(xcms_path_normal(current_path()), array('absolute' => TRUE)),
    'page_top' => implode("\n", xcms_modules_invoke('header')),
    'page_bottom' => implode("\n", xcms_modules_invoke('footer')),
    'body_classes' => ($is_front ? 'front' : 'not-front') . ' page-' . arg(0),
    // 'content' => empty($content) ? ' ' : $content,
  );

  $vars['styles'] = xcms_get_css();
  $vars['scripts'] = xcms_get_js();

  // Load page preprocess function.
  if (file_exists($directory . '/template.php')) {
    require_once $directory . '/template.php';
    if (function_exists(basename($directory) . '_preprocess_page')) {
      call_user_func_array(basename($directory) . '_preprocess_page', array(&$vars));
    }
  }

  // Execute messages right at the end so we can debug.
  $vars['messages'] = xcms_set_message();

  // Extract $vars.
  extract($vars);

  require_once $directory . '/page.tpl.php';
}

/**
 * Get local tasks.
 */
function xcms_get_local_tasks() {
  global $_local_tasks;
  $local_task_links = array();

  foreach ($_local_tasks as $href => $menu_item) {
    if (xcms_check_access($menu_item['access'])) {
      $local_task_links[] = array(
        'title' => $menu_item['title'],
        'href' => $href,
      );
    }
  }

  // Only show tabs if more than 1 tab.
  if (count($local_task_links) > 1) {
    return theme('links', array('links' => $local_task_links, 'attributes' => array('class' => array('tabs', 'clearfix'))));
  }

  return NULL;
}

/**
 * Theme list.
 */
function theme_list($vars) {
  $vars += array(
    'items' => array(),
    'options' => array(),
    'type' => 'ul',
    'attributes' => array(),
    'root' => TRUE,
  );
  $output = $vars['root'] ? '<' . $vars['type'] . attributes($vars['attributes']) . '>' : NULL;

  foreach ($vars['items'] as $key => $item) {

    $li_attributes = array();
    if (!is_numeric($key)) {
      $li_attributes['class'] = $key;
    }

    // Iterate over items build a HTML compliant nested list.
    if (next($vars['items'])) {
      $next_item = current($vars['items']);
    }

    if (is_array($item)) {
      $output .= '<ul>' . theme_list(array('items' => $item, 'root' => FALSE)) . '</ul></li>';
    }
    elseif (isset($next_item) && is_array($next_item)) {
      $output .= '<li' . attributes($li_attributes) . '>' . $item;
    }
    else {
      $output .= '<li' . attributes($li_attributes) . '>' . $item . '</li>';
    }
  }
  $output .= $vars['root'] ? '</' . $vars['type'] . '>' : NULL;
  return $output;
}

/**
 * Theme links.
 */
function theme_links($vars) {

  // Merge defaults that we're using.
  $vars += array(
    'type' => 'ul',
    'links' => array(),
    'attributes' => array(
      'class' => array('links'),
    ),
  );

  return '<' . $vars['type'] . attributes($vars['attributes']) . '>' . _build_links($vars['links']) . '</' . $vars['type'] . '>';
}

function _build_links($links) {
  $output = '';
  foreach ($links as $key => $link) {

    // Merge defaults that we're using.
    $link += array(
      'title' => NULL,
      'href' => NULL,
      'options' => array(
        'attributes' => array(
          'class' => array(),
        ),
      ),
    );

    // List item has children, add "expanded" class.
    $li_attributes = array();
    if (array_key_exists('children', $link)) {
      $li_attributes = array('class' => array('expanded'));
    }

    $output .= '<li' . attributes($li_attributes) . '>';

    if (!is_numeric($key)) {
      $link['options']['attributes']['class'][] = $key;
    }

    $output .= l($link['title'], $link['href'], $link['options']);

    // Add children, via recursion.
    if (array_key_exists('children', $link)) {
      $output .= theme('links', array('links' => $link['children']));
    }
    $output .= '</li>';
  }
  return $output;
}

/**
 * Theme form element.
 */
function theme_form_element($vars) {
  $output = '';
  $vars += form_element_defaults();
  // Element wrapper.
  // $attributes['id'] = 'edit-' . strip_class($vars['name']);
  $attributes['class'][] = 'form-item';
  $attributes['class'][] = 'form-type-' . $vars['type'];
  if ($vars['required']) {
    $attributes['class'][] = 'form-required';
  }
  $output .= '<div' . attributes($attributes) . '>';
  // Set type and class on form item.
  $vars['attributes']['id'] = 'edit-' . strip_class($vars['name']);
  $vars['attributes']['type'] = $vars['type'];
  $vars['attributes']['name'] = $vars['name'];
  $vars['attributes']['class'][] = 'form-' . $vars['type'];
  $vars['attributes']['value'] = empty($vars['value']) ? $vars['default_value'] : $vars['value'];
  // HTML5 "required" validation.
  if ($vars['required']) {
    // $vars['attributes']['required'] = TRUE;.
  }
  if ($vars['title']) {
    $output .= '<label>' . $vars['title'] . '</label>';
  }
  // Check if there is a special function for the element.
  $function = 'theme_form_' . $vars['type'];
  if (function_exists($function)) {
    $output .= call_user_func($function, $vars);
  }
  else {
    // Default form element function.
    $output .= theme_form_input($vars);
  }
  if ($vars['description']) {
    $output .= '<div class="description">' . $vars['description'] . '</div>';
  }
  $output .= '</div>';
  return $output;
}

/**
 * Retrieves default values for the element array.
 */
function form_element_defaults() {
  return array(
    'attributes' => array(),
    'default_value' => NULL,
    'description' => NULL,
    'markup' => NULL,
    'name' => NULL,
    'options' => array(),
    'required' => FALSE,
    'title' => NULL,
    'type' => 'markup',
    'value' => NULL,
  );
}

/**
 * Theme form item.
 */
function theme_form_markup($vars) {
  return '<div' . attributes($vars['attributes']) . '>' . $vars['markup'] . '</div>';
}

/**
 * Theme form input.
 */
function theme_form_input($vars) {
  return '<input' . attributes($vars['attributes']) . ' />';
}

/**
 * Theme form submit.
 */
function theme_form_submit($vars) {
  $vars['attributes']['value'] = $vars['value'];
  return '<input' . attributes($vars['attributes']) . ' />';
}

/**
 * Theme form textarea.
 */
function theme_form_textarea($vars) {
  $text = $vars['attributes']['value'];
  unset($vars['attributes']['value'], $vars['attributes']['type']);
  return '<textarea' . attributes($vars['attributes']) . '>' . $text . '</textarea>';
}

/**
 * Theme form select.
 */
function theme_form_select($vars) {
  $options = '';
  unset($vars['attributes']['type']);
  foreach ($vars['options'] as $key => $value) {
    $option_attributes = array();
    $option_attributes['value'] = $key;
    if ($key == $vars['default_value']) {
      $option_attributes['selected'] = 'selected';
    }
    $options .= '<option' . attributes($option_attributes) . '>' . $value . '</option>';
  }
  return '<select' . attributes($vars['attributes']) . '>' . $options . '</select>';
}

/**
 * Theme table.
 */
function theme_table($vars) {
  $vars += array(
    'header' => array(),
    'rows' => array(),
    'attributes' => array(),
  );
  $output = '<table' . attributes($vars['attributes']) . '>';
  $output .= '<thead><tr>';
  foreach ($vars['header'] as $th) {
    $output .= '<th>' . $th . '</th>';
  }
  $output .= '</thead></tr>';
  foreach ($vars['rows'] as $row) {
    $output .= '<tr><tbody>';
    foreach ($row as $td) {
      $output .= '<td>' . $td . '</td>';
    }
    $output .= '</tr></tbody>';
  }
  $output .= '</table>';
  return $output;
}

/**
 * Adds a cascading stylesheet to the stylesheet queue.
 */
function xcms_add_css($data = NULL, $options = array()) {
  $css = &xcms_static(__FUNCTION__);
  if (isset($data)) {
    $options += array(
      'weight' => 0,
      'media' => 'all',
    );
    $css[$data] = $options;
  }
  return $css;
}

/**
 * Returns all stylesheets to print on page.
 */
function xcms_get_css() {
  $styles = array();
  $css = xcms_add_css();
  uasort($css, 'xcms_weight_sort');
  foreach ($css as $path => $options) {
    $attributes = array(
      'type' => 'text/css',
      'rel' => 'stylesheet',
      'href' => url($path) . '?' . variable_get('cache_timestamp'),
      'media' => $options['media'],
    );
    $styles[] = '<link' . attributes($attributes) . ' />';
  }
  return implode("\n", $styles);
}

/**
 * Adds a JavaScript file to the page.
 */
function xcms_add_js($data = NULL, $options = array()) {
  $js = &xcms_static(__FUNCTION__);
  if (isset($data)) {
    $options += array(
      'weight' => count($js) / 100,
      'type' => 'file',
      'data' => $data,
    );
    $js[] = $options;
  }
  return $js;
}

/**
 * Returns a themed presentation of all JavaScript code for the current page.
 */
function xcms_get_js() {
  global $base_path;
  $scripts = array();
  $settings = array();
  $js = xcms_add_js();
  if (is_array($js)) {
    uasort($js, 'xcms_weight_sort');
    foreach ($js as $options) {
      $data = NULL;
      $attributes = array(
        'type' => 'text/javascript',
      );
      switch ($options['type']) {
        case 'setting':
          $settings += $options['data'];
          break;

        case 'inline':
          $data = $options['data'];
          break;

        default:
          $attributes['src'] = url($options['data']) . '?' . variable_get('cache_timestamp');
      }
      $scripts[] = '<script' . attributes($attributes) . '>' . $data . '</script>';
    }
  }
  // Merge default javascript variables.
  $settings += array(
    'basePath' => $base_path,
    'themeUrl' => url(variable_get('site_theme', 'core/themes/default')),
    'cacheTimestamp' => variable_get('cache_timestamp'),
  );
  $scripts[] = '<script type="text/javascript">var XCMS = ' . json_encode((object) $settings) . '</script>';
  return implode("\n", $scripts);
}

/**
 * Return array of timezones for form select options.
 */
function tz_options() {
  $tz = timezone_identifiers_list();
  return array_combine($tz, $tz);
}

/**
 *
 */
function format_date($timestamp) {
  // TODO add user / site time zone - currently only local.
  return date(variable_get('date_format', 'Y-m-d h:i'), $timestamp);
}

/**
 * Renders a system default template, which is essentially a PHP template.
 */
function render_template($template_file, $vars) {
  extract($vars, EXTR_SKIP);
  ob_start();
  if (file_exists($template_file . '.tpl.php')) {
    include $template_file . '.tpl.php';
  }

  return ob_get_clean();
}

