STM32 USB MIDI
===============

This is my usb midi device class for stm32 MCUs. The user interface for the class is located under src/usbd_midi_user.c and /inc/usbd_midi_user.h. The actual class can be found under /lib/USB_Device/Class/midi/src and /lib/USB_Device/Class/midi/inc.

Compilation
-----------
Download and install the following software for your platform
  * The gcc-arm-none-eabi toolchain.
  * openOCD.
  * st-link (optional)
  * The ST firmware - [Download from ST](http://www.st.com/web/en/catalog/tools/PF257904#)
  * Specify the folder of the firmware in the Makefile and run make.
  * Upload the resuling binary to the board using openOCD or st-link.

Usage
-----
After programming the MCU it should be detected as a midi device by any standard OS such as Windows, OS X and Linux. If you are using a stm32f4-discovery board the blue button should generate a test note which is sent to the host. 
