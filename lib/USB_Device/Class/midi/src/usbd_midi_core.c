/**
 * @file usbd_midi_core.c
 * @author Philip Karlsson
 * @version V1.0.0
 * @date 29-November-2014
 * @brief This file contains the main implementation of the midi class
 *
 * This is the main implementation of the class which interfaces with the
 * usbd_core CMSIS library.
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"

/***********************************
 * MIDI Device library callbacks
 *
 **********************************/
static uint8_t  *USBD_midi_GetCfgDesc (uint8_t speed, uint16_t *length); // Done
static void Handle_USBAsynchXfer  (void *pdev);

static uint8_t  usbd_midi_Init        (void  *pdev, uint8_t cfgidx);
static uint8_t  usbd_midi_DeInit      (void  *pdev, uint8_t cfgidx);
static uint8_t  usbd_midi_DataIn      (void *pdev, uint8_t epnum);
static uint8_t  usbd_midi_DataOut     (void *pdev, uint8_t epnum);
static uint8_t  usbd_midi_SOF         (void *pdev);

uint32_t APP_Rx_ptr_in  = 0;
uint32_t APP_Rx_ptr_out = 0;
uint32_t APP_Rx_length  = 0;

uint8_t  USB_Tx_State = 0;


/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN uint8_t usbd_midi_CfgDesc[]  __ALIGN_END =
{
		// configuration descriptor
		        0x09, 0x02, 0x65, 0x00, 0x02, 0x01, 0x00, 0xc0, 0x50,

		        // The Audio Interface Collection
		        0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, // Standard AC Interface Descriptor
		        0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01, // Class-specific AC Interface Descriptor
		        0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, // MIDIStreaming Interface Descriptors
		        0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,             // Class-Specific MS Interface Header Descriptor

		        // MIDI IN JACKS
		        0x06, 0x24, 0x02, 0x01, 0x01, 0x00,
		        0x06, 0x24, 0x02, 0x02, 0x02, 0x00,

		        // MIDI OUT JACKS
		        0x09, 0x24, 0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00,
		        0x09, 0x24, 0x03, 0x02, 0x06, 0x01, 0x01, 0x01, 0x00,

		        // OUT endpoint descriptor
		        0x09, 0x05, MIDI_OUT_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
		        0x05, 0x25, 0x01, 0x01, 0x01,

		        // IN endpoint descriptor
		        0x09, 0x05, MIDI_IN_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
		        0x05, 0x25, 0x01, 0x01, 0x03,
} ;

__ALIGN_BEGIN uint8_t USB_Rx_Buffer   [MIDI_DATA_OUT_PACKET_SIZE] __ALIGN_END ;

__ALIGN_BEGIN uint8_t APP_Rx_Buffer   [APP_RX_DATA_SIZE] __ALIGN_END ;

MIDI_IF_Prop_TypeDef *APP_fops;

/* The interface class calbacks for the midi driver */
USBD_Class_cb_TypeDef  USBD_MIDI_cb =
{
		usbd_midi_Init,
		usbd_midi_DeInit,
		NULL,
		NULL,
		NULL,
		usbd_midi_DataIn,
		usbd_midi_DataOut,
		usbd_midi_SOF,
		NULL,
		NULL,
		USBD_midi_GetCfgDesc,
#ifdef USE_USB_OTG_HS
		USBD_midi_GetCfgDesc, /* use same config as per FS */
#endif /* USE_USB_OTG_HS  */
};

void usbd_midi_init_driver(MIDI_IF_Prop_TypeDef *addr){
	APP_fops = addr;
}


// Dummys - this one is hard..
static uint8_t  usbd_midi_Init        (void  *pdev, uint8_t cfgidx){
	/* Open the in EP */
	DCD_EP_Open(pdev,
	              MIDI_IN_EP,
	              MIDI_DATA_IN_PACKET_SIZE,
	              USB_OTG_EP_BULK);

	/* Open the out EP */
	DCD_EP_Open(pdev,
				  MIDI_OUT_EP,
				  MIDI_DATA_OUT_PACKET_SIZE,
				  USB_OTG_EP_BULK);

	/* Prepare Out endpoint to receive next packet */
	DCD_EP_PrepareRx(pdev,
	                   CDC_OUT_EP,
	                   (uint8_t*)(USB_Rx_Buffer),
	                   MIDI_DATA_OUT_PACKET_SIZE);

	return USBD_OK;
}

