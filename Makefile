# Makefile for "hidraw-dump"
# 2021 - @todbot / Tod Kurt
# https://github.com/todbot/hidraw-dump
#


LIBS=`pkg-config libudev --libs`

CFLAGS = 
OBJS += hidraw-dump.o

#INSTALL = install -D
#PREFIX ?= /usr/local
#EXELOCATION ?= $(PREFIX)/bin

hidraw-dump: 
	$(CC) $(CFLAGS) hidraw-dump.c -o hidraw-dump $(LIBS)

#install: hidraw-dump
#	$(INSTALL) hidraw-dump $(DESTDIR)$(EXELOCATION)/hidraw-dump

clean:
	rm -f $(OBJS)
	rm -f hidraw-dump
