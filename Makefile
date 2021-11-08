# Makefile for "hidraw-dump"
# 2021 - @todbot / Tod Kurt
# https://github.com/todbot/hidraw-dump
#

LIBS=`pkg-config libudev --libs`

CFLAGS = 
OBJS += hidraw-dump.o


hidraw-dump: 
	@echo "hello"
	$(CC) $(CFLAGS) hidraw-dump.c -o hidraw-dump $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f hidraw-dump
