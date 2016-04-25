### Application-specific constants

APP_NAME := util_ack

### Constant symbols

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar

CFLAGS := -O2 -Wall -Wextra -std=c99 -Iinc -I.

OBJDIR = obj

### General build targets

all: $(APP_NAME)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(APP_NAME)

### Main program compilation and assembly

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(APP_NAME): $(OBJDIR)/$(APP_NAME).o
	$(CC) $< -o $@

### EOF