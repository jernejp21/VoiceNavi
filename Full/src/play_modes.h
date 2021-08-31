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

int16_t normalPlay();
int16_t lastInputPlay();
int16_t priorityPlay();
int16_t inputPlay();
int16_t binary128ch();
int16_t binary255_positive();
int16_t binary255_negative();
uint8_t bitOrder(uint8_t);

typedef union un_gpio
{
  uint8_t BYTE;
  struct
  {
    uint8_t GPIO3 :1;
    uint8_t GPIO2 :1;
    uint8_t GPIO1 :1;
    uint8_t GPIO0 :1;
    uint8_t GPIO4 :1;
    uint8_t GPIO5 :1;
    uint8_t GPIO6 :1;
    uint8_t GPIO7 :1;
  } BIT;
} gpio_t;

#define RINGBUF_SIZE 2048
#define RINGBUF_SIZE_MAX (RINGBUF_SIZE)
#define p_inc(p, max) (((p + 1) >= max) ? 0 : (p + 1))
extern uint16_t ringbuf[RINGBUF_SIZE];

void wavmp3p_put(void*, uint32_t);

#endif //__PLAY_MODES_H
