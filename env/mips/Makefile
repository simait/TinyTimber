################################################################################
# Make sure the ROOT variable is set as it defines where to fing the
# environment.
################################################################################

ifndef ENV_ROOT
$(error Variable ENV_ROOT was not defined.)
endif

################################################################################
# Set the CC so that we may build the system.
################################################################################

CC				= mipsel-linux-gcc

# ENV_MCU, we need gcc specific flag, not always the same :/
ifndef ENV_MCU
$(error Variable ENV_MCU was not defined.)
endif
ENV_MCU				:= -mcpu=$(ENV_MCU)

################################################################################
# Setup all the ENV_* variables, ENV_CFLAGS we allow the user to
# override so check if it's defined.
################################################################################

ENV_DEF 		= -DENV_MIPS=1
ENV_SRC			= $(ENV_ROOT)/$(ENV_ARCH)/env.c

ENV_INC			= -I$(ENV_ROOT)
ENV_LDFLAGS		= -Wl,-T$(ENV_ROOT)/$(ENV_ARCH)/mips_r3k.x
ifndef ENV_CFLAGS
ENV_CFLAGS		= -O2 -Wall -nostartfiles -nostdlib
endif

################################################################################
# Ok, some sanity check of the TT_CFLAGS and APP_CFLAGS is needed, we must
# use -mthumb-interwork -nostartfiles on both.
################################################################################
TT_CFLAGS		+= -nostartfiles -nostdlib
APP_CFLAGS		+= -nostartfiles -nostdlib
