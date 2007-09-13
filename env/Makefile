################################################################################
# Make sure the ROOT variable is set as it defines where the root of
# the tinyTimber code is located.
################################################################################

ifndef TT_ROOT
$(error Variable TT_ROOT was not defined.)
endif

# Fix the kernel sources etc.
TT_SRC	= $(TT_ROOT)/kernel.c
TT_OBJ	= $(patsubst %.c, %.o, $(TT_SRC))
TT_INC	= -I./$(TT_ROOT)

################################################################################
# Make sure the Environment related variables are defined. This is a
# sanity check.
################################################################################

# ENV_ROOT is the root to the given environment, we should make sure
# it is set.
ifndef ENV_ROOT
$(error ENV_ROOT was not set.)
endif

# Include the environment Makefile.
include $(ENV_ROOT)/$(ENV_ARCH)/Makefile

# ENV_SRC should contain the sources for the environment.
ifndef ENV_SRC
$(error Variable ENV_SRC was not defined.)
endif
ENV_OBJ	= $(patsubst %.c, %.o, $(patsubst %.S, %.o, $(ENV_SRC)))

# ENV_INC should contain the environment specific includes.
ifndef ENV_INC
$(error Variable ENV_INC was not defined.)
endif

################################################################################
# Create the default targets, all should build all the environment
# objects along with the tinyTimber sources. If APP_SRC is defined then
# those objects will be built as well.
################################################################################

ifdef APP_SRC
ifndef APP_ELF
$(error APP_SRC was defined but not APP_ELF.)
endif
APP_HEX = $(patsubst %.elf, %.hex, $(APP_ELF))
APP_BIN = $(patsubst %.elf, %.bin, $(APP_ELF))
APP_OBJ = $(patsubst %.c, %.o, $(patsubst %.S, %.o, $(APP_SRC)))
endif

################################################################################
# The default targets, we supply all, clean, env, tt, and app(if APP_SRC
# is defined).
################################################################################

# Make sure the ENV_MCU variable is set since it's /very/ vital for
# compiling the code.
ifndef ENV_MCU
$(error Variable ENV_MCU was not set.)
endif

# Only include app if APP_SRC was defined.
ifdef APP_SRC
.PHONY: all env tt app clean
all: env tt app
else
.PHONY: all env tt clean
all: env tt
endif

ENV_TT_FLAGS:=$(sort $(ENV_CFLAGS) $(ENV_INC) $(TT_INC) $(ENV_DEF) $(ENV_MCU))

env: CFLAGS=$(ENV_TT_FLAGS)
env: ASFLAGS=$(ENV_TT_FLAGS)
env: $(ENV_OBJ)

tt:	CFLAGS=$(ENV_TT_FLAGS)
tt: ASFLAGS=$(ENV_TT_FLAGS)
tt: $(TT_OBJ)

ifdef APP_SRC
ENV_TT_APP_FLAGS:=$(sort $(ENV_TT_FLAGS) $(APP_CFLAGS) $(APP_INC) $(APP_DEF))
app: CFLAGS=$(ENV_TT_APP_FLAGS)
app: ASFLAGS=$(ENV_TT_APP_FLAGS)
app: $(APP_OBJ)

$(APP_ELF): CFLAGS=$(ENV_TT_APP_FLAGS)
$(APP_ELF): LDFLAGS=$(sort $(ENV_LDFLAGS) $(APP_LDFLAGS))
$(APP_ELF): env tt app
	$(CC) $(CFLAGS) $(LDFLAGS) $(APP_OBJ) $(TT_OBJ) $(ENV_OBJ) -o $@

# elf target, easier than to type the name of the elf.
elf: $(APP_ELF)
endif

ifdef APP_SRC
clean:
	rm -rf $(TT_OBJ) $(ENV_OBJ) $(APP_OBJ) $(APP_ELF)
else
clean:
	rm -rf $(TT_OBJ) $(ENV_OBJ)
endif
