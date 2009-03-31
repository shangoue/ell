#!/bin/sh

if [ ! "$*" ]; then
    echo Please, give a list of modules you want to package
    exit 1
fi

svn up
ver=`svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%Y%b%d`-r$ver"
echo Generate $pkg
mkdir $pkg

cp --parent CommonHeaders.h COPYING.LESSER Makefile $pkg/
cp --parent `find Script -name "*.mk"` $pkg/
for m in libELL $*; do
    cp --parent $m/Makefile $pkg/
    for dir in Source Test Include; do
        [ -d $m/$dir ] && cp --parent `find $m/$dir -name "*.cpp" -o -name "*.h"` $pkg/
    done
done

pushd ../wiki
svn up
popd
cp ../wiki/ReferenceManual.wiki $pkg/

tar cjvf $pkg.tar.bz2 $pkg
rm -rf $pkg
