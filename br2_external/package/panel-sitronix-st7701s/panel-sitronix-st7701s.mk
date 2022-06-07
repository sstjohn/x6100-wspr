################################################################################
#
#  panel-sitronix-st7701s
#
################################################################################

PANEL_SITRONIX_ST7701S_SITE = $(BR2_EXTERNAL_X6100_WSPR_PATH)/../st7701s
PANEL_SITRONIX_ST7701S_SITE_METHOD = local

$(eval $(kernel-module))
$(eval $(generic-package))
