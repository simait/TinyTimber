################################################################################
# Check the required variables, such as BUILD_ROOT, TT_ROOT, and ENV_ROOT.
################################################################################

ifndef BUILD_ROOT
$(error Variable BUILD_ROOT was not defined.)
endif

ifndef ENV_ROOT
$(error Variable ENV_ROOT was not defined.)
endif

################################################################################
# Sanity check since and SRP environment is quite different.
################################################################################
ifdef SRP
$(error Environment does not support SRP.)
endif

################################################################################
# Setup any build related flags, such as CC, AS, LDFLAGS, CFLAGS etc.
################################################################################

CC		:= arm-elf-gcc
ASFLAGS	:= -mthumb-interwork
CFLAGS	:= -DENV_ARM7=1 -O2 -gstabs+ -Wall -mthumb-interwork\
			   -I$(ENV_ROOT) -mcpu=arm7tdmi $(CFLAGS)
LDFLAGS	:= -Wl,-T$(ENV_ROOT)/$(ENV)/at91sam7x256.x $(LDFLAGS)\
		   -mthumb-interwork -nostartfiles -mcpu=arm7tdmi

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################

$(BUILD_ROOT)/crt.o: $(ENV_ROOT)/$(ENV)/crt.S
	$(CC) $(ASFLAGS) $< -c -o $@

$(BUILD_ROOT)/env.o: $(ENV_ROOT)/$(ENV)/env.c
	$(CC) $(CFLAGS) $< -c -o $@

$(BUILD_ROOT)/sys_call.o: $(ENV_ROOT)/$(ENV)/sys_call.c
	$(CC) $(CFLAGS) $< -c -o $@

################################################################################
# Setup the required objects for the enviroment sources.
################################################################################

ENV_OBJECTS := $(BUILD_ROOT)/crt.o\
			   $(BUILD_ROOT)/env.o\
			   $(BUILD_ROOT)/sys_call.o

################################################################################
# 
# ARM7 Specific targets, nothing portable to see here...
#
################################################################################

OPENOCD_BINARY		= $(BUILD_ROOT)/openocd_binary.bin
OPENOCD_DEBUG		= $(BUILD_ROOT)/openocd.olimex.debug
OPENOCD_FLASH		= $(BUILD_ROOT)/openocd.olimex.flash
OPENOCD_FLASH_GO	= $(BUILD_ROOT)/openocd.olimex.flash.go

$(OPENOCD_DEBUG): $(BUILD_ROOT)
	$(ENV_ROOT)/$(ENV)/openocd.olimex.debug.gen > $@

$(OPENOCD_FLASH): $(BUILD_ROOT)
	$(ENV_ROOT)/$(ENV)/openocd.olimex.flash.gen "${OPENOCD_FLASH_GO}" > $@

$(OPENOCD_FLASH_GO): $(BUILD_ROOT)
	$(ENV_ROOT)/$(ENV)/openocd.olimex.flash.go.gen "${OPENOCD_BINARY}" > $@

$(OPENOCD_BINARY): $(APP_BINARY)
	arm-elf-objcopy -O binary --strip-all $(APP_BINARY) $@

flash: $(OPENOCD_FLASH) $(OPENOCD_FLASH_GO) $(OPENOCD_BINARY)
	sudo openocd --file $(OPENOCD_FLASH)

debug:
	arm-elf-gdb -x $(ENV_ROOT)/$(ENV)/gdbrc $(APP_BINARY)

debug_launch: $(OPENOCD_DEBUG)
	sudo openocd --file $(OPENOCD_DEBUG) > /dev/null
