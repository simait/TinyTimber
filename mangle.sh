#!/bin/sh

MANGLE_SPEC=env/$1/mangle.spec
if test ! -f $MANGLE_SPEC
then
	echo "Environment ${1} does not support mangling."
	exit 1
fi

TT_C=`cat $MANGLE_SPEC|awk -F= '/TT_C/{print $2}'`
TT_H=`cat $MANGLE_SPEC|awk -F= '/TT_H/{print $2}'`

TT_C="${TT_C} tT/kernel.c"
TT_H="${TT_H} tT/tT.h tT/kernel.h"

MANGLED_DIR=mangled/$1
MANGLED_C=mangled/$1/tT.c
MANGLED_H=mangled/$1/tT.h

SHA1=`git-show-ref --heads --hash`
ORIGIN="/* MANGLED ORIGIN: ${SHA1} */\n"

MANGLED_C_HEADER="#include <tT.h>"
MANGLED_C_FOOTER=""

MANGLED_H_HEADER="#ifndef TT_MANGLED\n#define TT_MANGLED"
MANGLED_H_FOOTER="#endif\n"

mkdir -p $MANGLED_DIR

rm -f $MANGLED_C
echo -e "${ORIGIN}" >> $MANGLED_C
echo -e "${MANGLED_C_HEADER}" >> $MANGLED_C
for i in $TT_C
do
	echo -e "\n/* MANGLE FILE: ${i} */\n" >> $MANGLED_C
	cat $i >> $MANGLED_C
done
echo -e "${MANGLED_C_FOOTER}" >> $MANGLED_C

rm -f $MANGLED_H
echo -e "${ORIGIN}" >> $MANGLED_H
echo -e "${MANGLED_H_HEADER}" >> $MANGLED_H
for i in $TT_H
do
	echo -e "\n/* MANGLE FILE: ${i} */\n" >> $MANGLED_H
	cat $i >> $MANGLED_H
done
echo -e "${MANGLED_H_FOOTER}" >> $MANGLED_H
