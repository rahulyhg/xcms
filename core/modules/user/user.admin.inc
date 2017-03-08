<?php

/**
 * @file
 */

/**
 * user_admin_page().
 * TODO pager
 */
function user_admin_page() {

  $header = array('Username', 'E-mail', 'Created', 'Operations');
  $rows = array();

  $accounts = db_query('SELECT * FROM users ORDER BY uid')->fetchAll(PDO::FETCH_OBJ);

  foreach ($accounts as $account) {
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
      $rows[] = array(l($account->name, 'user/' . $account->uid), $account->mail, format_date($account->created), $ops);
    }
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

