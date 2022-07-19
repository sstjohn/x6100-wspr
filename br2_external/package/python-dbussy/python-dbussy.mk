################################################################################
#
# python-dbussy
#
################################################################################

PYTHON_DBUSSY_VERSION = 2e61c386a0b43cf3e78f1ae4df2a61f502324821
PYTHON_DBUSSY_SITE_METHOD = git
PYTHON_DBUSSY_SITE = https://github.com/ldo/dbussy.git
PYTHON_DBUSSY_LICENSE = LGPL-2.1
PYTHON_DBUSSY_LICENSE_FILES = COPYING
PYTHON_DBUSSY_SETUP_TYPE = setuptools

$(eval $(python-package))
