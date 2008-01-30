################################################################################
# Check the required variables, such as BUILD_ROOT, TT_ROOT, and ENV_ROOT.
################################################################################

ifndef BUILD_ROOT
$(error Variable BUILD_ROOT was not defined.)
endif

ifndef APP_ROOT
$(error Variable APP_ROOT was not defined.)
endif

################################################################################
# Setup any build related flags, such as CC, AS, LDFLAGS, CFLAGS etc.
################################################################################

CFLAGS		:= -I$(APP_ROOT) $(CFLAGS)

################################################################################
# Setup the rules for building the required object files from the source.
################################################################################

$(BUILD_ROOT)/main.o: $(APP_ROOT)/main.c
	$(CC) $(CFLAGS) $< -c -o $@

################################################################################
# Setup the required objects for the application sources.
################################################################################

APP_OBJECTS	:= $(BUILD_ROOT)/main.o

################################################################################
# Last but not the least we define the binary output of the application.
################################################################################

APP_BINARY	:= $(BUILD_ROOT)/app.elf
