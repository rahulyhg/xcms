<html>
  <head>
    <title><?php print $head_title; ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link rel="shortcut icon" href="<?php print $directory; ?>/favicon.ico" type="image/vnd.microsoft.icon" />
    <link rel="stylesheet" type="text/css" href="<?php print $directory; ?>/base.css" />
    <link rel="stylesheet" type="text/css" href="<?php print $directory; ?>/style.css" />
    <meta name="Generator" content="XCMS (https://www.xweb.com.au)" />
  </head>
  <body>

  <div id="header-wrapper">
    <div id="header" class="container clearfix">
      <div id="logo">
        <a href="<?php print $front_url; ?>" title="Home" rel="home"><img src="<?php print $directory; ?>/logo.png" alt="Home" /></a>
      </div>
      <a id="mobile-nav">Menu</a>
    </div>
  </div>

  <div id="nav-wrapper">
    <div id="nav" class="container clearfix">
    </div>
  </div>

  <div id="main-wrapper">
    <div id="main" class="container clearfix">
    <?php print $messages; ?>
    <?php if ($title): ?><h1><?php print $title; ?></h1><?php endif; ?>
    <?php print $content; ?>
    </div>
  </div>

    <div id="footer-wrapper">
      <div id="footer" class="container clearfix">
        Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.
      </div>
    </div>

    <div id="copyright-wrapper">
      <div id="copyright" class="container clearfix">
        © Company Name <?php print date('Y'); ?>. All Rights Reserved. <span>Site by <a href="https://www.xweb.com.au" target="_blank">XWeb</a></span>
      </div>
    </div>

  </body>
</html>
