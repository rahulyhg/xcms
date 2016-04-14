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
 * Theme_page().
 */
function theme_page($vars) {
  global $conf;

  // TODO load styles from styles[] theme.info file.
  xcms_add_css('themes/default/base.css', array('weight' => -1));
  xcms_add_css('themes/default/style.css');
  xcms_add_js('themes/default/js/jquery-1.12.1.min.js', array('weight' => -1));
  xcms_add_js('themes/default/js/site.js');

  $title = strip_tags(xcms_set_title());

  $vars += array(
    'title' => $title,
    'directory' => 'themes/' . issetor($conf['current_theme'], 'default'),
    'head_title' => $conf['site_name'] . ' | ' . $title,
    'front_url' => url('<front>'),
    'messages' => xcms_set_message(),
    'page_top' => implode("\n", xcms_modules_invoke('header')),
    'page_bottom' => implode("\n", xcms_modules_invoke('footer')),
    'body_classes' => array(),
  );

  $vars['styles'] = xcms_get_css();
  $vars['scripts'] = xcms_get_js();

  // TODO $tabs
  // $tabs = theme_links(menu_get_local_tasks());

  // 1st attempt at a theme hook
  // TODO this should go into a function
  if (file_exists($vars['directory'] . '/template.php')) {
    require_once $vars['directory'] . '/template.php';
    if (function_exists($conf['current_theme'] . '_preprocess_page')) {
      call_user_func_array($conf['current_theme'] . '_preprocess_page', array(&$vars));
    }
  }

  // Extract $vars.
  extract($vars);

  $body_classes = implode(' ', $body_classes);

  require_once $directory . '/page.tpl.php';
}

/**
 * Theme_list().
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
    // TODO: first / last class?

    // Use the $key as class name if it's set and not numeric
    $li_attributes = array();
    if (!is_numeric($key)) {
      $li_attributes['class'] = $key;
    }

    // Iterate over items build a HTML compliant nested list
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
 * Theme_links().
 */
function theme_links($vars) {

  $vars += array(
    'links' => array(),
    'attributes' => array(
      'class' => array('links'),
    ),
  );

  $items = array();
  foreach ($vars['links'] as $key => $link) {
    $items[$key] = l($link['title'], $link['href']);
  }

  return theme('list', array('items' => $items, 'attributes' => $vars['attributes']));
}

/**
 * Theme_form_element().
 */
function theme_form_element($vars) {
  $output = '';
  $vars += form_element_defaults();

  // Element wrapper.
  $attributes['id'] = 'edit-' . strip_class($vars['name']);
  $attributes['class'][] = 'form-item';
  $attributes['class'][] = 'form-type-' . $vars['type'];
  if ($vars['required']) {
    $attributes['class'][] = 'form-required';
  }

  $output .= '<div' . attributes($attributes) . '>';

  // Set type and class on form item.
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
 * Theme_form_item().
 */
function theme_form_markup($vars) {
  return '<div' . attributes($vars['attributes']) . '>' . $vars['markup'] . '</div>';
}

/**
 * Theme_form_input().
 */
function theme_form_input($vars) {
  return '<input' . attributes($vars['attributes']) . ' />';
}

/**
 * Theme_form_submit().
 */
function theme_form_submit($vars) {
  $vars['attributes']['value'] = $vars['value'];
  return '<input' . attributes($vars['attributes']) . ' />';
}

/**
 * Theme_form_textarea().
 */
function theme_form_textarea($vars) {
  $text = $vars['attributes']['value'];
  unset($vars['attributes']['value'], $vars['attributes']['type']);
  return '<textarea' . attributes($vars['attributes']) . '>' . $text . '</textarea>';
}

/**
 * Theme_form_select().
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
 * Theme_table().
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
      'href' => url($path),
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
      'weight' => 0,
    );
    $js[$data] = $options;
  }
  return $js;
}

/**
 * Returns a themed presentation of all JavaScript code for the current page.
 *
 * TODO, loads all on every page, should be optimised to load only on page if needed?
 */
function xcms_get_js() {
  $scripts = array();
  $js = xcms_add_js();
  uasort($js, 'xcms_weight_sort');

  foreach ($js as $path => $options) {
    $attributes = array(
      'type' => 'text/javascript',
      'src' => url($path),
    );
    $scripts[] = '<script' . attributes($attributes) . '></script>';
  }
  return implode("\n", $scripts);
}

