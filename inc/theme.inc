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

// usage: return ('<ul>' . theme_list($list_items) . '</ul>');

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

