################################################################################
#
#  libqinj
#
################################################################################

LIBQINJ_SITE = $(BR2_EXTERNAL_X6100_WSPR_PATH)/../qinj
LIBQINJ_SITE_METHOD = local
LIBQINJ_VERSION = 1.0.0
LIBQINJ_DEPENDENCIES = qt5base qt5charts qt5connectivity qt5location qt5multimedia qt5quickcontrols qt5quickcontrols2 qt5scxml qt5sensors qt5serialbus qt5tools qt5virtualkeyboard qt5websockets python3

define LIBQINJ_CONFIGURE_CMDS
	cd $(@D); $(TARGET_MAKE_ENV) $(QT5_QMAKE) $(LIBQINJ_CONF_OPTS)
endef
define LIBQINJ_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef
define LIBQINJ_INSTALL_STAGING_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install
endef
define LIBQINJ_INSTALL_TARGET_CMDS
	cp -a $(@D)/*.so.* $(TARGET_DIR)/usr/lib
endef

$(eval $(generic-package))
