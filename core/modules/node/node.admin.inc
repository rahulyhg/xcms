<?php

/**
 * @file
 */

/**
 * Node admin page.
 * TODO pager.
 */
function node_admin_page() {
  $output = '';

  foreach (node_get_types() as $type => $structure) {
    // xsm($structure);
    $add_links[$type] = array(
      'title' => 'Add ' . $structure['title'],
      'href' => 'node/add/' . $type,
    );
  }

  $output .= theme('links', array('links' => $add_links));

  $header = array('NID', 'Title', 'Created', 'Operations');
  $rows = array();
  $nodes = db_query('SELECT * FROM node ORDER BY nid')->fetchAll(PDO::FETCH_OBJ);

  foreach ($nodes as $node) {
    $options = array('query' => array('destination' => 'admin/node'));

    $op_links = array(
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

    $rows[] = array($node->nid, $node->title, format_date($node->created), theme('links', array('links' => $op_links)));
  }
    $output .= theme('table', array('header' => $header, 'rows' => $rows));
    return $output;
}

