#!/bin/sh
if test  $# -lt 2
then
	echo "Usage make_bios.sh <input> <output>"
	exit 1
fi

arm-elf-objcopy --only-section .bios -O binary $1 $2
