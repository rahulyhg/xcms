<?php

/**
 * @file
 */

/**
 * node_admin_page().
 * TODO pager
 */
function node_admin_page() {
  $output = '';

  $header = array('NID', 'Title', 'Created', 'Operations');
  $rows = array();

  $nodes = db_query('SELECT * FROM users ORDER BY uid')->fetchAll(PDO::FETCH_OBJ);

  foreach ($nodes as $node) {
    // $node = (object) $node;
    if (isset($node->nid)) {

      $links = array(
        'edit' => array(
          'title' => 'Edit',
          'href' => 'node/' . $node->nid . '/edit',
        ),
        'delete' => array(
          'title' => 'Delete',
          'href' => 'node/' . $node->nid . '/delete',
        ),
      );

      $ops = theme('links', array('links' => $links));
      $rows[] = array($node->nid, $node->title, format_date($node->created), $ops);
    }
  }

  $output .= l('Add node', 'node/add');
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

