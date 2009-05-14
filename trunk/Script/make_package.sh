#!/bin/sh

# Generate package name
svn up || exit 1
ver=`svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%Y%b%d`-r$ver"
rm -rf Build $pkg
rm -f Ell-*.tar.bz2

echo Generate $pkg
mkdir $pkg || exit 1
mkdir $pkg/lib
mkdir $pkg/include

cp -v --parent COPYING.LESSER $pkg/ || exit 1

make -j 5 MODE=Release || exit 1

# Run non-regression test-suite
for test in `find Build -name *_test`;
do
    $test || exit 1
done

for m in */; do
    if [ -e $m/Include ]; then
        cd $m/Include || exit 1
        cp --parent `find . -name "*.h"` ../../$pkg/include || exit 1
        cd ../..
    fi
done

cp -v `find Build -name "*.so" -o -name "*.a"` $pkg/lib || exit 1
for lib in $pkg/lib/*; do
    strip -s $lib
done

# Generate dev package
tar cjvf $pkg-dev.tar.bz2 $pkg
rm -rf $pkg

# Generate src package
svn export . $pkg || exit 1
tar cjvf $pkg-src.tar.bz2 $pkg
rm -rf $pkg
