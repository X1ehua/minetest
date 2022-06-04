#!/bin/sh

files=(
    'lapi.c'
    'lauxlib.c'
    'lbaselib.c'
    'lcode.c'
    'ldblib.c'
    'ldebug.c'
    'ldo.c'
    'ldump.c'
    'lfunc.c'
    'lgc.c'
    'linit.c'
    'liolib.c'
    'llex.c'
    'lmathlib.c'
    'lmem.c'
    'loadlib.c'
    'lobject.c'
    'lopcodes.c'
    'loslib.c'
    'lparser.c'
    'lstate.c'
    'lstring.c'
    'lstrlib.c'
    'ltable.c'
    'ltablib.c'
    'ltm.c'
    'lundump.c'
    'lvm.c'
    'lzio.c'
)

for f in ${files[*]}; do
    rm $f
done
