$(document).ready(function() {
  var progressbar = $('#progress_bar');
  var rainbow = new Rainbow();
  var value = Math.floor(Math.random() * 1024); // Generate a random value between 0 and 1023
  rainbow.setSpectrum('#8A360F', '#DFFF00', 'green');

  var loading = function() {
    // Simulate the AJAX call and use the random value
    var decimal = (1023 - value) / 1023;
    var percent = decimal * 100;
    progressbar.val(percent);

    $('.progress-value').html(decimal * 100 + '%');
    var $ppc = $('.progress-pie-chart'),
      deg = 360 * decimal;
    if (percent > 50) {
      $ppc.addClass('gt-50');
    } else {
      $ppc.removeClass('gt-50');
    }

    $('.ppc-progress-fill').css('transform', 'rotate(' + deg + 'deg)');
    // Set the value into the background color using rainbowvis
    $('.ppc-percents').css("background-color", '#' + rainbow.colorAt(percent));
  };

  loading();
});
