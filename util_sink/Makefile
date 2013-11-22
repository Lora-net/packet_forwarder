### Application-specific constants

APP_NAME=util_sink

### constant symbols

CC=gcc
CFLAGS=-O2 -Wall -Wextra -Iinc
C99FLAGS=-O2 -Wall -Wextra -Iinc -std=c99
FLAG_AUX=

### general build targets

all: $(APP_NAME)

clean:
	rm -f obj/*.o
	rm -f $(APP_NAME)

### main program compilation and assembly

obj/$(APP_NAME).o: src/$(APP_NAME).c 
	$(CC) -c $(C99FLAGS) -o obj/$(APP_NAME).o src/$(APP_NAME).c $(FLAG_AUX)

$(APP_NAME): obj/$(APP_NAME).o
	$(CC) -o $(APP_NAME) obj/$(APP_NAME).o
