<?php

/**
 * Common functions that need to be referenced.
 */

/**
 * Encode special characters in plain-text.
 */
function check_plain($text) {
  return htmlspecialchars($text, ENT_QUOTES, 'UTF-8');
}

/**
 * Create a css class safe string.
 */
function strip_class($text) {

  // lower case everything
  $text = strtolower($text);

  // make alphanumeric (removes all other characters)
  $text = preg_replace('/[^a-z0-9_\s-]/', '', $text);

  // clean up multiple dashes or whitespaces
  $text = preg_replace('/[\s-]+/', ' ', $text);

  // convert whitespaces and underscore to dash
  $text = preg_replace('/[\s_]/', '-', $text);
  return $text;
}

