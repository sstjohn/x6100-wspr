WSJTX_SITE = git://git.code.sf.net/p/wsjt/wsjtx
WSJTX_SITE_METHOD = git
WSJTX_VERSION = wsjtx-2.5.4
WSJTX_DEPENDS = libfftw3f hamlib-for-wsjtx boost
WSJTX_INSTALL_STAGING = no
WSJTX_INSTALL_TARGET = yes
WSJTX_INSTALL_TARGET_OPTS += wsprd
WSJTX_MAKE_OPTS += wsprd

$(eval $(cmake-package))
