/**
 * @file usbd_midi_user.h
 * @author Philip Karlsson
 * @version V1.0.0
 * @date 29-November-2014
 * @brief The header for the unser interface to usbd_midi_core
 */

#ifndef __USBD_MIDI_USER_H
#define __USBD_MIDI_USER_H
/* Includes ------- */

#include "stm32f4xx_conf.h"

#include "usbd_midi_core.h"
#include "usbd_conf.h"

extern MIDI_IF_Prop_TypeDef MIDI_fops;

extern void send_MIDI_msg(uint8_t *msg, uint16_t length);

#endif
