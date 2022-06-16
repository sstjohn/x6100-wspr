################################################################################
#
# python-backports-zoneinfo
#
################################################################################

PYTHON_BACKPORTS_ZONEINFO_VERSION = 0.2.1
PYTHON_BACKPORTS_ZONEINFO_SOURCE = backports.zoneinfo-$(PYTHON_BACKPORTS_ZONEINFO_VERSION).tar.gz
PYTHON_BACKPORTS_ZONEINFO_SITE = https://files.pythonhosted.org/packages/ad/85/475e514c3140937cf435954f78dedea1861aeab7662d11de232bdaa90655
PYTHON_BACKPORTS_ZONEINFO_LICENSE = Apache-2.0
PYTHON_BACKPORTS_ZONEINFO_LICENSE_FILES = LICENSE
PYTHON_BACKPORTS_ZONEINFO_SETUP_TYPE = setuptools

$(eval $(python-package))
