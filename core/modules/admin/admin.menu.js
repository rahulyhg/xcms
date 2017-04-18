/**
 * @file
 */
window.addEventListener('DOMContentLoaded', function() {
  document.getElementById('admin-menu-tab').addEventListener('click', function() {
    document.getElementById('admin-menu').classList.toggle('open');
    event.preventDefault();
  });
});
