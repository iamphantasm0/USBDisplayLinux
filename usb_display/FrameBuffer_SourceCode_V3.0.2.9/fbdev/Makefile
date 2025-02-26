
ifneq ($(KERNELRELEASE),)
include Kbuild
else



default: module

module:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	rm -rf *.mod *.mod.c *.o *.ko .tmp* .*.o.d .*.*.cmd Module.symvers modules.order

endif