static uint8_t  usbd_midi_DeInit      (void  *pdev, uint8_t cfgidx){
	DCD_EP_Close(pdev,
			MIDI_IN_EP);
	DCD_EP_Close(pdev,
			MIDI_OUT_EP);
	return USBD_OK;
}


static uint8_t  usbd_midi_DataIn      (void *pdev, uint8_t epnum){
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;

	if (USB_Tx_State == 1)
	{
		if (APP_Rx_length == 0)
		{
			USB_Tx_State = 0;
		}
		else
		{
			if (APP_Rx_length > MIDI_DATA_IN_PACKET_SIZE){
				USB_Tx_ptr = APP_Rx_ptr_out;
				USB_Tx_length = MIDI_DATA_IN_PACKET_SIZE;

				APP_Rx_ptr_out += MIDI_DATA_IN_PACKET_SIZE;
				APP_Rx_length -= MIDI_DATA_IN_PACKET_SIZE;
			}
			else
			{
				USB_Tx_ptr = APP_Rx_ptr_out;
				USB_Tx_length = APP_Rx_length;

				APP_Rx_ptr_out += APP_Rx_length;
				APP_Rx_length = 0;
			}

			/* Prepare the available data buffer to be sent on IN endpoint */
			DCD_EP_Tx (pdev,
					MIDI_IN_EP,
					(uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr],
					USB_Tx_length);
		}
	}

	return USBD_OK;
}

static uint8_t  usbd_midi_DataOut     (void *pdev, uint8_t epnum){
//	Should take care of processing the data and deliver it to the application.
	uint16_t USB_Rx_Cnt;
	/* Get the received data buffer and update the counter */
	USB_Rx_Cnt = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;

	/* Forward the data to the user callback. */
	APP_fops->pIf_MidiRx(&USB_Rx_Buffer, USB_Rx_Cnt);

	DCD_EP_PrepareRx(pdev,
		                   CDC_OUT_EP,
		                   (uint8_t*)(USB_Rx_Buffer),
		                   MIDI_DATA_OUT_PACKET_SIZE);
	return USBD_OK;
}

static uint8_t  usbd_midi_SOF         (void *pdev){
	static uint32_t FrameCount = 0;

	if (FrameCount++ == MIDI_IN_FRAME_INTERVAL)
	{
		/* Reset the frame counter */
		FrameCount = 0;

		/* Check the data to be sent through IN pipe */
		Handle_USBAsynchXfer(pdev);
	}

	return USBD_OK;
}


static void Handle_USBAsynchXfer (void *pdev)
{
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;

	if(USB_Tx_State != 1)
	{
		if (APP_Rx_ptr_out == APP_RX_DATA_SIZE)
		{
			APP_Rx_ptr_out = 0;
		}

		if(APP_Rx_ptr_out == APP_Rx_ptr_in)
		{
			USB_Tx_State = 0;
			return;
		}

		if(APP_Rx_ptr_out > APP_Rx_ptr_in) /* rollback */
				{
			APP_Rx_length = APP_RX_DATA_SIZE - APP_Rx_ptr_out;

				}
		else
		{
			APP_Rx_length = APP_Rx_ptr_in - APP_Rx_ptr_out;

		}
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
		APP_Rx_length &= ~0x03;
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

		if (APP_Rx_length > MIDI_DATA_IN_PACKET_SIZE)
		{
			USB_Tx_ptr = APP_Rx_ptr_out;
			USB_Tx_length = MIDI_DATA_IN_PACKET_SIZE;

			APP_Rx_ptr_out += MIDI_DATA_IN_PACKET_SIZE;
			APP_Rx_length -= MIDI_DATA_IN_PACKET_SIZE;
		}
		else
		{
			USB_Tx_ptr = APP_Rx_ptr_out;
			USB_Tx_length = APP_Rx_length;

			APP_Rx_ptr_out += APP_Rx_length;
			APP_Rx_length = 0;
		}
		USB_Tx_State = 1;

		DCD_EP_Tx (pdev,
				MIDI_IN_EP,
				(uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr],
				USB_Tx_length);
	}

}

static uint8_t  *USBD_midi_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usbd_midi_CfgDesc);
  return usbd_midi_CfgDesc;
}
