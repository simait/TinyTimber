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

CC		:= mipsel-linux-gcc
CFLAGS	:= -DENV_MIPS=1 -mmcu=r3k -O2 -Wall -nostartfiles -nostdlib\
		   -I$(ENV_ROOT) $(CFLAGS)
LDFLAGS	:= -Wl,-T$(ENV_ROOT)/$(ENV)/mips_r3k.x $(LDFLAGS)

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################

$(BUILD_ROOT)/env.o: $(ENV_ROOT)/$(ENV)/env_asm.S
	$(CC) $< -c -o $@

$(BUILD_ROOT)/env.o: $(ENV_ROOT)/$(ENV)/env.c
	$(CC) $(CFLAGS) $< -c -o $@

################################################################################
# Setup the required objects for the enviroment sources.
################################################################################

ENV_OBJECTS := $(BUILD_ROOT)/env_asm.o\
			   $(BUILD_ROOT)/env.o

