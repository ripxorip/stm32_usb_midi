/**
 * @file usbd_midi_user.c
 * @author Philip Karlsson
 * @version V1.0.0
 * @date 29-November-2014
 * @brief This file contains user interface to the midi class driver.
 *
 * This is the implementation of the user interface to usbd_midi_core. MIDI_DataRx 
 * is called whenever new midi data has arrived to the device. In order to send data
 * call send_MIDI_msg with the appropriate midi message to be sent to the host.
 */

/* Includes */
#include "usbd_midi_user.h"
#include "stm32f4xx_conf.h"


/* Private functions ------------
 *
 *
 */

static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);
static uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);

MIDI_IF_Prop_TypeDef MIDI_fops = {MIDI_DataRx, MIDI_DataTx};

/* This is the callback function that is called
 * whenever a midi message is revieved.
 */
static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length){
	return 0;
}

/* This function is called in order to send a midi message
 * over USB.
 */
void send_MIDI_msg(uint8_t *msg, uint16_t length){
	MIDI_DataTx(msg, length);
}

static uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length){
	uint32_t i = 0;
	while (i < length) {
		APP_Rx_Buffer[APP_Rx_ptr_in] = *(msg + i);
		APP_Rx_ptr_in++;
		i++;
		/* To avoid buffer overflow */
		if (APP_Rx_ptr_in == APP_RX_DATA_SIZE) {
			APP_Rx_ptr_in = 0;
		}
	}

	return USBD_OK;
}
