################################################################################
# ARCH is required to determine which files to build etc.
################################################################################

#ENV_ARCH=avr5
#ENV_ARCH=msp430
#ENV_ARCH=arm7
#ENV_ARCH=m16c

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
APP_CFLAGS=-g

################################################################################
# The target mcu we should compile the code for.
################################################################################

#ENV_MCU=at90can128
#ENV_MCU=msp430x1611
#ENV_MCU=arm7tdmi
#ENV_MCU=m16c

ifndef ENV_MCU
$(error Required variable ENV_MCU was not defined.)
endif

################################################################################
# Include the file that performes the real magic, such as setting the
# environment sources as well as the kernel sources. Will include the
# targets env, tt, clean, and app (if APP_SRC is non-empty).
################################################################################
include $(ENV_ROOT)/Makefile

################################################################################
# Since we can't really supply the means of flashing the device in a somewhat
# portable manner we'll make the target specific for each ARCH.
################################################################################

# msp430 specific targets
ifeq ($(ENV_ARCH), msp430)
flash: $(APP_ELF)
	msp430-jtag --no-close --elf -e $(APP_ELF)
endif

# avr5 specific targets.
ifeq ($(ENV_ARCH), avr5)
ifdef APP_HEX
$(APP_HEX): $(APP_ELF)
	avr-objcopy -R .eeprom -O ihex $(APP_ELF) $(APP_HEX)

flash: $(APP_HEX)
	avrdude -c jtag2 -P usb -p c128 -U flash:w:$<
	
debug:
	avarice --mkII --detach --jtag usb :2000
	avr-gdb -x $(ENV_ROOT)/$(ENV_ARCH)/gdbrc
endif

ifeq ($(ENV_ARCH), m16c)
ifdef APP_MOT
# m16c specific targets.
$(APP_MOT): $(APP_ELF)
	m32c-objcopy -O srec $(APP_ELF) $(APP_MOT)
endif
endif

endif
