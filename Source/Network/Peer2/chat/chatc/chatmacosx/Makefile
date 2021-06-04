# Chat SDK Makefile
# Copyright 2000 GameSpy Industries

PROJECT=chatsdk

CC=cc
BASE_CFLAGS=-DLINUX

#use these cflags to optimize it
CFLAGS=$(BASE_CFLAGS)
#use these when debugging 
#CFLAGS=$(BASE_CFLAGS) -g

PROG_OBJS = \
	../../../nonport.o\
	../../../darray.o\
	../../../hashtable.o\
	../../chatCallbacks.o\
	../../chatCrypt.o\
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
	$(CC) -MM $(PROG_OBJS:.o=.c)

