<?php

/**
 * @file
 */

/**
 * user_admin_page().
 */
function user_admin_page() {
  $output = '';
  $users = user_load_all();

  $header = array('UID', 'Username', 'E-mail', 'Created', 'Operations');
  $rows = array();

  foreach ($users as $account) {
    $account = (object) $account;
    if ($account->uid) {

      $links = array(
        'edit' => array(
          'title' => 'Edit',
          'href' => 'user/' . $account->uid . '/edit',
        ),
        'delete' => array(
          'title' => 'Delete',
          'href' => 'user/' . $account->uid . '/delete',
        ),
      );

      $ops = theme('links', array('links' => $links));
      $rows[] = array($account->uid, $account->name, $account->mail, format_date($account->created), $ops);
    }
  }

  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

