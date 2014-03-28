### Application-specific constants

APP_NAME := util_sink

### Constant symbols

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar

CFLAGS := -O2 -Wall -Wextra -std=c99 -Iinc -I.

### General build targets

all: $(APP_NAME)

clean:
	rm -f obj/*.o
	rm -f $(APP_NAME)

### Main program compilation and assembly

obj/$(APP_NAME).o: src/$(APP_NAME).c 
	$(CC) -c $(CFLAGS) $< -o $@

$(APP_NAME): obj/$(APP_NAME).o
	$(CC) $< -o $@

### EOF