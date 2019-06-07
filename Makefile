
CROSS_COMPILE = arm-none-linux-gnueabi-

# Make variables (CC, etc...)
CC 	= $(CROSS_COMPILE)gcc
LD 	= $(CROSS_COMPILE)ld
AS 	= $(CROSS_COMPILE)as
AR 	= $(CROSS_COMPILE)ar
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
export CC LD AS AR 
export STRIP OBJDUMP OBJCOPY 


CFLAGS := -Wall -O2 
CFLAGS += -I $(shell pwd)/include 
LDFLAGS := -lfreetype -lm -lts
export CFLAGS LDFLAGS



#recording top directory
TOPDIR := $(shell pwd)
export TOPDIR


# aim file
TARGET := e-book  


obj-y += main.o
obj-y += display/
obj-y += encoding/
obj-y += fonts/
obj-y += draw/
obj-y += input/


# -C chenge directory, -f designation make file
.PHONY:all
all: 
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC)  $(LDFLAGS) -o $(TARGET) built-in.o


.PHONY:clean
clean: 
	rm -rf $(TARGET)
	rm -rf $(shell find -name "*.o")

.PHONY:distclean
distclean:
	rm -rf $(shell find -name "*.o")
	rm -rf $(shell find -name "*.d")
	rm -rf $(TARGET)


.PHONY:cp
cp:
	cp e-book ../../rootfs/rootfs_3.16.57/app/ -f



