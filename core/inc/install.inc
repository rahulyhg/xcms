<?php

/**
 * @file
 */

/**
 * Xcms_install()
 * $options for automated installation.
 */
function xcms_install($conf_file, $options = array()) {
  // Merge defaults
  // TODO, could possibly pull these defaults in from conf.json.example.
  $options += array(
    'base_path' => rtrim(dirname($_SERVER['PHP_SELF']), '/') . '/',
    'site_name' => 'XCMS',
    'current_theme' => 'default',
  );

  if (is_writable($conf_file)) {
    // Need FAPI here to prompt for defaults for install.
  }
  else {

    // Prompt for example conf file to be copied writable.
  }

  // Do something useful.
  return $options;
}
