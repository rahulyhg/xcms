<?php

/**
 * The theme functions, controls the output.
 */

/**
 * theme_page()
 */
function theme_page($content) {
  global $conf;

  $title = strip_tags(xcms_set_title());
  $vars = array(
    'title' => $title,
    'directory' => $conf['base_path'] . 'themes/' . $conf['current_theme'],
    'head_title' => $conf['site_name'] . ' | ' . $title,
    'front_url' => url('<front>'),
    'messages' => xcms_set_message(),
  );

  // TODO $tabs
  // $tabs = theme_links(menu_get_local_tasks());

  // TODO $body_classes
  //if ( .. is front) body_classes [] = is_front
  // $body_classes = array();
  //if (is_admin()) {
  //  $body_classes[] = 'is_admin';
  //}
  //$body_classes = implode(' ', $body_classes);

  // TODO hook_page for theme hooks.php
  // so variables can be modified from hook call

  extract($vars);
  require_once ROOT_DIR . '/themes/' . $conf['current_theme'] . '/page.php';
}

/**
 * theme_list()
 */
function theme_list($list_items, $options = array()) {
  $options += array(
    'type' => 'ul',
    'attributes' => array(),
    'root' => TRUE,
  );

  $output = $options['root'] ? '<' . $options['type'] . attributes($options['attributes']) . '>' : NULL;

  foreach ($list_items as $item) {
    if (next($list_items)) {
      $next_item = current($list_items);
    }
    if (is_array($item)) {
      $output .= '<ul>' . theme_list($item, array('root' => FALSE)) . '</ul></li>';
    }
    elseif (is_array($next_item)) {
      $output .= '<li>' . $item;
    }
    else {
      $output .= '<li>' . $item . '</li>';
    }
  }

  $output .= $options['root'] ? '</' . $options['type'] . '>' : NULL;

  return $output;
}

/**
 * theme_form_element()
 */
function theme_form_element($name, $element) {
  $element += array(
    'type' => 'markup',
    'title' => '',
    'description' => '',
    'required' => FALSE,
    'attributes' => array(),
    'markup' => '',
    'options' => array(),
  );

  $output = '';

  // element wrapper
  $attributes['id'] = 'edit-' . strip_class($name);
  $attributes['class'][] = 'form-item';
  $attributes['class'][] = 'form-type-' . $element['type'];
  if ($element['required']) {
    $attributes['class'][] = 'form-required';
  }

  $output .= '<div' . attributes($attributes) . '>';

  // set type and class on form item
  $element['attributes']['type'] = $element['type'];
  $element['attributes']['class'][] = 'form-' . $element['type'];

  if ($element['title']) {
    $output .= '<label>' . $element['title'] . '</label>';
  }

  // check if there is a special function for the element
  $function = 'theme_form_' . $element['type'];
  if (function_exists($function)) {
    $output .= call_user_func($function, $element);
  }
  else {
    // default form element function
    $output .= theme_form_input($element);
  }

  if ($element['description']) {
    $output .= '<div class="description">' . check_plain($element['description']) . '</div>';
  }

  $output .= '</div>';
  return $output;
}

/**
 * theme_form_item()
 */
function theme_form_markup($element) {
  return '<div' . attributes($element['attributes']) . '>' . $element['markup'] . '</div>';
}

/**
 * theme_form_input()
 */
function theme_form_input($element) {
  return '<input' . attributes($element['attributes']) . ' />';
}

/**
 * theme_form_textarea()
 */
function theme_form_textarea($element) {
  unset($element['attributes']['type']);
  return '<textarea' . attributes($element['attributes']) . ' ></textarea>';
}

/**
 * theme_form_select()
 */
function theme_form_select($element) {
  unset($element['attributes']['type']);

  $options = '';
  foreach ($element['options'] as $key => $value) {
    $options .= '<option value="' . $key . '">' . $value . '</option>';
  }

  return '<select' . attributes($element['attributes']) . ' >' . $options . '</select>';
}

/**
 * theme_table()
 */
function theme_table($header, $rows, $attributes) {
  $output = '<table' . attributes($attributes) . '>';
  $output .= '<tr>';
  foreach ($header as $th) {
    $output .= '<th>' . $th . '</th>';
  }
  $output .= '</tr>';

  foreach ($rows as $row) {
    $output .= '<tr>';
    foreach ($row as $td) {
      $output .= '<td>' . $td . '</td>';
    }
    $output .= '</tr>';
  }

  $output .= '</table>';
  return $output;
}

