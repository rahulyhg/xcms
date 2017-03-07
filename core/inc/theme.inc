<?php

/**
 * @file
 * The theme functions, controls the output.
 */

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
  $title = strip_tags(xcms_set_title());
  $is_front = current_path() == variable_get('site_front');

  // Common variables, can be changed in preprocess.
  $vars += array(
    'title' => $title,
    'head_title' => variable_get('site_name', 'XCMS') . ' | ' . $title,
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

