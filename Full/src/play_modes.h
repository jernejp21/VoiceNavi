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

void DIP_Init();
uint8_t DIP_ReadState();

#endif //__PLAY_MODES_H
