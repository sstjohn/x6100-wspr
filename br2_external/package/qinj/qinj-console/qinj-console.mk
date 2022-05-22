QINJ_CONSOLE_SITE = $(BR2_EXTERNAL_X6100_WSPR_PATH)/../qinj
QINJ_CONSOLE_SITE_METHOD = local
QINJ_CONSOLE_VERSION = 1.0.0
QINJ_CONSOLE_DEPENDENCIES = qt5base qt5charts qt5connectivity qt5location qt5multimedia qt5quickcontrols qt5quickcontrols2 qt5scxml qt5sensors qt5serialbus qt5tools qt5virtualkeyboard qt5websockets python3
QINJ_CONSOLE_SETUP_TYPE = setuptools


$(eval $(python-package))
