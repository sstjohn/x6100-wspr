HAMLIB_SITE = https://github.com/Hamlib/Hamlib.git
HAMLIB_VERSION = origin/master
HAMLIB_SITE_METHOD = git
HAMLIB_AUTORECONF = YES
HAMLIB_DEPENDS = python3 host-python3
HAMLIB_CONF_OPTS += \
  --with-python-binding \
  PYTHON_CPPFLAGS="-I$(STAGING_DIR)/usr/include/python$(PYTHON3_VERSION_MAJOR)" \
  PYTHON_LIBS="-L$(STAGING_DIR)/usr/lib -lpython$(PYTHON3_VERSION_MAJOR)"

$(eval $(autotools-package))
