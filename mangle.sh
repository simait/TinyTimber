#!/bin/sh

case $1 in
	avr)
		TT_C="env/avr/env.c"
		TT_H="env/range.h env/msp430/types.h env/avr/env.h"
		;;
	msp430)
		TT_C="env/msp430/env.c"
		TT_H="env/range.h env/msp430/types.h env/msp430/env.h"
		;;
	pic18)
		TT_C="env/pic18/env.c"
		TT_H="env/range.h env/pic18/types.h env/pic18/env.h"
		;;
	*)
		echo "Unknown environment to mangle."
		exit 1
		;;
esac

TT_C="${TT_C} tT/kernel.c"
TT_H="${TT_H} tT/tT.h tT/kernel.h"

MANGLED_DIR=mangled/$1
MANGLED_C=mangled/$1/tT.c
MANGLED_H=mangled/$1/tT.h

SHA1=`cat .git/HEAD`
case $SHA1 in
	ref:*)
		SHA1=`echo "${SHA1}"|awk '{print ".git/" $2}'`
		SHA1=`cat "${SHA1}"`
		;;
esac
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
	echo -e "\n/* MANGLING FILE: ${i} */\n" >> $MANGLED_C
	cat $i >> $MANGLED_C
done
echo -e "${MANGLED_C_FOOTER}" >> $MANGLED_C

rm -f $MANGLED_H
echo -e "${ORIGIN}" >> $MANGLED_H
echo -e "${MANGLED_H_HEADER}" >> $MANGLED_H
for i in $TT_H
do
	echo -e "\n/* MANGLING FILE: ${i} */\n" >> $MANGLED_H
	cat $i >> $MANGLED_H
done
echo -e "${MANGLED_H_FOOTER}" >> $MANGLED_H
