/**
 * Voice Navi Firmware
 *
 * Different play modes for Voice Navi
 *
 * Author: Jernej Pangerc (Azur Test)
 * Date: Aug 2021
 *
 * Copyright (C) 2021 Azur Test. All rights reserved
 */

#ifndef __PLAY_MODES_H
#define __PLAY_MODES_H

uint8_t normalPlay(uint8_t *songArray);
uint8_t lastInputInterruptPlay(uint8_t *songArray);
uint8_t priorityPlay(uint8_t *songArray);
uint8_t inputPlay(uint8_t *songArray);
uint8_t binary128ch(uint8_t *songArray);
uint8_t binary255_positive(uint8_t *songArray);
uint8_t binary255_negative(uint8_t *songArray);
uint8_t bitOrder(uint8_t);
uint8_t switchToPlay(uint16_t, uint8_t*);

typedef union un_gpio
{
  uint8_t BYTE;
  struct
  {
    uint8_t GPIO1 :1;
    uint8_t GPIO2 :1;
    uint8_t GPIO3 :1;
    uint8_t GPIO4 :1;
    uint8_t GPIO5 :1;
    uint8_t GPIO6 :1;
    uint8_t GPIO7 :1;
    uint8_t GPIO8 :1;
  } BIT;
} gpio_t;

#endif //__PLAY_MODES_H
