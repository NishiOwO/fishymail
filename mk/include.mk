# $Id$

SRCS = `find include src "(" -name "*.c" -or -name "*.h" ")" -and -not -name "stb_ds.h"`

P_CC ?= $(CROSS)gcc
P_RC ?= $(CROSS)windres

CFLAGS = $(P_CFLAGS) -I include
LDFLAGS = $(P_LDFLAGS)
LIBS = $(P_LIBS)

OBJS = $(P_OBJS)
OBJS += src/main.o

.PHONY: all clean format propset
.SUFFIXES: .c .o .rc .res

all: fishymail$(EXEC)

fishymail$(EXEC): $(OBJS)
	$(P_CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	$(AFTERCMD)

.c.o:
	$(P_CC) $(CFLAGS) -c -o $@ $<

.rc.res:
	$(P_RC) -I include -O coff $< $@

src/fishymail.res: src/fishymail.rc src/gui.rc src/images/fishymail.ico

format:
	clang-format --verbose -i $(SRCS)

propset:
	svn propset svn:keywords Id $(SRCS)

clean:
	rm -f *.exe src/*.o src/*.res src/*/*.o fishymail
