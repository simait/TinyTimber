CC		= gcc

CFLAGS	= -I./ -I../../env/ -I../../tT -Wall -g -O2 -DTT_SRP=1 -DENV_POSIX=1
LDFLAGS	= -lrt

TT_SRC	= ../../tT/kernel_srp.c ../../tT/objects_srp.c
ENV_SRC	= ../../env/posix_srp/env.c
APP_SRC	= main.c

TT_OBJ	= $(patsubst %.c, %.o, $(TT_SRC))
ENV_OBJ	= $(patsubst %.c, %.o, $(ENV_SRC))
APP_OBJ	= $(patsubst %.c, %.o, $(APP_SRC))
APP_BIN	= srp

all: $(APP_BIN)

$(APP_BIN): $(TT_OBJ) $(ENV_OBJ) $(APP_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(TT_OBJ) $(ENV_OBJ) $(APP_OBJ) $(APP_BIN)
