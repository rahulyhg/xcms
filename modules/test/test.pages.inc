<?php

/**
 * @file
 */

/**
 * test_page_inc()
 */
function test_page_inc() {
  return lipsum(10);
}

/**
 * test_page_list()
 */
function test_page_list() {

  /*
  $list_items = array(
    '1' => lipsum(1),
    '2' => lipsum(1),
    '3' => 'parent',
    '4' => array(
      '41' => lipsum(1),
      '42' => lipsum(1),
      '3' => 'parent',
      '4' => array(
        '41' => lipsum(1),
        '42' => lipsum(1),
        '43' => lipsum(1),
      ),
    ),
  );
  */

  $list_items = array(
    '1' => 'Parent',
    'sub1' => array(
      '1' => 'child 1',
      '2' => 'child 2',
      'sub1' => array(
        '1' => lipsum(1),
        '2' => 'child 2',
      ),
    ),
  );

  return theme_list($list_items, array('attributes' => array('class' => 'nav-menu')));

  /*
  $list_items = array();
  $list_items['test'] = lipsum(1);
  $list_items['test']['second'] = lipsum(1);
  */

  // return theme_list_test($list_items);
}

