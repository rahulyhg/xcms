<?php

/**
 * @file
 */

/**
 *
 */
function node_page() {
  return 'nodes list';
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
  $node = node_load(arg(1));

  $form['title'] = array(
    'type' => 'text',
    'title' => 'Title',
    'default_value' => issetor($node->title),
    'required' => TRUE,
  );

  $form['body'] = array(
    'type' => 'textarea',
    'title' => 'Body',
    'default_value' => issetor($node->data['body']),
    'required' => TRUE,
    'attributes' => array(
      'rows' => 10,
    ),
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

  $node = node_load(arg(1));

  $node = array(
    'title' => $form_state['values']['title'],
    'data' => $form_state['values'],
  );

  node_save($node);

  xcms_set_message('The changes have been saved.');
}

