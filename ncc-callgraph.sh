#!/bin/sh

export CC="/usr/bin/nccgen -ncgcc -ncld -ncfabs"
export AR="/usr/bin/nccar"
export LD="/usr/bin/nccld"

./configure
make


genfull -s "vm lib" -g cncc -o nanovm-lib.graph

echo "generating call graph for vm/main() -> main-poster.ps ..."
gengraph -g nanovm-lib.graph -f main
poster -v -mA4 main.ps -s0.5 > main-poster.ps

echo "generating call graph for vm/exe_elist() -> exe_elist-poster.ps ..."
gengraph -g nanovm-lib.graph -f exe_elist
poster -v -mA4 -s0.5 exe_elist.ps > exe_elist-poster.ps


