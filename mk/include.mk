# $Id$

include config.mk

# format and propset files
SRCS = `find include src -name "*.c" \
	! -name "ui.tab.c" \
	! -name "ui.yy.c" \
	! -name "ui.c"`
SRCS_HEADERS = `find include src -name "*.h" \
	! -name "ui.tab.h" \
	! -name "stb_ds.h"`
PERL_SRCS = `find tools -name "*.pl"`

P_CC ?= $(CROSS)gcc
P_RC ?= $(CROSS)windres

CFLAGS = $(C_CFLAGS) $(U_CFLAGS) $(P_CFLAGS) -I include -Wall -Wextra -Wno-unused-function -Wno-deprecated-declarations
LDFLAGS = $(C_LDFLAGS) $(U_LDFLAGS) $(P_LDFLAGS)
LIBS = $(C_LIBS) $(U_LIBS) $(P_LIBS)

OBJS = $(P_OBJS)
OBJS += src/main.o src/ui.o src/ui.yy.o src/ui.tab.o src/stb_ds.o src/util.o src/layout.o src/debug.o src/version.o src/socket.o src/dns.o src/imap.o src/smtp.o src/m2ini.o

include lua/Makefile

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

src/fishymail.res: src/fishymail.rc src/images/fishymail.ico

format:
	clang-format --verbose -i $(SRCS) $(SRCS_HEADERS)
	perltidy -b -bext='/' --paren-tightness=2 $(PERL_SRCS)

propset:
	svn propset svn:keywords Id $(SRCS) $(SRCS_HEADERS)

clean:
	rm -f *.exe src/*.o src/*.res src/*/*.o lua/src/*.o fishymail
