CC=gcc
CFLAGS=-O2 -Wall -W -Wshadow -D_DEF_NORMAL -I.
OS=Windows

OBJDIR=obj

OUTPUTDIR=out
SRCDIR=src
BUILDDIR=build

BASEDIR=base


objects = main.o \
	initialize.o
	
OBJS = $(patsubst %,$(OBJDIR)/%,$(objects))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

subsystem:
	$(MAKE) -C src/base

main: $(OBJS) $(OBJDIR)/$(BASEDIR)/*.o
	$(CC) -o $(OUTPUTDIR)/$@ $^

all: $(OBJS) $(OBJDIR)/$(BASEDIR)/*.o
	$(CC) -o $(OUTPUTDIR)/$@ $^ $(CFLAGS)

Debug: $(OBJS) $(OBJDIR)/$(BASEDIR)/*.o
	$(CC) -o $(OUTPUTDIR)/$@ $^ $(CFLAGS)

Release: $(OBJS) $(OBJDIR)/$(BASEDIR)/*.o
	$(CC) -o $(OUTPUTDIR)/$@ $^ $(CFLAGS)

.PHONY: clean

cleanDebug: clean

cleanRelease: clean

clean:
	rm $(OBJDIR)/*.o *~
	rm -r $(BUILDDIR)
	rm $(OUTPUTDIR)/*.*

build:
	mkdir $(BUILDDIR)
	cp $(OUTPUTDIR)/*.exe $(BUILDIR)


