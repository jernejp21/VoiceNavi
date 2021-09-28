/**
 * Voice Navi Firmware
 *
 * MIT License
 *
 * Copyright (c) 2021 Azurtest
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * This file contains all globals that are used over multiple c files.
 * This is due to interrupt routines are in separate file than main function.
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "r_smc_entry.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"
#include "r_tfat_drv_if_dev.h"
#include "r_memdrv_rx_if.h"
#include "r_sci_iic_rx_if.h"
#include "r_cmt_rx_if.h"

#include "../NAND_flash.h"
#include "../wav.h"
#include "../play_modes.h"
#include "../Errors.h"

/** Macro definitions */
#define NAND_VOL_PAGE (2 * NAND_PAGE_SIZE)
#define NAND_DATA_PAGE (8 * NAND_PAGE_SIZE)
#define NAND_FILE_LIST_PAGE (3 * NAND_PAGE_SIZE)
#define NAND_PLAYLIST_PAGE (5 * NAND_PAGE_SIZE)

#define FILE_SIZE (2048)
#define NAND_CS PORTD.PODR.BIT.B4
#define NAND_CS_HIGH (NAND_CS = 1)
#define NAND_CS_LOW (NAND_CS = 0)

/** Enum definitions */
typedef enum modeSelect
{
  WAV_5A2 = 3,
  WAV_5F2 = 2,
  WAV_5F1 = 1,
  WAV_5F9IH = 0,
} modeSelect_t;

typedef struct system_status
{
  uint8_t flag_isPlaying;
  uint8_t flag_isIRQ;
  uint8_t song_cnt;
  uint8_t flag_isSongAvailable;
  uint8_t flag_waitForInterval;
  uint8_t flag_semaphoreLock;
} system_status_t;

/** Global functions */
modeSelect_t PIN_BoardSelection();

/** Global variables */
//extern uint16_t ringbuf[RINGBUF_SIZE];
extern uint8_t g_binary_vol_reduction;
extern uint32_t g_binary_vol_reduction_address;
extern system_status_t g_systemStatus;

#endif //__GLOBALS_H
