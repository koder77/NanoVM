#!/bin/sh

#gettextize --force --copy --intl --no-changelog
#autopoint --force
aclocal-1.11 -I m4 --acdir=/usr/share/aclocal-1.11
autoheader
automake-1.11 --add-missing --gnu --copy
autoconf
