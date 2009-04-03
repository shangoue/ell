#!/bin/sh

# Generate package name
svn up || exit 1
ver=`svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%Y%b%d`-r$ver"
rm -rf GNU_Linux $pkg
rm -f Ell-*.tar.bz2

echo Generate $pkg
mkdir $pkg || exit 1
mkdir $pkg/lib
mkdir $pkg/include

cp --parent COPYING.LESSER $pkg/ || exit 1

make MODE=Release NOPCH=1 || exit 1

for m in */; do
    if [ -e $m/Include ]; then
        cd $m/Include || exit 1
        cp --parent `find . -name "*.h"` ../../$pkg/include || exit 1
        cd ../..
    fi
done

cp `find GNU_Linux -name "*.so"` $pkg/lib || exit 1

# Generate dev package
tar cjvf $pkg-dev.tar.bz2 $pkg
rm -rf $pkg

# Generate src package
svn export . $pkg || exit 1
tar cjvf $pkg-src.tar.bz2 $pkg
rm -rf $pkg
