################################################################################
#
#  wsjt-x
#
################################################################################

WSJTX_SITE = https://github.com/saitohirga/WSJT-X.git
WSJTX_SITE_METHOD = git
WSJTX_VERSION = wsjtx-2.5.4
WSJTX_DEPENDS = fftw-single hamlib-for-wsjtx boost
WSJTX_MAKE_OPTS += wsprd wsprsimwav

define WSJTX_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/lib/wsprd all
endef

define WSJTX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/lib/wsprd/wsprd $(TARGET_DIR)/usr/bin/wsprd
	$(INSTALL) -D -m 0755 $(@D)/lib/wsprd/wsprsimwav $(TARGET_DIR)/usr/bin/wsprsimwav
endef

$(eval $(generic-package))
