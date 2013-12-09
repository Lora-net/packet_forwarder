### Application-specific constants

APP_NAME=util_tx_test

### constant symbols
CROSS_COMPILE=
CC=gcc
CFLAGS=-O2 -Wall -Wextra -Iinc
C99FLAGS=-O2 -Wall -Wextra -std=c99 -Iinc
FLAG_AUX=

### general build targets

all: $(APP_NAME)

clean:
	rm -f obj/*.o
	rm -f $(APP_NAME)

### sub-modules compilation

obj/base64.o: src/base64.c 
	$(CROSS_COMPILE)$(CC) -c $(CFLAGS) -o obj/base64.o src/base64.c $(FLAG_AUX)

### main program compilation and assembly

obj/$(APP_NAME).o: src/$(APP_NAME).c 
	$(CROSS_COMPILE)$(CC) -c $(C99FLAGS) -o obj/$(APP_NAME).o src/$(APP_NAME).c $(FLAG_AUX)

$(APP_NAME): obj/$(APP_NAME).o obj/base64.o
	$(CROSS_COMPILE)$(CC) -o $(APP_NAME) obj/$(APP_NAME).o obj/base64.o
