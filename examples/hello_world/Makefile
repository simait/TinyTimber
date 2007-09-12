################################################################################
# ARCH is required to determine which files to build etc.
################################################################################
#ENV_ARCH=avr
#ENV_ARCH=msp430
#ENV_ARCH=arm-elf

ifndef ENV_ARCH
$(error Required variable ENV_ARCH was not defined.)
endif

################################################################################
# TT_ROOT and ENV_ROOT are required to build the tinyTimber objects. In
# this simple example we use ARCH to determine which environment we are
# building for.
################################################################################

TT_ROOT=../../tT/
ENV_ROOT=../../env/

################################################################################
# If we define APP_SRC then we must also define the APP_ELF macro so we
# know where to output the elf binary. Please note that the build system
# will not create a binary image, only ELF images.
################################################################################

APP_SRC=main.c
APP_ELF=main.elf

################################################################################
# This is simply the CC specific switch used to determine which CPU to
# compile the code for.
################################################################################

#MCU				= at90can128
#MCU				= msp430x1611
#MCU				= arm7tdimi

################################################################################
# Include the file that performes the real magic, such as setting the
# environment sources as well as the kernel sources. Will include the
# targets env, tt, clean, and app (if APP_SRC is non-empty).
################################################################################
include ../../Makefile

################################################################################
# Since we can't really supply the means of flashing the device in a somewhat
# portable manner we'll make the target specific for each ARCH.
################################################################################

# msp 430 specific targets
ifeq ($(ENV_ARCH), msp430)
flash: $(APP_ELF)
	msp430-jtag --no-close --elf -e $(APP_ELF)
endif

# avr specific targets.
ifeq ($(ENV_ARCH), avr)
$(TEST_HEX): $(TEST_ELF)
	avr-objcopy -R .eeprom -O ihex $(TEST_ELF) $(TEST_HEX)

flash: $(TEST_HEX)
	avrdude -c jtag2 -P usb -p c128 -U flash:w:$(TEST_HEX)
	
debug:
	avarice --mkII --detach --jtag usb :2000
	avr-gdb -x env/avr/gdbrc
endif
