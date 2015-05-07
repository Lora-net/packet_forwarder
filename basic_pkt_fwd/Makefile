### Application-specific constants

APP_NAME := basic_pkt_fwd

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

### Constants for Lora concentrator HAL library
# List the library sub-modules that are used by the application

LGW_INC =
ifneq ($(wildcard $(LGW_PATH)/inc/config.h),)
  # only for HAL version 1.3 and beyond
  LGW_INC += $(LGW_PATH)/inc/config.h
endif
LGW_INC += $(LGW_PATH)/inc/loragw_hal.h

### Linking options

ifeq ($(CFG_SPI),native)
  LIBS := -lloragw -lrt -lpthread
else ifeq ($(CFG_SPI),ftdi)
  LIBS := -lloragw -lrt -lpthread -lmpsse
else
  # keep compatibility with SX1301 HAL version 1.2.x and bellow
  ifeq ($(LGW_PHY),native)
    LIBS := -lloragw -lrt -lpthread
  else ifeq ($(LGW_PHY),ftdi)
    LIBS := -lloragw -lrt -lpthread -lmpsse
  else
    $(error [error] Can't find configuration for SPI phy)
  endif
endif

### General build targets

all: $(APP_NAME)

clean:
	rm -f obj/*.o
	rm -f $(APP_NAME)

### Sub-modules compilation

obj/base64.o: src/base64.c inc/base64.h
	$(CC) -c $(CFLAGS) $< -o $@

obj/parson.o: src/parson.c inc/parson.h
	$(CC) -c $(CFLAGS) $< -o $@

### Main program compilation and assembly

obj/$(APP_NAME).o: src/$(APP_NAME).c $(LGW_INC) inc/parson.h inc/base64.h
	$(CC) -c $(CFLAGS) $(VFLAG) -I$(LGW_PATH)/inc $< -o $@

$(APP_NAME): obj/$(APP_NAME).o $(LGW_PATH)/libloragw.a obj/parson.o obj/base64.o
	$(CC) -L$(LGW_PATH) $< obj/parson.o obj/base64.o -o $@ $(LIBS)

### EOF
