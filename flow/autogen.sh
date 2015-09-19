#!/bin/sh

#gettextize --force --copy --intl --no-changelog
#autopoint --force
aclocal-1.15
autoheader
automake-1.15 --add-missing --gnu --copy
autoconf
