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

CC				= msp430-gcc

# MCU, we need gcc specific flag, not always the same :/
ifndef MCU
$(error Variable MCU was not defined.)
endif
MCU				:= -mmcu=$(MCU)

################################################################################
# Setup all the ENV_* variables, ENV_CFLAGS we allow the user to
# override so check if it's defined.
################################################################################

ENV_DEF 		= -DENV_MSP430=1
ENV_SRC			= $(ENV_ROOT)/$(ENV_ARCH)/env.c

ENV_INC			= -I$(ENV_ROOT)
ifndef ENV_CFLAGS
ENV_CFLAGS		= -O2 -Wall -mendup-at=main -mno-stack-init
endif
