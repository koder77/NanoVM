#!/bin/sh

# gettextize --force --copy --no-changelog
# autopoint --force
aclocal-1.9 --acdir=/usr/share/aclocal-1.9
autoheader
automake-1.9 --add-missing --gnu --copy
autoconf
