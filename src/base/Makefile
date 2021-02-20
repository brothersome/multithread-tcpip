CC=gcc
CFLAGS=-O2 -Wall -W -Wshadow -D_DEF_NORMAL -I.
OS=Windows

OBJDIR=../../obj/base

objects = basemem.o \
	defaults.o \
	fieldpool.o \
	logging.o \
	memory.o \
	readconfig.o \
	scheduler.o \
	stringmanip.o \
	threading.o \
	timemanip.o
	
	
OBJS = $(patsubst %,$(OBJDIR)/%,$(objects))

$(OBJDIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
	
all: $(OBJS)

clean:
	rm $(OBJDIR)/*.o *~
