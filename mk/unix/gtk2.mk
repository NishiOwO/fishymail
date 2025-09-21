# $Id$
# Makefile for Unix with GTK2.

P_CFLAGS += `pkg-config --cflags gtk+-2.0`
P_LIBS += `pkg-config --libs gtk+-2.0`

P_OBJS += src/ui/gtk2.o

include mk/include.mk
