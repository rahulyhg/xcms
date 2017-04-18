<?php

/**
 * @file
 */

/**
 * Render node page.
 */
function node_page() {
  $node = node_load(arg(1));

  if (empty($node)) {
    xcms_not_found();
  }

  xcms_set_title($node->title);

  $directory = variable_get('site_theme', 'core/themes/default');
  $content = '';

  // Load field_*
  foreach ($node->data as $key => $value) {
    if (strpos($key, 'field_') !== FALSE) {
      $structure = node_get_type_structure($node->type);
      if (empty($structure['fields'][$key]['hide'])) {
        $content .= '<div class="' . strip_class($key) . '">' . $node->data[$key] . '</div>';
      }
    }
  }

  // TODO fix missing content hack. if empty content is returned it wont render theme page
  $content .= ' ';

  // Templates in order of preference.
  $templates = array(
    $directory . '/node--' . $node->nid,
    $directory . '/node--' . $node->type,
    // May want these in future.
    // $directory . '/node',
    // 'core/node/node.tpl.php',
  );

  foreach ($templates as $template) {
    $render = render_template($directory . '/node--' . $node->nid, array('content' => $content, 'node' => $node));
    if (!empty($render)) {
      return $render;
    }
  }

  return $content;
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

  $form['submit'] = array(
    'type' => 'submit',
    'value' => 'Save',
    'weight' => 999,
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

  // Set fields, node object and data column.
  $data = array();
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, 'field_') !== FALSE) {
      $data[$key] = $value;
    }
  }
  $node->data = $data;

  // Save the node.
  node_save($node);

  xcms_set_message('The <em>' . $node->type . '</em> has been saved.');

  $form_state['redirect'] = 'node/' . $node->nid;
}

