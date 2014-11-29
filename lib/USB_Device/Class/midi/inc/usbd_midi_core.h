/**
 * @file usbd_midi_core.h
 * @author Philip Karlsson
 * @version V1.0.0
 * @date 29-November-2014
 * @brief This is the header for the implementation of the midi class.
 *
 */

#ifndef __USB_MIDI_CORE_H_
#define __USB_MIDI_CORE_H_

#include  "usbd_ioreq.h"

/* The structure of the callbacks related to the midi core driver.. */
extern USBD_Class_cb_TypeDef  USBD_MIDI_cb;

/* User callbacks for the midi driver */
typedef struct _MIDI_IF_PROP
{
	uint16_t (*pIf_MidiRx)		(uint8_t *msg, uint16_t length);
	uint16_t (*pIf_MidiTx)		(uint8_t *msg, uint16_t length);
}
MIDI_IF_Prop_TypeDef;

extern void usbd_midi_init_driver(MIDI_IF_Prop_TypeDef *addr);


extern uint8_t APP_Rx_Buffer   [APP_RX_DATA_SIZE];

extern uint32_t APP_Rx_ptr_in;
extern uint32_t APP_Rx_ptr_out;
extern uint32_t APP_Rx_length;

extern uint8_t  USB_Tx_State;


#endif  // __USB_CDC_CORE_H_
