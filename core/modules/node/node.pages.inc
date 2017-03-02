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
 *
 */
function node_form($form, &$form_state) {

  // Add mode, create new node object.
  if (arg(1) == 'add' && arg(2)) {
    $node = (object)array(
      'type' => arg(2),
      'path' => NULL,
    );
    xcms_set_title('Add ' . $node->type);
  }
  elseif (is_numeric(arg(1)) && arg(2) == 'edit') {
    $node = node_load(arg(1));
    xcms_set_title('Edit ' . $node->type);
  }

  if (empty($node)) {
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

  // Load node hook fields from Field API.
  $fields = node_get_type_fields($node->type);
  foreach ($fields as $field) {
    $form += xcms_modules_invoke($field, $node);
  }

  $form['path'] = array(
    'type' => 'text',
    'title' => 'Path',
    'default_value' => $node->path,
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
  $node->path = $form_state['values']['path'];

  // TODO move to fields table, use node save hook
  $node->data['field_body'] = $form_state['values']['field_body'];

  node_save($node);
  xcms_set_message('The ' . $node->type . ' has been saved.');
  $form_state['redirect'] = 'node/' . $node->nid . '/edit';
}

