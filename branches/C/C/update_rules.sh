#!/bin/sh
cat CGrammar.def | sed "s/^ *\(\w\+\) * =.*$/D(\1)/" | grep "^ *D(" | grep -v "root" > CRules.def
