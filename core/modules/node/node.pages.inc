<?php

/**
 * @file
 */

/**
 *
 */
function node_page() {
  $output = '';

  $node = node_load(arg(1));

  if (empty($node)) {
    xcms_not_found();
  }

  xcms_set_title($node->title);

  // TODO, foreach felds display field rendered / themed + weight?
  $output .= $node->data['field_body'];
  return $output;
}

/**
 * Node_form_page().
 */
function node_form_page() {
  return xcms_get_form('node_form');
}

/**
 *
 */
function node_form($form, &$form_state) {

  // Add mode, create new node object.
  if (arg(1) == 'add' && arg(2)) {
    $node = (object)array(
      'type' => arg(2),
    );
  }
  elseif (is_numeric(arg(1)) && arg(2) == 'edit') {
    $node = node_load(arg(1));
    if (empty($node)) {
      xcms_not_found();
    }
  }
  else {
    xcms_not_found();
  }

  // Store node object in post.
  $form_state['node'] = $node;

  $form['title'] = array(
    'type' => 'text',
    'title' => 'Title',
    'default_value' => issetor($node->title),
    'required' => TRUE,
  );

  // TODO node form hook to load fields depending on the node type.
  // TODO node type builder (no user interface - module hooks only)
  // TODO, form body should be loaded via hook_node_form in node.module for "page type" and not required
  $form['field_body'] = array(
    'type' => 'textarea',
    'title' => 'Body',
    'default_value' => issetor($node->data['field_body']),
    'attributes' => array(
      'rows' => 10,
    ),
  );

  $form['path'] = array(
    'type' => 'text',
    'title' => 'Path',
    'default_value' => issetor($node->data['path']),
  );

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Save',
  );

  return $form;
}

/**
 *
 */
function node_form_submit($form, &$form_state) {
  $node = $form_state['node'];
  $node->title = $form_state['values']['title'];
  $node->data = $form_state['values'];
  node_save($node);
  xcms_set_message($node->type . ' has been saved.');
}

