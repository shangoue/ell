#!/bin/sh

# Generate package name
svn up || exit 1
ver=`svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%Y%b%d`-r$ver"

echo Generate $pkg
rm -rf $pkg
mkdir $pkg || exit 1
mkdir $pkg/lib
mkdir $pkg/include

cp -v --parent COPYING.LESSER $pkg/ || exit 1

# Build all configurations
make MODE=Debug || exit 1
make MODE=Release || exit 1

# Run non-regression test-suite for both modes
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

ReleaseBuild=`find Build -type d -name Release`
cp -v `find $ReleaseBuild -name "*.so" -o -name "*.a"` $pkg/lib || exit 1
for lib in $pkg/lib/*; do
    strip -s $lib
done

# Generate dev package
rm -f   $pkg-dev.tar.bz2
tar cjf $pkg-dev.tar.bz2 $pkg
rm -rf $pkg

# Generate src package
svn export . $pkg || exit 1
rm -f   $pkg-src.tar.bz2
tar cjf $pkg-src.tar.bz2 $pkg
rm -rf $pkg
