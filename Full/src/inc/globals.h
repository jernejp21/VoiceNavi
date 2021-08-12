/*
 * This file contains all globals that are used over multiple c files.
 * This is due to interrupt routines are in seperate file than main function.
 *
 * Author: Jernej Pangerc
 * Created on (DD.MM.YYYY): 8.7.2021
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

#include "../NAND_flash.h"
#include "../wav.h"
#include "../play_modes.h"

#define FILE_SIZE (3000)

#define FIFO_ELEMENTS 6000
#define FIFO_SIZE (FIFO_ELEMENTS + 1)
extern uint8_t FIFO_buffer[FIFO_SIZE];
extern uint16_t FIFO_head, FIFO_tail;
void FIFO_Init(void);
int FIFO_Write(uint8_t*, uint16_t);
int FIFO_Read(uint8_t*, uint16_t);
typedef enum fifo
{
  FIFO_OK = 0,
  FIFO_EMPTY = 1,
  FIFO_FULL = 2,
}fifo_t;

#define I2C_ADDR 0x21
#define I2C_READ ((I2C_ADDR << 1) | 0x01)
#define I2C_WRITE ((I2C_ADDR << 1) | 0x00)
void callBack_read();
void callBack_write();
void I2C_Send(uint8_t reg_add, uint8_t value);
uint8_t I2C_Receive(uint8_t reg_add);
void I2C_Init();

void playFromPlaylist(uint8_t);

void LED_BusyOn();
void LED_BusyOff();
void LED_Init();
void LED_Toggle();

extern int g_counter;
extern int g_readBuffer;
extern uint8_t g_file_data[FILE_SIZE];
extern wav_header_t g_wav_file;
extern uint32_t g_file_size;
extern uint32_t g_current_byte;
extern int g_playing;
extern playlist_t g_output_music[255];
extern file_meta_data_t g_file_meta_data[255];
extern uint8_t g_isIRQ;
extern int g_stopPlaying;

#endif //__GLOBALS_H
