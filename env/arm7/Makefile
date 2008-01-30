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
CFLAGS	:= -DENV_ARM7=1 -O2 -Wall -nostartfiles -mthumb-interwork\
			   -I$(ENV_ROOT) -mcpu=arm7tdmi $(CFLAGS)
LDFLAGS	:= -Wl,-T$(ENV_ROOT)/$(ENV)/at91sam7x256.x $(LDFLAGS)

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################

$(BUILD_ROOT)/crt.o: $(ENV_ROOT)/$(ENV)/crt.S
	$(CC) $< -c -o $@

$(BUILD_ROOT)/env.o: $(ENV_ROOT)/$(ENV)/env.c
	$(CC) $(CFLAGS) $< -c -o $@

$(BUILD_ROOT)/sys_call.o: $(ENV_ROOT)/$(ENV)/sys_call.c
	$(CC) $(CFLAGS) $< -c -o $@

################################################################################
# Setup the required objects for the enviroment sources.
################################################################################

ENV_OBJECTS := $(BUILD_ROOT)/crt.o\
			   $(BUILD_ROOT)/env.o\
			   $(BUILD_ROOT)/sys_call.c
