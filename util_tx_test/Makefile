### Application-specific constants

APP_NAME := util_tx_test

### Constant symbols

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar

CFLAGS := -O2 -Wall -Wextra -std=c99 -Iinc -I.

### General build targets

all: $(APP_NAME)

clean:
	rm -f obj/*.o
	rm -f $(APP_NAME)

### Sub-modules compilation

obj/base64.o: src/base64.c inc/base64.h
	$(CC) -c $(CFLAGS) $< -o $@

### Main program compilation and assembly

obj/$(APP_NAME).o: src/$(APP_NAME).c inc/base64.h
	$(CC) -c $(CFLAGS) $< -o $@

$(APP_NAME): obj/$(APP_NAME).o  obj/base64.o
	$(CC) $<  obj/base64.o -o $@

### EOF