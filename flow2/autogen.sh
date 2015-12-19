#!/bin/sh

#gettextize --force --intl --copy --no-changelog
#autopoint --force
aclocal-1.15 -I m4
autoheader
automake-1.15 --add-missing --gnu --copy
autoconf
