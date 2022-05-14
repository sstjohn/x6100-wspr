GAMMARAY_VERSION = 5658516fc0c13c0d26797f690fb096872b8ec66c
GAMMARAY_SITE = https://github.com/KDAB/GammaRay.git
GAMMARAY_SITE_METHOD = git
GAMMARAY_INSTALL_STAGING = NO
GAMMARAY_INSTALL_TARGET = YES
GAMMARAY_LICENSE = GPLv2
GAMMARAY_LICENSE_FILES = LICENSE
GAMMARAY_DEPENDENCIES = qt5base qt5charts qt5connectivity qt5location qt5multimedia qt5quickcontrols qt5quickcontrols2 qt5scxml qt5sensors qt5serialbus qt5tools qt5virtualkeyboard qt5websockets

GAMMARAY_CONF_OPTS += -DGAMMARAY_BUILD_UI=OFF

$(eval $(cmake-package))
