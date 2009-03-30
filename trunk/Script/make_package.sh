#!/bin/sh

if [ ! "$*" ]; then
    echo Please, give a list of modules you want to package
    exit 1
fi

ver=`svnversion | sed "s/^[^:]*:\([0-9M]\+\)/\1/"`
pkg="Ell-`date +%Y%b%d`-r$ver"
echo Generate $pkg
mkdir $pkg
cp --parent CommonHeaders.h COPYING.LESSER Makefile $pkg/
cp --parent `find Script -name "*.mk"` $pkg/
for m in $*; do
    cp --parent `find $m/Include -name "*.h"` $pkg/
    cp --parent `find $m/Source $m/Test -name "*.cpp"` $pkg/
    cp --parent $m/Doc/* $pkg/
    cp --parent $m/Makefile $pkg/
done

tar cjvf $pkg.tar.bz2 $pkg
rm -rf $pkg
