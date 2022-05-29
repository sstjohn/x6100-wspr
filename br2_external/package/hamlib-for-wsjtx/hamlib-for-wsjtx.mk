################################################################################
#
#  hamlib-for-wsjtx
#
################################################################################

HAMLIB_FOR_WSJTX_SITE = git://git.code.sf.net/u/bsomervi/hamlib
HAMLIB_FOR_WSJTX_VERSION = origin/integration
HAMLIB_FOR_WSJTX_SITE_METHOD = git
HAMLIB_FOR_WSJTX_AUTORECONF = YES
HAMLIB_FOR_WSJTX_INSTALL_STAGING = YES
HAMLIB_FOR_WSJTX_INSTALL_TARGET = NO
HAMLIB_FOR_WSJTX_CONF_OPTS = --disable-shared --enable-static --without-cxx-binding --disable-winradio CFLAGS=-"g -O2 -fdata-sections -ffunction-sections" LDFLAGS=-Wl,--gc-sections

$(eval $(autotools-package))
