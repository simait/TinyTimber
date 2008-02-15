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
AS		:= arm-elf-as
ASFLAGS	:= -mthumb-interwork -mcpu=arm7tdmi
CFLAGS	:= -DENV_GBA=1 -O2 -gstabs+ -Wall -mthumb-interwork -fPIC\
			   -I$(ENV_ROOT) -mcpu=arm7tdmi $(CFLAGS)
LDFLAGS	:= -Wl,-T$(ENV_ROOT)/$(ENV)/gba.x $(LDFLAGS)\
		   -mthumb-interwork -nostartfiles -mcpu=arm7tdmi

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################

$(BUILD_ROOT)/crt.o: $(ENV_ROOT)/$(ENV)/crt.S
	$(AS) $(ASFLAGS) -k -mfloat-abi=soft $< -o $@

$(BUILD_ROOT)/bios.o: $(ENV_ROOT)/$(ENV)/bios.S
	$(CC) $(ASFLAGS) $< -c -o $@

$(BUILD_ROOT)/env.o: $(ENV_ROOT)/$(ENV)/env.c
	$(CC) $(CFLAGS) $< -c -o $@

$(BUILD_ROOT)/arm7.o: $(ENV_ROOT)/$(ENV)/arm7.c
	$(CC) $(CFLAGS) $< -c -o $@

$(BUILD_ROOT)/sys_call.o: $(ENV_ROOT)/$(ENV)/sys_call.c
	$(CC) $(CFLAGS) $< -c -o $@

################################################################################
# Setup the required objects for the enviroment sources.
################################################################################

ENV_OBJECTS := $(BUILD_ROOT)/crt.o\
			   $(BUILD_ROOT)/bios.o\
			   $(BUILD_ROOT)/env.o\
			   $(BUILD_ROOT)/arm7.o\
			   $(BUILD_ROOT)/sys_call.o

################################################################################
# Setup some special targets fort GBA environment.
################################################################################

GBA_BIOS	:= $(BUILD_ROOT)/bios.bin
GBA_BINARY	:= $(BUILD_ROOT)/app.bin

$(GBA_BIOS): $(APP_BINARY)
	$(ENV_ROOT)/$(ENV)/make_bios.sh $(APP_BINARY) $(GBA_BIOS)

$(GBA_BINARY): $(APP_BINARY)
	arm-elf-objcopy -j .text -O binary $(APP_BINARY) $(GBA_BINARY)
	
bios: $(GBA_BIOS)

binary: $(GBA_BINARY)

run: $(APP_BINARY) $(GBA_BIOS) 
	VisualBoyAdvance --bios=$(GBA_BIOS) $(APP_BINARY)

debug: $(APP_BINARY) $(GBA_BIOS)
	VisualBoyAdvance --bios=$(GBA_BIOS) --gdb=tcp $(APP_BINARY) > /dev/null &
	arm-elf-gdb -x $(ENV_ROOT)/$(ENV)/gdbrc $(APP_BINARY)
