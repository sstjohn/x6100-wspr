################################################################################
#
#  python3-apscheduler
#
################################################################################

PYTHON3_APSCHEDULER_VERSION = 3.6.3
PYTHON3_APSCHEDULER_SITE = https://files.pythonhosted.org/packages/89/3d/f65972547c5aa533276ada2bea3c2ef51bb4c4de55b67a66129c111b89ad
PYTHON3_APSCHEDULER_SOURCE = APScheduler-$(PYTHON3_APSCHEDULER_VERSION).tar.gz
PYTHON3_APSCHEDULER_LICENSE = MIT
PYTHON3_APSCHEDULER_LICENSE_FILES = LICENSE.txt
PYTHON3_APSCHEDULER_SETUP_TYPE = setuptools
PYTHON3_APSCHEDULER_DEPENDENCIES = python3 python-pip python-setuptools

$(eval $(python-package))
