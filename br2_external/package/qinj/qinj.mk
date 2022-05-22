QINJ_DEPENDENCIES = libqinj qinj-console

include $(sort $(wildcard $(BR2_EXTERNAL_X6100_WSPR_PATH)/package/qinj/*/*.mk))
