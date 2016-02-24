<?php

/**
 * The theme functions, controls the output.
 */

/**
 * theme_page()
 */
function theme_page($vars) {
  global $conf;

  $title = strip_tags(xcms_set_title());
  $vars += array(
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
function theme_list($vars) {

  $vars += array(
    'items' => array(),
    'options' => array(),
    'type' => 'ul',
    'attributes' => array(),
    'root' => TRUE,
  );

  $output = $vars['root'] ? '<' . $vars['type'] . attributes($vars['attributes']) . '>' : NULL;

  foreach ($vars['items'] as $item) {
    if (next($vars['items'])) {
      $next_item = current($vars['items']);
    }
    if (is_array($item)) {
      $output .= '<ul>' . theme_list(array('items' => $item, 'root' => FALSE)) . '</ul></li>';
    }
    elseif (is_array($next_item)) {
      $output .= '<li>' . $item;
    }
    else {
      $output .= '<li>' . $item . '</li>';
    }
  }

  $output .= $vars['root'] ? '</' . $vars['type'] . '>' : NULL;

  return $output;
}

/**
 * theme_form_element()
 */
function theme_form_element($vars) {
  $vars += array(
    'name' => '',
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
  $attributes['id'] = 'edit-' . strip_class($vars['name']);
  $attributes['class'][] = 'form-item';
  $attributes['class'][] = 'form-type-' . $vars['type'];
  if ($vars['required']) {
    $attributes['class'][] = 'form-required';
  }

  $output .= '<div' . attributes($attributes) . '>';

  // set type and class on form item
  $vars['attributes']['type'] = $vars['type'];
  $vars['attributes']['class'][] = 'form-' . $vars['type'];

  if ($vars['title']) {
    $output .= '<label>' . $vars['title'] . '</label>';
  }

  // check if there is a special function for the element
  $function = 'theme_form_' . $vars['type'];
  if (function_exists($function)) {
    $output .= call_user_func($function, $vars);
  }
  else {
    // default form element function
    $output .= theme_form_input($vars);
  }

  if ($vars['description']) {
    $output .= '<div class="description">' . check_plain($vars['description']) . '</div>';
  }

  $output .= '</div>';
  return $output;
}

/**
 * theme_form_item()
 */
function theme_form_markup($vars) {
  return '<div' . attributes($vars['attributes']) . '>' . $vars['markup'] . '</div>';
}

/**
 * theme_form_input()
 */
function theme_form_input($vars) {
  return '<input' . attributes($vars['attributes']) . ' />';
}

/**
 * theme_form_textarea()
 */
function theme_form_textarea($vars) {
  unset($vars['attributes']['type']);
  return '<textarea' . attributes($vars['attributes']) . ' ></textarea>';
}

/**
 * theme_form_select()
 */
function theme_form_select($vars) {
  $options = '';
  unset($vars['attributes']['type']);
  foreach ($vars['options'] as $key => $value) {
    $options .= '<option value="' . $key . '">' . $value . '</option>';
  }
  return '<select' . attributes($vars['attributes']) . ' >' . $options . '</select>';
}

/**
 * theme_table()
 */
function theme_table($vars) {

  $vars += array(
    'header' => array(),
    'rows' => array(),
    'attributes' => array(),
  );

  $output = '<table' . attributes($vars['attributes']) . '>';
  $output .= '<tr>';
  foreach ($vars['header'] as $th) {
    $output .= '<th>' . $th . '</th>';
  }
  $output .= '</tr>';

  foreach ($vars['rows'] as $row) {
    $output .= '<tr>';
    foreach ($row as $td) {
      $output .= '<td>' . $td . '</td>';
    }
    $output .= '</tr>';
  }

  $output .= '</table>';
  return $output;
}

