#!/bin/sh

# Build and tests configurations
make MODE=Debug || exit 1
make MODE=Release || exit 1

for test in `find Build -name *_test`; do echo $test; $test || exit 1; done
# Generate package name
svn up || exit 1
ver=`LANG=C svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%F`-r$ver"

# Generate dev package
genDevPkg()
{
    echo Generate $pkg
    rm -rf $pkg
    mkdir -p $pkg/include || exit 1

    cp -v --parent COPYING.LESSER $pkg/ || exit 1

    for m in */; do
        if [ -e $m/Include ]; then
            cd $m/Include || exit 1
            cp --parent `find . -name "*.h"` ../../$pkg/include || exit 1
            cd ../..
        fi
    done

    rm -f   $pkg-dev.tar.bz2
    tar cjf $pkg-dev.tar.bz2 $pkg
    rm -rf $pkg
}

# Generate src package
genSrcPkg()
{
    svn export . $pkg || exit 1
    rm -f   $pkg-src.tar.bz2
    tar cjf $pkg-src.tar.bz2 $pkg
    rm -rf $pkg
}

genDevPkg
genSrcPkg
