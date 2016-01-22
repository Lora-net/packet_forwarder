### Application-specific constants

APP_NAME := lora_pkt_fwd

### Environment constants 

LGW_PATH ?= ../../lora_gateway/libloragw
ARCH ?=
CROSS_COMPILE ?=

### External constant definitions
# must get library build option to know if mpsse must be linked or not

include $(LGW_PATH)/library.cfg
RELEASE_VERSION := `cat ../VERSION`

### Constant symbols

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar

CFLAGS := -O2 -Wall -Wextra -std=c99 -Iinc -I.
VFLAG := -D VERSION_STRING="\"$(RELEASE_VERSION)\""

### Linking options

ifeq ($(CFG_SPI),native)
  LIBS := -lloragw -lrt -lpthread -lm
else ifeq ($(CFG_SPI),ftdi)
  LIBS := -lloragw -lrt -lpthread -lmpsse -lm
else
  $(error [error] Cannot find configuration for SPI)
endif

### General build targets

all: $(APP_NAME)

clean:
	rm -f obj/*.o obj/*.d
	rm -f $(APP_NAME)

### Sub-modules compilation

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
-include $(OBJ:.o=.d)

obj/%.o: src/%.c
	### Generate object file
	$(CC) $(CFLAGS) $(VFLAG) -I$(LGW_PATH)/inc -c $< -o $@
	### Generate dependency file
	$(CC) $(CFLAGS) $(VFLAG) -I$(LGW_PATH)/inc -MM -MT"$@" $< > obj/$*.d

### Main program assembly

$(APP_NAME): $(OBJ) $(LGW_PATH)/libloragw.a
	$(CC) -L$(LGW_PATH) $^ $(LIBS) -o $@

### EOF
