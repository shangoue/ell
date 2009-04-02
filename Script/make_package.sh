#!/bin/sh

# Generate package name
svn up
ver=`svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%Y%b%d`-r$ver"
rm -rf GNU_Linux $pkg
rm -f Ell-*.tar.bz2

echo Generate $pkg
mkdir $pkg
mkdir $pkg/lib
mkdir $pkg/include

cp --parent COPYING.LESSER $pkg/

make MODE=Release NOPCH=1 

for m in */; do
    if [ -e $m/Include ]; then
        cd $m/Include
        cp --parent `find . -name "*.h"` ../../$pkg/include
        cd ../..
    fi
done

cp `find GNU_Linux -name "*.so"` $pkg/lib

# Generate dev package
tar cjvf $pkg-dev.tar.bz2 $pkg
rm -rf $pkg

# Generate src package
svn export . $pkg
tar cjvf $pkg-src.tar.bz2 $pkg
