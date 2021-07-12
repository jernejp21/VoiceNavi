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

int counter;
char line[100];
int startOfFileNames;
int startOfPlaylist;

static FATFS fs; /* File system object structure */
const static uint8_t g_msc_file[14] = "0:sample02.wpj";
FIL file;
UINT file_size;
FRESULT fr;

file_name_pos_t file_names[255];
playlist_t output_music[255];

void main(void)
{
  char flash_data = 1;
  usb_ctrl_t ctrl;
  usb_cfg_t cfg;
  usb_err_t usb_err;
  uint16_t event;

  // CPU init (cloks, RAM, etc.) and peripheral init is done in
  // resetpgr.c in PowerON_Reset_PC function.

  //PORTD.PDR.BIT.B6 = 1;
  R_Config_TMR0_TMR1_Set_Frequency(1);  //Set freq to 1 kHz
  R_Config_TMR0_TMR1_Start();

  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS;
  cfg.usb_mode = USB_HOST;
  usb_err = R_USB_Open(&ctrl, &cfg);

  /* With polling, check for 1s if USB is connected or not */
  while(counter < 1000)
  {
    event = R_USB_GetEvent(&ctrl);  // Get event code

    switch(event)
    {
      case USB_STS_CONFIGURED:

        PORTD.PDR.BIT.B6 = 1;
        PORTD.PDR.BIT.B7 = 0;
        R_Config_TMR0_TMR1_Stop();
        printf("Detected attached USB memory.\n");
        fr = f_mount(&fs, "", 0); /* Create a file object. */
        fr = f_open(&file, (const TCHAR*) &g_msc_file, FA_READ);
        counter = 1000;
        break;

      case USB_STS_DETACH:
        printf("Detected detached USB memory.\n");
        //R_Config_TMR0_TMR1_Stop();
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
  /* If data in flash, prepare lookout tables. */
  if(flash_data == 1)
  {
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
        counter = 0;
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
        counter = 0;
        continue;
      }

      if(startOfFileNames)
      {
        placeNameToTable((char*) &file_names[counter].file_name, (char*) &line);
        counter++;
      }

      if(startOfPlaylist)
      {
        placeSongsToTable((playlist_t*) &output_music[counter], (char*) &line);
        counter++;
      }

      printf(line);
    }

  }
  else
  {

  }

  while(1)
  {

  }

}
