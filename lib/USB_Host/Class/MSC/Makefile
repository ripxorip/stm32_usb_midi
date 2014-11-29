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
CFLAGS += -Iinc -I../../../Core/cmsis -I../../../Core/stm32
CFLAGS += -I../../../Conf -I../../../USB_OTG/inc
CFLAGS += -I../../Core/inc
CFLAGS += -I../../../fat_fs/inc

# Sources
SRCS = usbh_msc_bot.c usbh_msc_core.c usbh_msc_fatfs.c usbh_msc_scsi.c

OBJS = $(SRCS:.c=.o)
LIBNAME = libusbhostmsc.a

all: $(LIBNAME)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIBNAME): $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBNAME)
	