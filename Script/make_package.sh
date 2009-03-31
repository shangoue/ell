#!/bin/sh

if [ ! "$*" ]; then
    echo Please, give a list of modules you want to package
    exit 1
fi

# Generate package name
svn up
ver=`svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%Y%b%d`-r$ver"
rm -rf GNU_Linux $pkg
rm -f Ell-*-.tar.bz2

echo Generate $pkg
mkdir $pkg
mkdir $pkg/lib
mkdir $pkg/include

# Get dev package content
cp --parent COPYING.LESSER $pkg/
MODE=Release COMPILER=icc make -C libELL
for m in $*; do
    MODE=Release COMPILER=icc make -C $m
    cp GNU_Linux/x86_64/Release/lib`basename $m`.a $pkg/lib
done

for m in libELL $*; do
    cd $m/Include
    cp --parent `find . -name "*.h"` ../../$pkg/include
    cd ../..
done

# Get doc
pushd ../wiki
svn up
popd
cp ../wiki/ReferenceManual.wiki $pkg/

# Generate dev package
tar cjvf $pkg-dev.tar.bz2 $pkg
rm -rf $pkg
