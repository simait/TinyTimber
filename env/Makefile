################################################################################
# Setup some helper macros to check for this such as if a variable is
# defined or not, setting the correct output of an object file etc.
################################################################################

# Simple reuired function, not pretty to call it but at least it's
# somewhat more clear what it means/does.
define required
ifeq ($$(flavor $1),undefined)
$$(error $1 was undefined)
endif
endef

# Fix the location of the object files for a given set of object files.
# We must also set the output prefix here, before we mess it up.
OUTPUT_PREFIX := $(ENV_ARCH)/$(ENV_MCU)-
define object_output
$(addprefix $(OUTPUT_PREFIX),$(notdir $1))
endef

# function for changing the suffix of a set of files to .o
define object_convert
$(patsubst %.c,%.o, $(patsubst %.S,%.o,$1))
endef

# Target rule for a given source file
define object_target
$(call object_output,$(call object_convert,$1)):
	$$(CC) $$(CFLAGS) $1 -c -o $$@
endef

################################################################################
# Make sure the TT_ROOT variable is set as it defines where the root of
# the tinyTimber code is located.
################################################################################

# Variable TT_ROOT is required.
$(eval $(call required,TT_ROOT))

# Fix the kernel sources etc.
TT_SRC = $(TT_ROOT)/kernel.c
TT_OBJ = $(call object_convert, $(TT_SRC))
TT_OBJ := $(call object_output, $(TT_OBJ))
TT_INC = -I./$(TT_ROOT)

################################################################################
# Make sure the Environment related variables are defined. This is a
# sanity check.
################################################################################

# ENV_ROOT is the root to the given environment, we should make sure
# it is set.
$(eval $(call required,ENV_ROOT))

# Include the environment Makefile.
include $(ENV_ROOT)/$(ENV_ARCH)/Makefile

# ENV_SRC should contain the sources for the environment.
$(eval $(call required,ENV_SRC))
ENV_OBJ	= $(call object_convert,$(ENV_SRC))
ENV_OBJ := $(call object_output,$(ENV_OBJ))

# ENV_INC should contain the environment specific includes.
$(eval $(call required,ENV_INC))

################################################################################
# Create the default targets, all should build all the environment
# objects along with the tinyTimber sources. If APP_SRC is defined then
# those objects will be built as well.
################################################################################

ifdef APP_SRC
# Do not use required here since we want specail message...
ifndef APP_ELF
$(error APP_SRC was defined but not APP_ELF.)
endif
APP_ELF := $(OUTPUT_PREFIX)$(APP_ELF)
APP_HEX = $(patsubst %.elf, %.hex, $(APP_ELF))
APP_MOT = $(patsubst %.elf, %.mot, $(APP_ELF))
APP_BIN = $(patsubst %.elf, %.bin, $(APP_ELF))
APP_OBJ = $(call object_convert, $(APP_SRC))
APP_OBJ := $(call object_output, $(APP_OBJ))
endif

################################################################################
# The default targets, we supply all, clean, env, tt, and app(if APP_SRC
# is defined).
################################################################################

.SUFFIXES:

# Make sure the ENV_MCU variable is set since it's /very/ vital for
# compiling the code.
$(eval $(call required,ENV_MCU))

# Setup the output directory for the object files.
$(eval $(shell mkdir -p $(ENV_ARCH)))

# Only include app if APP_SRC was defined.
ifdef APP_SRC
.PHONY: all env tt app clean
all: env tt app
else
.PHONY: all env tt clean
all: env tt
endif

# Generate the targets for each environment source file.
$(foreach i,$(ENV_SRC),$(eval $(call object_target,$(i))))

ENV_TT_FLAGS:=$(sort $(ENV_CFLAGS) $(ENV_INC) $(TT_INC) $(ENV_DEF) $(ENV_MCU))

env: CFLAGS=$(ENV_TT_FLAGS)
env: ASFLAGS=$(ENV_TT_FLAGS)
env: $(ENV_OBJ)

# Generate the targets for each tinyTimber source file.
$(foreach i,$(TT_SRC),$(eval $(call object_target,$i)))

tt:	CFLAGS=$(ENV_TT_FLAGS)
tt: ASFLAGS=$(ENV_TT_FLAGS)
tt: $(TT_OBJ)

ifdef APP_SRC
# Generate the targets for each application source file.
$(foreach i,$(APP_SRC),$(eval $(call object_target,$(i))))

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
mot: $(APP_MOT)
bin: $(APP_BIN)
endif

ifdef APP_SRC
clean:
	rm -f $(TT_OBJ) $(ENV_OBJ) $(APP_OBJ) $(APP_ELF) $(APP_HEX) $(APP_BIN) $(APP_MOT)
else
clean:
	rm -f $(TT_OBJ) $(ENV_OBJ)
endif
