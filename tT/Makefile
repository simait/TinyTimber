################################################################################
# Check the required variables, such as BUILD_ROOT, TT_ROOT, and ENV_ROOT.
################################################################################

ifndef BUILD_ROOT
$(error Variable BUILD_ROOT was not defined.)
endif

ifndef TT_ROOT
$(error Variable TT_ROOT was not defined.)
endif

################################################################################
# Setup any build related flags, such as CC, AS, LDFLAGS, CFLAGS etc.
################################################################################

ifdef SRP
CFLAGS	:= -DTT_SRP=1 -I$(TT_ROOT) $(CFLAGS)
else
CFLAGS	:= -I$(TT_ROOT) $(CFLAGS)
endif

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################


ifdef SRP
$(BUILD_ROOT)/kernel_srp.o: $(TT_ROOT)/kernel_srp.c
	$(CC) $(CFLAGS) $< -c -o $@
$(BUILD_ROOT)/objects_srp.o: $(TT_ROOT)/objects_srp.c
	$(CC) $(CFLAGS) $< -c -o $@
else
$(BUILD_ROOT)/kernel.o: $(TT_ROOT)/kernel.c
	$(CC) $(CFLAGS) $< -c -o $@
endif

################################################################################
# Setup the required objects for the kernel sources.
################################################################################

ifdef SRP
TT_OBJECTS := $(BUILD_ROOT)/kernel_srp.o\
			  $(BUILD_ROOT)/objects_srp.o
else
TT_OBJECTS := $(BUILD_ROOT)/kernel.o
endif
