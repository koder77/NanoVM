#!/bin/sh

#gettextize --force --copy --intl --no-changelog
#autopoint --force
aclocal-1.7 -I m4 --acdir=/usr/share/aclocal-1.7
autoheader
automake-1.7 --add-missing --gnu --copy
autoconf
