#!/bin/sh

# Generate package name
svn up || exit 1
ver=`LANG=C svnversion | sed "s/^[^:]*:\([0-9]\+\)/\1/" | sed "s/\([0-9]\+\)M\?/\1/"`

pkg="Ell-`date +%Y%b%d`-r$ver"

echo Generate $pkg
rm -rf $pkg
mkdir -p $pkg/include || exit 1
#mkdir $pkg/lib/release
#mkdir $pkg/lib/debug

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

# Notice: No more libraries to copy
#
#rlibs=`find Build -type d -name Release`
#cp -v `find $rlibs -name "*.so" -o -name "*.a"` $pkg/lib/release || exit 1
#rlibs=`find Build -type d -name Debug`
#cp -v `find $rlibs -name "*.so" -o -name "*.a"` $pkg/lib/debug || exit 1
#for lib in $pkg/lib/release/*; do
#    strip -s $lib
#done


cat > $pkg/readme.txt << END
Coming from:
http://ell.google.com/svn/trunk
Complete documentation can be found at:
http://code.google.com/p/ell/wiki/ReferenceManualEn
To compile with the release library, please define ELL_DEBUG=0 or NDEBUG
To compile with the debug library, please define ELL_DEBUG=1 or undefine NDEBUG
Thanks for using libELL!
END


# Generate dev package
rm -f   $pkg-dev.tar.bz2
tar cjf $pkg-dev.tar.bz2 $pkg
rm -rf $pkg

# Generate src package
svn export . $pkg || exit 1
rm -f   $pkg-src.tar.bz2
tar cjf $pkg-src.tar.bz2 $pkg
rm -rf $pkg
