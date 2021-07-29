/***********************************************************************
 *
 *  FILE        : Full.c
 *  DATE        : 2021-07-08
 *  DESCRIPTION : Main Program
 *
 *  NOTE:THIS IS A TYPICAL EXAMPLE.
 *
 ***********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "r_smc_entry.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"
#include "r_tfat_drv_if_dev.h"
#include "r_memdrv_rx_if.h"

#include "globals.h"
#include "wav.h"
#include "NAND_flash.h"

void main(void);
void playFromPlaylist(uint8_t);

int g_counter;
int g_readBuffer;
uint32_t g_file_size;
uint32_t g_current_byte;
int g_playing;

char line[100];
int startOfFileNames;
int startOfPlaylist;

static FATFS fs; /* File system object structure */
const static uint8_t g_msc_file[14] = "0:sample02.wpj";
FIL file;
FRESULT fr;
uint8_t g_file_data[FILE_SIZE];

file_meta_data_t g_file_meta_data[255];
playlist_t g_output_music[255];
flash_custom_FAT_t flash_table[255];

uint8_t spi_tx_buff[4];
uint8_t spi_rx_buff[4];

wav_header_t g_wav_file;

void main(void)
{
  char is_data_in_flash = 0;
  usb_ctrl_t ctrl;
  usb_cfg_t cfg;
  usb_err_t usb_err;
  uint16_t event;
  st_memdrv_info_t memdrv_info;
  nand_flash_status_t nand_status;

  PORTE.PDR.BYTE = 0;
  PORTE.PCR.BYTE = 0xFF;

  PORTD.PDR.BIT.B6 = 1;
  PORTD.PDR.BIT.B7 = 1;

  //SPI CS Pin
  PORTA.PDR.BIT.B4 = 1; //Set pin as output
  PORTA.PODR.BIT.B4 = 1; //Set pin HIGH

  // CPU init (cloks, RAM, etc.) and peripheral init is done in
  // resetpgr.c in PowerON_Reset_PC function.

  FIFO_Init();
  R_DAC1_Start();
  R_DAC1_Set_ConversionValue(0x800);  //This is to avoid popping sound at the start
  R_TMR23_Start();

  //Init USB Host Mass Storage Device controller
  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS;
  cfg.usb_mode = USB_HOST;
  usb_err = R_USB_Open(&ctrl, &cfg);

  //Init SPI for NAND Flash
  memdrv_info.cnt = 0;
  memdrv_info.p_data = NULL;
  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  R_MEMDRV_Open(0, &memdrv_info);

  /* With polling, check for 1s if USB is connected or not */
  while(g_counter < 1000)
  {
    event = R_USB_GetEvent(&ctrl);  // Get event code

    switch(event)
    {
      case USB_STS_CONFIGURED:

        R_TMR23_Stop();
        g_counter = 1000;
        //nand_status = NAND_Erase();
        //NAND_CopyToFlash();
        is_data_in_flash = 1;
        break;

      case USB_STS_DETACH:
        break;

      case USB_STS_NOT_SUPPORT:
        break;

      case USB_STS_NONE:
        break;

      default:
        break;
    }
  }
  R_TMR23_Stop();
  g_counter = 0;

  /* Check if there is any data in flash. */
  //is_data_in_flash = check_if_data_in_flash()
  /* If data in flash, prepare lookout tables. */
  if(is_data_in_flash == 1)
  {
    NAND_ReadFromFlash(0, sizeof(flash_table), (uint8_t*)&flash_table[0]);
    NAND_ReadFromFlash(NAND_PAGE_SIZE+10, sizeof(g_output_music), (uint8_t*)&g_output_music[0]);
  }
  else
  {
    while(1);  // No data in flash, look forever here.
  }

  while(1)
  {
    uint8_t xor = 5; //PORTE.PIDR.BYTE ^ 0xFF;  //Unset bit will be 1, other bits will be 0.
    int counter = -1;

    while(xor)
    {
      xor = xor >> 1;
      counter++;
    }

    if(counter != -1)
    {
      playFromPlaylist((uint8_t) counter);
    }
  }

}

int g_playing = 0;

void playFromPlaylist(uint8_t playNr)
{
  int index = 0;
  int fileToPlay;
  UINT size;
  uint32_t fileAddress;
  int trackNr = 0;
  int repetitions = 0;
  int fifo_status = FIFO_OK;
  playNr = 155;

  while(g_output_music[playNr].repeat > repetitions)
  {
    while(g_output_music[playNr].playlist_len > trackNr)
    {
      fileToPlay = g_output_music[playNr].file_nr[index];
      fileAddress = flash_table[fileToPlay].address;

      //fr = f_open(&file, fileName_p, FA_READ);
      //fr = f_read(&file, &g_file_data, WAV_HEADER_SIZE, &size);
      NAND_ReadFromFlash(fileAddress, WAV_HEADER_SIZE, g_file_data);

      WAV_Open(&g_wav_file, g_file_data);
      fileAddress += WAV_HEADER_SIZE;
      NAND_ReadFromFlash(fileAddress, sizeof(g_file_data), g_file_data);
      fifo_status = FIFO_Write(g_file_data, sizeof(g_file_data));
      g_counter = 0;

      g_playing = 1;
      R_TMR01_Set_Frequency(g_wav_file.sample_rate);

      R_TMR01_Start();
      while(g_playing)
      {
        if(g_readBuffer)
        {
          g_readBuffer = 0;
          fileAddress += sizeof(g_file_data);
          PORTD.PODR.BIT.B7 = 1;
          NAND_ReadFromFlash(fileAddress, sizeof(g_file_data), g_file_data);
          fifo_status = FIFO_Write(g_file_data, sizeof(g_file_data));
          if(fifo_status == FIFO_FULL)
          {
            __asm("nop");
          }
          PORTD.PODR.BIT.B7 = 0;
          //f_read(&file, &g_file_data, sizeof(g_file_data), &size);
        }
      }
      R_TMR01_Stop();
      //f_close(&file);

      trackNr++;
      index++;
    }

    trackNr = 0;
    index = 0;
    repetitions++;
  }
}

uint8_t FIFO_buffer[FIFO_SIZE];
uint16_t FIFO_head, FIFO_tail;



void FIFO_Init(void)
{
  FIFO_head = 0;
  FIFO_tail = 0;
}

int FIFO_Write(uint8_t *new, uint16_t size)
{
  for(int i = 0; i < size; i++)
  {
    if(FIFO_head == ((FIFO_tail - 1 + FIFO_SIZE) % FIFO_SIZE))
    {
      return FIFO_FULL; /* Queue Full*/
    }

    FIFO_buffer[FIFO_head] = *(new + i);

    FIFO_head = (FIFO_head + 1) % FIFO_SIZE;
  }

  return FIFO_OK;  // No errors
}

int FIFO_Read(uint8_t *old, uint16_t size)
{
  for(int i = 0; i < size; i++)
  {
    if(FIFO_head == FIFO_tail)
    {
      return FIFO_EMPTY; /* Queue Empty - nothing to get*/
    }

    *(old + i) = FIFO_buffer[FIFO_tail];

    FIFO_tail = (FIFO_tail + 1) % FIFO_SIZE;
  }

  return FIFO_OK;  // No errors
}
