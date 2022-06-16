################################################################################
#
# python-tzlocal
#
################################################################################

PYTHON_TZLOCAL_VERSION = 4.2
PYTHON_TZLOCAL_SOURCE = tzlocal-$(PYTHON_TZLOCAL_VERSION).tar.gz
PYTHON_TZLOCAL_SITE = https://files.pythonhosted.org/packages/7d/b9/164d5f510e0547ae92280d0ca4a90407a15625901afbb9f57a19d9acd9eb
PYTHON_TZLOCAL_LICENSE = MIT
PYTHON_TZLOCAL_LICENSE_FILES = LICENSE.txt
PYTHON_TZLOCAL_SETUP_TYPE = setuptools

$(eval $(python-package))
