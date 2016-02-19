<?php

/**
 * @file
 */

/**
 * xcms_install()
 * $options for automated installation
 */
function xcms_install($conf_file, $options) {
  // merge defaults
  // TODO, could possibly pull these defaults in from conf.json.example
  $options += array(
    'base_path' => rtrim(dirname($_SERVER['PHP_SELF']), '/') . '/',
    'site_name' => 'XCMS',
    'current_theme' => 'default',
  );

  if (is_writable($conf_file)) {
    // need FAPI here to prompt for defaults for install
  }
  else {

    // prompt for example conf file to be copied writable
  }

  // do something useful
  return $options;
}

