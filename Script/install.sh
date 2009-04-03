#!/bin/bash
./Script/make_package.sh
rm Ell-*-src.tar.bz2
rm ~/Ell* -rf
tar xjvf Ell-*-dev.tar.bz2 -C ~/
mv ~/Ell-* ~/Ell
