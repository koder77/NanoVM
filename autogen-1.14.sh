#!/bin/sh

#gettextize --force --copy --intl --no-changelog
#autopoint --force
aclocal-1.14 -I m4
autoheader
automake-1.14 --add-missing --gnu --copy
autoconf