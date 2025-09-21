# $Id$
# Makefile for Unix.

UNIX_UI = motif

P_LIBS += -lpthread -lresolv -lm

include mk/unix/$(UNIX_UI).mk
