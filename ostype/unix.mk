# $Id$
# Makefile for Unix.

P_LIBS += -lXm -lXt -lX11 -lXpm -lpthread -lresolv

P_OBJS += src/ui/motif.o

include mk/include.mk
