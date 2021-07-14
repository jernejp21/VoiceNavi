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

#include "globals.h"
#include "wav.h"

void main(void);

int g_counter;
int g_readBuffer;

char line[100];
int startOfFileNames;
int startOfPlaylist;

static FATFS fs; /* File system object structure */
const static uint8_t g_msc_file[14] = "0:sample02.wpj";
FIL file;
UINT file_size;
FRESULT fr;
uint8_t g_file_data[FILE_SIZE];

file_name_pos_t file_names[255];
playlist_t output_music[255];

wav_header_t g_wav_file;

void main(void)
{
  char flash_data = 0;
  usb_ctrl_t ctrl;
  usb_cfg_t cfg;
  usb_err_t usb_err;
  uint16_t event;

  // CPU init (cloks, RAM, etc.) and peripheral init is done in
  // resetpgr.c in PowerON_Reset_PC function.

  R_DAC1_Start();
  R_TMR01_Set_Frequency(1, 1);  //Set freq to 1 kHz for counter B
  R_TMR01_Start();
  R_TMR01_Stop_A();

  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS;
  cfg.usb_mode = USB_HOST;
  usb_err = R_USB_Open(&ctrl, &cfg);

  /* With polling, check for 1s if USB is connected or not */
  while(g_counter < 1000)
  {
    event = R_USB_GetEvent(&ctrl);  // Get event code

    switch(event)
    {
      case USB_STS_CONFIGURED:

        PORTD.PDR.BIT.B6 = 1;
        PORTD.PDR.BIT.B7 = 0;
        R_TMR01_Stop();
        printf("Detected attached USB memory.\n");
        g_counter = 1000;
        break;

      case USB_STS_DETACH:
        printf("Detected detached USB memory.\n");
        //R_TMR01_Stop();
        break;

      case USB_STS_NOT_SUPPORT:
        printf("USB not supported.\n");
        break;

      case USB_STS_NONE:
        //printf("STS_NONE.\n");
        PORTD.PDR.BIT.B6 = 0;
        PORTD.PDR.BIT.B7 = 1;

        break;

      default:
        break;
    }
  }

  /* Check if there is any data in flash. */
  //flash_data = check_flash_data()
  flash_data = 1;

  /* If data in flash, prepare lookout tables. */
  if(flash_data == 1)
  {
    fr = f_mount(&fs, "", 0);
    fr = f_open(&file, (const TCHAR*) &g_msc_file, FA_READ);
    while(f_gets((TCHAR*) &line, sizeof(line), &file))
    {
      /* Section #2 in wpj file represents file name order.
       * 0,file1.wav
       * 1,file50.wav
       * 2,file8.wav 
       */
      if(strncmp(line, "#2", 2) == 0)
      {
        startOfFileNames = 1;
        g_counter = 0;
        continue;
      }

      /* Section #3 in wpj file represents playlist.
       * input channel, output channel, repetition, file to play (up to 8 files)
       * 0,1,1,2
       * 1,1,1,2
       * 2,1,2,0
       */
      if(strncmp(line, "#3", 2) == 0)
      {
        startOfPlaylist = 1;
        startOfFileNames = 0;
        g_counter = 0;
        continue;
      }

      if(startOfFileNames)
      {
        placeNameToTable((char*) &file_names[g_counter].file_name, (char*) &line);
        g_counter++;
      }

      if(startOfPlaylist)
      {
        placeSongsToTable((playlist_t*) &output_music[g_counter], (char*) &line);
        g_counter++;
      }

      printf(line);
    }
    f_close(&file);

  }
  else
  {
    while(1);  // No data in flash, look forever here.
  }

  f_open(&file, "0808000m.wav", FA_READ);
  f_read(&file, &g_file_data, sizeof(g_file_data), &file_size);
  WAV_Open(&g_wav_file, &g_file_data[0]);
  R_TMR01_Set_Frequency(g_wav_file.sample_rate, 0);
  R_TMR01_Start_A();
  R_TMR01_Start();

  while(1)
  {
    if(g_readBuffer)
    {
      g_readBuffer = 0;
      PORTD.PDR.BIT.B6 ^= 1;
      f_read(&file, &g_file_data, sizeof(g_file_data), &file_size);
      //PORTD.PDR.BIT.B6 = 0;
    }
  }

}
