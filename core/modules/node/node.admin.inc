<?php

/**
 * @file
 */

/**
 * Node_admin_page().
 * TODO pager.
 */
function node_admin_page() {
  $output = '';

  // TODO fix this, use theme(links) for add links / each content type
  // TODO display all content types, table for types (type_name, data_fields)?
  $output .= l('Add node (page)', 'node/add/page');

  $header = array('NID', 'Title', 'Created', 'Operations');
  $rows = array();

  $nodes = db_query('SELECT * FROM node ORDER BY nid')->fetchAll(PDO::FETCH_OBJ);

  foreach ($nodes as $node) {

    $options = array('query' => array('destination' => 'admin/node'));

    if (isset($node->nid)) {
      $links = array(
        'view' => array(
          'title' => 'View',
          'href' => 'node/' . $node->nid,
        ),
        'edit' => array(
          'title' => 'Edit',
          'href' => 'node/' . $node->nid . '/edit',
          'options' => $options,
        ),
        'delete' => array(
          'title' => 'Delete',
          'href' => 'node/' . $node->nid . '/delete',
          'options' => $options,
        ),
      );
      $ops = theme('links', array('links' => $links));

      $rows[] = array($node->nid, $node->title, format_date($node->created), $ops);
    }
  }

  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

