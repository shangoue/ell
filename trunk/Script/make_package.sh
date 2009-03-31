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


# Get dev package content
cp --parent COPYING.LESSER $pkg/
MODE=Release COMPILER=icc make -C libELL
for m in $*; do
    MODE=Release COMPILER=icc make -C $m
    cp --parent GNU_Linux/x86_64/Release/lib$m.a $pkg
done

for m in libELL $*; do
    cp --parent `find $m/Include -name "*.h"` $pkg/
done

# Get doc
pushd ../wiki
svn up
popd
cp ../wiki/ReferenceManual.wiki $pkg/

# Generate dev package
tar cjvf $pkg-dev.tar.bz2 $pkg

# Remove binaries
rm -rf $pkg/GNU_Linux

# Add source package content
cp --parent CommonHeaders.h COPYING.LESSER Makefile $pkg/
cp --parent `find Script -name "*.mk"` $pkg/

for m in libELL $*; do
    cp --parent $m/Makefile $pkg/
    for dir in Source Test; do
        [ -d $m/$dir ] && cp --parent `find $m/$dir -name "*.cpp" -o -name "*.h"` $pkg/
    done
done

# Generate source package
tar cjvf $pkg.tar.bz2 $pkg

rm -rf $pkg
