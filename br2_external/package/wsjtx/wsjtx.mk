WSJTX_SITE = https://github.com/saitohirga/WSJT-X.git
WSJTX_SITE_METHOD = git
WSJTX_VERSION = wsjtx-2.5.4
WSJTX_DEPENDS = fftw-single hamlib-for-wsjtx boost
WSJTX_MAKE_OPTS += wsprd

define WSJTX_INSTALL_TARGET_CMDS
  $(INSTALL) -D -m 0755 $(@D)/wsprd $(TARGET_DIR)/usr/bin/wsprd
endef


$(eval $(cmake-package))
