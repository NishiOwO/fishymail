# $Id$
# Makefile for Windows.

P_CFLAGS += -mwindows
P_LDFLAGS += -mwindows
P_LIBS += -lgdi32 -lws2_32 -lcomctl32 -ldnsapi

P_OBJS += src/ui/win32.o src/fishymail.res

EXEC = .exe

include mk/include.mk
