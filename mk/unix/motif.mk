# $Id$
# Makefile for Unix with Motif.

P_LIBS += -lXm -lXt -lX11 -lXpm

P_OBJS += src/ui/motif.o

include mk/include.mk
