(function($) {
  $(document).ready(function() {

    // contact webform client side validation
    $('.webform-client-form').validate();

    // mobile nav toggle
    $('#mobile-nav').click(function() {
      // $('#nav').slideToggle();
      $('#nav').toggleClass('open');
    });
  });
})(jQuery);
