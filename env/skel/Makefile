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

CC		:= gcc
CFLAGS	:= -DENV_SKEL=1 -Wall -O2 -I$(ENV_ROOT) $(CFLAGS)

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################

$(BUILD_ROOT)/env.o: $(ENV_ROOT)/$(ENV)/env.c
	$(CC) $(CFLAGS) $< -c -o $@

################################################################################
# Setup the required objects for the enviroment sources.
################################################################################

ENV_OBJECTS := $(BUILD_ROOT)/env.o
