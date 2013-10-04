(function ($, Drupal, window, document, undefined) {
	Drupal.behaviors.redirect_to_donate_page = {
			attach: function(context) { 

			$('input[name=identity_type]').change( function() { 
        			$identity_type = $(this).val();
				if($identity_type == 'Domain') {
					$('.identity-domain-class').show();
					$('.identity-email-class').hide();
				}
				if($identity_type == 'EmailAddress') {
					$('.identity-email-class').show();
					$('.identity-domain-class').hide();
				}
		 
			}); 

		}
	}	

}
)(jQuery, Drupal, this, this.document); 




