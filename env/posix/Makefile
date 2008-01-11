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

CC				= gcc

# Don't care about the MCU flags, just set it to -Wfatal-errors, for fun.
ENV_MCU				:= -Wfatal-errors

################################################################################
# Setup all the ENV_* variables, ENV_CFLAGS we allow the user to
# override so check if it's defined.
################################################################################

ENV_DEF 		= -DENV_POSIX=1 -DPOSIX_TICK=100000
ENV_SRC			= $(ENV_ROOT)/$(ENV_ARCH)/env.c\
				  $(ENV_ROOT)/$(ENV_ARCH)/ack.c

ENV_INC			= -I$(ENV_ROOT)
ifndef ENV_CFLAGS
ENV_CFLAGS		= -O2 -Wall -g
endif
ENV_LDFLAGS		= -lpthread
