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
    $add_links[$type] = array(
      'title' => 'Add ' . $structure['title'],
      'href' => 'node/add/' . $type,
    );
  }

  $output .= theme('links', array('links' => $add_links));

  $header = array('Title', 'Type', 'Created', 'Operations');
  $rows = array();
  $nodes = db_query('SELECT * FROM node ORDER BY nid DESC')->fetchAll(PDO::FETCH_OBJ);

  foreach ($nodes as $node) {
    $options = array('query' => array('destination' => 'admin/node'));

    $op_links = array(
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

    $node_type = node_get_type_structure($type);
    $rows[] = array(l($node->title, 'node/' . $node->nid), $node_type['title'], format_date($node->created), theme('links', array('links' => $op_links)));
  }

  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

