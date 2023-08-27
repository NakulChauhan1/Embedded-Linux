obj-m := hello_world.o

KVERSION = $(uname -r)
all:
	#  make -C <path_to_kernel_src> M=$PWD modules
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) modules
clean:
	#  make -C <path_to_kernel_src> M=$PWD modules
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) clean

# Downloading Kernel Source: sudo apt-get install linux-headers-$(uname -r)