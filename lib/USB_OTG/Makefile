BINPATH=~/sat/bin

CC=$(BINPATH)/arm-none-eabi-gcc
AR=$(BINPATH)/arm-none-eabi-ar

###################################################

# Check for valid float argument
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else
CFLAGS += -msoft-float
endif

CFLAGS += -ffreestanding -nostdlib

# Includes
CFLAGS += -Iinc -I../Core/cmsis -I../Core/stm32
CFLAGS += -I../Conf

# Sources
SRCS = usb_core.c usb_dcd.c usb_dcd_int.c usb_hcd.c
SRCS += usb_hcd_int.c usb_otg.c

OBJS = $(SRCS:.c=.o)
LIBNAME = libusbcore.a

all: $(LIBNAME)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIBNAME): $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBNAME)
	