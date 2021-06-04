# Chat SDK Makefile
# Copyright 2000 GameSpy Industries

PROJECT=chatsdk

CC=gcc
BASE_CFLAGS=-DLINUX

#use these cflags to optimize it
CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2
#use these when debugging 
#CFLAGS=$(BASE_CFLAGS) -g

PROG_OBJS = \
	../../../nonport.o\
	../../../darray.o\
	../../../hashtable.o\
	../../chatCrypt.o\
	../../chatCallbacks.o\
	../../chatChannel.o\
	../../chatHandlers.o\
	../../chatMain.o\
	../../chatSocket.o\
	../chatc.o


#############################################################################
# SETUP AND BUILD
#############################################################################

$(PROJECT): $(PROG_OBJS)        
	$(CC) $(CFLAGS) -o $@ $(PROG_OBJS)

#############################################################################
# MISC
#############################################################################

clean:	
	rm -f $(PROG_OBJS)

depend:
	gcc -MM $(PROG_OBJS:.o=.c)

