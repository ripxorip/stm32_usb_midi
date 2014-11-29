/**
 * @file main.c
 * @author Philip Karlsson
 * @version V1.0.0
 * @date 29-November-2014
 * @brief This file contains a simple example usage of the new midi device class.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "main.h"

// Midi
#include "usbd_usr.h"
#include "usbd_desc.h"

#include "usbd_midi_core.h"
#include "usbd_midi_user.h"

#define BUTTON      (GPIOA->IDR & GPIO_Pin_0)

// Private variables
volatile uint32_t time_var1, time_var2;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

// Private function prototypes
void Delay(volatile uint32_t nCount);
void init();
void calculation_test();

int main(void) {
	// Simple midi messages for test notes..
	uint8_t noteOn[4];
	noteOn[0] = 0x08;
	noteOn[1] = 0x90;
	noteOn[2] = 0x47;
	noteOn[3] = 0x47;

	uint8_t noteOff[4];
	noteOff[0] = 0x08;
	noteOff[1] = 0x80;
	noteOff[2] = 0x47;
	noteOff[3] = 0x47;

	init();


	int active = 0;

	for(;;) {
		if (BUTTON) {
				// Debounce
				Delay(10);
				if (BUTTON) {
					if(!active){
						active = 1;
						GPIO_SetBits(GPIOD, GPIO_Pin_12);
						send_MIDI_msg(noteOn, 4); // Send to usbd_midi_user
					}
				}
				else{
					active = 0;
					GPIO_ResetBits(GPIOD, GPIO_Pin_12);
					send_MIDI_msg(noteOff, 4);
					Delay(10);
				}

			}
	}

	return 0;
}


void init() {
	GPIO_InitTypeDef  GPIO_InitStructure;

	// ---------- SysTick timer -------- //
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error
		while (1){};
	}

	// ---------- GPIO -------- //
	// GPIOD Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	// Configure PD12, PD13, PD14 and PD15 in output pushpull mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// ------------- USB -------------- //
    /* Initialize the midi driver */
	usbd_midi_init_driver(&MIDI_fops);
    /* Make the connection and initialize to USB_OTG/usbdc_core */
	USBD_Init(&USB_OTG_dev,
	            USB_OTG_FS_CORE_ID,
	            &USR_desc,
				&USBD_MIDI_cb,
	            &USR_cb);
}

/*
 * Called from systick handler
 */
void timing_handler() {
	if (time_var1) {
		time_var1--;
	}

	time_var2++;
}

/*
 * Delay a number of systick cycles (1ms)
 */
void Delay(volatile uint32_t nCount) {
	time_var1 = nCount;
	while(time_var1){};
}

/*
 * Dummy function to avoid compiler error
 */
void _init() {

}

