################################################################################
# ENV is a required variable since it's used to generate a lot of
# the other variables such as BUILD_ROOT etc.
################################################################################

#ENV=arm7
#ENV=avr5
#ENV=m16c
#ENV=mips
#ENV=msp430
#ENV=pic18
#ENV=posix
#ENV=posix_srp
#ENV=skel

ifndef ENV
$(error Variable ENV was not defined.)
endif

################################################################################
# Almost as important as the ENV variable is the SRP variable. SRP
# controls if the SRP or regular version of the kernel should be used. Since
# this is not an SRP example we will not define SRP.
################################################################################

#SRP=yes

################################################################################
# The BUILD_ROOT Variable is required as all the objects will be created in
# the $(BUILD_ROOT)/ directory.
################################################################################

BUILD_ROOT=./$(ENV)

################################################################################
# TT_ROOT specifies where the TinyTimber headers and source is located.
################################################################################

TT_ROOT=../../tT/

################################################################################
# ENV_ROOT specifies the Environment root, where all the different environents
# are located.
################################################################################

ENV_ROOT=../../env/

################################################################################
# APP_ROOT specifies the Application root, where the source of the application
# is located.
################################################################################

APP_ROOT=./src/

################################################################################
# Create the all target before we include any other target rules in Environment/
# Kernel Makefiles.
################################################################################

.PHONY: all build_root build clean
all: build_root build

################################################################################
# Now we will include the Makefile for the environment that we wish to build.
################################################################################

include $(ENV_ROOT)/$(ENV)/Makefile

################################################################################
# Now we will include the Makefile for the TinyTimber kernel that we wish
# to build (Regular or SRP). Make sure that the enviroment supports the
# kernel we wish to build.
################################################################################

include $(TT_ROOT)/Makefile

################################################################################
# And last but not the least include the application Makefile.
################################################################################

include $(APP_ROOT)/Makefile

################################################################################
# Define the targets required by all.
################################################################################

build_root:
	@test -d $(BUILD_ROOT) || mkdir -p $(BUILD_ROOT)

build: $(ENV_OBJECTS) $(TT_OBJECTS) $(APP_OBJECTS) $(APP_BINARY)

$(APP_BINARY): $(ENV_OBJECTS) $(TT_OBJECTS) $(APP_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

################################################################################
# Last but not the least let's make a clean rule.
################################################################################

clean:
	rm -f $(ENV_OBJECTS) $(TT_OBJECTS) $(APP_OBJECTS)

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
endif

ifeq ($(ENV_ARCH), m16c)
ifdef APP_MOT
# Create the mot file.
$(APP_MOT): $(APP_ELF)
	m32c-elf-objcopy -O srec $(APP_ELF) $(APP_MOT)

# Flash the mot file to the device.
flash: $(APP_MOT)
	~/src/sf/sf.py --device /dev/ttyUSB0 --device-id 0:0:0:0:0:0:0 --baud-rate 57600 --input-file $(APP_MOT) --id-validate --flash-erase-all --flash-write
endif
endif
