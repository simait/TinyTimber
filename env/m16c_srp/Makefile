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
ifndef SRP
$(error SRP not defined for SRP environment)
endif

################################################################################
# Setup any build related flags, such as CC, AS, LDFLAGS, CFLAGS etc.
################################################################################

CC		:= m32c-elf-gcc
CFLAGS	:= -DENV_M16C=1 -mcpu=m16c -O2 -Wall -I$(ENV_ROOT) $(CFLAGS)
LDFLAGS	:= -Wl,-T$(ENV_ROOT)/$(ENV)/m16c.x -nostartfiles -mcpu=m16c

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################

$(BUILD_ROOT)/crt.o: $(ENV_ROOT)/$(ENV)/crt.S
	$(CC) $< -c -o $@

$(BUILD_ROOT)/env.o: $(ENV_ROOT)/$(ENV)/env.c
	$(CC) $(CFLAGS) $< -c -o $@

################################################################################
# Setup the required objects for the enviroment sources.
################################################################################

ENV_OBJECTS := $(BUILD_ROOT)/crt.o\
			   $(BUILD_ROOT)/env.o
