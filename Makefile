

CROSS_COMPLIE := arm-none-linux-gnueabi-

CFLAGS := -Wall -O2 -c
CFLAGS += -I$(PWD)/include

LDFLAGS := -lm -lfreetype


CC := $(CROSS_COMPLIE)gcc
LD := $(CROSS_COMPLIE)ld


OBJS += main.o
OBJS += display/fb.o
OBJS += encoding/ansi.o 
OBJS += encoding/encoding_manage.o
OBJS += encoding/utf-16le.o
OBJS += encoding/utf-16be.o
OBJS += encoding/utf-8.o
OBJS += fonts/ascii.o
OBJS += fonts/fonts_manage.o
OBJS += fonts/freetype.o
OBJS += fonts/gbk.o

all: $(OBJS)
	$(CC)  $(LDFLAGS) -o e-book $^

%o:%c
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	rm -rf e-book
	rm -rf $(OBJS)





