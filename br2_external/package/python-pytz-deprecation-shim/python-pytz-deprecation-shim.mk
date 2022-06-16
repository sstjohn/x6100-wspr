################################################################################
#
# python-pytz-deprecation-shim
#
################################################################################

PYTHON_PYTZ_DEPRECATION_SHIM_VERSION = 0.1.0.post0
PYTHON_PYTZ_DEPRECATION_SHIM_SOURCE = pytz_deprecation_shim-$(PYTHON_PYTZ_DEPRECATION_SHIM_VERSION).tar.gz
PYTHON_PYTZ_DEPRECATION_SHIM_SITE = https://files.pythonhosted.org/packages/94/f0/909f94fea74759654390a3e1a9e4e185b6cd9aa810e533e3586f39da3097
PYTHON_PYTZ_DEPRECATION_SHIM_LICENSE = Apache-2.0
PYTHON_PYTZ_DEPRECATION_SHIM_LICENSE_FILES = LICENSE
PYTHON_PYTZ_DEPRECATION_SHIM_SETUP_TYPE = setuptools

define PYTHON_PYTZ_DEPRECATION_SHIM_ADD_SETUP_PY
	echo "import setuptools; setuptools.setup()" > $(@D)/setup.py
endef

PYTHON_PYTZ_DEPRECATION_SHIM_PRE_BUILD_HOOKS += PYTHON_PYTZ_DEPRECATION_SHIM_ADD_SETUP_PY

$(eval $(python-package))
