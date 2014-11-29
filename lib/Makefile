# Build all libraries

# Check for valid float argument
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

all:
	$(MAKE) -C StdPeriph FLOAT_TYPE=$(FLOAT_TYPE)
	$(MAKE) -C USB_OTG FLOAT_TYPE=$(FLOAT_TYPE)
	$(MAKE) -C USB_Device/Core FLOAT_TYPE=$(FLOAT_TYPE)
	$(MAKE) -C USB_Device/Class/cdc FLOAT_TYPE=$(FLOAT_TYPE)
	$(MAKE) -C USB_Device/Class/midi FLOAT_TYPE=$(FLOAT_TYPE)
#	$(MAKE) -C USB_Host/Core FLOAT_TYPE=$(FLOAT_TYPE)
#	$(MAKE) -C USB_Host/Class/MSC FLOAT_TYPE=$(FLOAT_TYPE)
	$(MAKE) -C fat_fs FLOAT_TYPE=$(FLOAT_TYPE)

clean:
	$(MAKE) clean -C StdPeriph
	$(MAKE) clean -C USB_OTG
	$(MAKE) clean -C USB_Device/Core
	$(MAKE) clean -C USB_Device/Class/cdc
#	$(MAKE) clean -C USB_Host/Core
#	$(MAKE) clean -C USB_Host/Class/MSC
	$(MAKE) clean -C fat_fs