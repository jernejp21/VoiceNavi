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
#include "r_cmt_rx_if.h"

#include "../NAND_flash.h"
#include "../wav.h"
#include "../play_modes.h"
#include "../Errors.h"

#define FILE_SIZE (2048)

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

#define NAND_CS PORTD.PODR.BIT.B4
#define NAND_CS_HIGH (NAND_CS = 1)
#define NAND_CS_LOW (NAND_CS = 0)

#define I2C_GPIO_ADDR 0x21
#define I2C_GPIO_READ ((I2C_GPIO_ADDR << 1) | 0x01)
#define I2C_GPIO_WRITE ((I2C_GPIO_ADDR << 1) | 0x00)

#define I2C_POTENT_ADDR 0x28
#define I2C_POTENT_READ ((I2C_POTENT_ADDR << 1) | 0x01)
#define I2C_POTENT_WRITE ((I2C_POTENT_ADDR << 1) | 0x00)
void callBack_read();
void callBack_write();
void I2C_Send(uint8_t reg_add, uint8_t value);
uint8_t I2C_Receive(uint8_t reg_add);
void I2C_Init();
void I2C_Periodic();
uint8_t g_i2c_gpio_rx[2];
uint8_t g_i2c_gpio_tx[2];

void playFromPlaylist(uint8_t);

typedef enum modeSelect
{
  WAV_5A2 = 3,
  WAV_5F2 = 2,
  WAV_5F1 = 1,
  WAV_5F9IH = 0,
}modeSelect_t;
modeSelect_t PIN_BoardSelection();

extern uint16_t g_volume[2];

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
