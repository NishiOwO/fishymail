#!/bin/sh
# $Id$

OLD="$PWD"
OBJS=""

svn rm --force lua/src
svn rm --force lua/include

mkdir -p lua/src lua/include

cd $1
for i in *.c; do
	if [ ! "$i" = "lua.c" -a ! "$i" = "onelua.c" ]; then
		OBJS="$OBJS lua/src/`echo $i | sed 's/.c$/.o/g'`"
		cp $i $OLD/lua/src/
	fi
done
for i in *.h; do
	cp $i $OLD/lua/include/
done

cd "$OLD"

svn add lua/src
svn add lua/include

echo "# \$Id\$" > lua/Makefile
echo "OBJS +=$OBJS" >> lua/Makefile
echo "CFLAGS += -DLUA_USE_C89 -Ilua/include" >> lua/Makefile
