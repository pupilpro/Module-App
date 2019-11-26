LINUXKERNEL_INSTALL_DIR=/home/chen/share/kernel/linux-xlnx-xilinx-v2018.3
obj-m += ledDrv.o

EXTRA_CFLAGS +=	-DUSE_UDEV=$(USE_UDEV) 		\
				-DMAX_POOLS=$(MAX_POOLS)

MAKE_ENV = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

debug: EXTRA_CFLAGS += -D__DEBUG

.PHONY: clean all install default

default: all

docs:

all:
	make -C $(LINUXKERNEL_INSTALL_DIR) M=`pwd` $(MAKE_ENV) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules

clean:
	-rm -rf  *.o  *.ko  .tmp*  .cmd*  .symvers* *.mod* *.order *.symvers .*.cmd
