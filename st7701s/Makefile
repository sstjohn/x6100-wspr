ifneq ($(KERNELRELEASE),)

obj-m	:= panel-sitronix-st7701s.o 

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

endif
