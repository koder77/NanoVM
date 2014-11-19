#!/bin/sh

#gettextize --force --intl --copy --no-changelog
#autopoint --force
aclocal-1.9 -I m4 --acdir=/usr/share/aclocal-1.9
autoheader
automake-1.9 --add-missing --gnu --copy
autoconf
