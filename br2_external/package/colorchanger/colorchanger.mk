################################################################################
#
#  colorchanger 
#
################################################################################

COLORCHANGER_SITE = $(BR2_EXTERNAL_X6100_WSPR_PATH)/../colorchanger
COLORCHANGER_SITE_METHOD = local
COLORCHANGER_VERSION = 1.0.0
COLORCHANGER_DEPENDENCIES = qt5base

define COLORCHANGER_CONFIGURE_CMDS
	cd $(@D); $(TARGET_MAKE_ENV) $(QT5_QMAKE) $(COLORCHANGER_CONF_OPTS)
endef
define COLORCHANGER_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef
define COLORCHANGER_INSTALL_STAGING_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install
endef
define COLORCHANGER_INSTALL_TARGET_CMDS
	cp -a $(@D)/*.so.* $(TARGET_DIR)/usr/lib
endef

$(eval $(generic-package))
