# $Id$
# Makefile for Linux with musl libc.
# TODO: Check this working

P_CFLAGS += -DUSE_UNSAFE_DNS
P_LDFLAGS +=

include ostype/unix.mk
