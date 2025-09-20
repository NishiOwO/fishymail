#!/bin/sh
# $Id$

OLD="$PWD"
OBJS=""

svn rm --force src/lua
svn rm --force include/lua

mkdir -p src/lua include/lua

cd $1
for i in *.c; do
	if [ ! "$i" = "lua.c" ]; then
		OBJS="$OBJS `echo $i | sed 's/.c$/.o/g'`"
		cp $i $OLD/src/lua/
	fi
done
for i in *.h; do
	cp $i $OLD/include/lua/
done

cd "$OLD"

svn add src/lua
svn add include/lua

echo "# \$Id\$" > mk/lua.mk
echo "OBJS +=$OBJS" >> mk/lua.mk
echo "CFLAGS += -Iinclude/lua" >> mk/lua.mk
