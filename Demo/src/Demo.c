/***********************************************************************
 *
 *  FILE        : Demo.c
 *  DATE        : 2021-07-02
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
//#include "r_tfat_driver_rx_config.h"
#include "r_tfat_driver_rx_if.h"
#include "r_flash_rx_if.h"
#include "sbrk.h"

#include "wav.h"

void main(void);
void placeNameToTable(char*, char*);

#define FILE_SIZE (FLASH_DF_BLOCK_SIZE)

usb_ctrl_t ctrl;
usb_cfg_t cfg;
usb_err_t usb_err;
uint16_t event;
uint8_t g_drv_no;
static char g_file_data[FILE_SIZE];
static FATFS fs; /* File system object structure */
const static uint8_t g_msc_file[14] = "0:sample02.wpj";
FIL file;
UINT file_size;
FRESULT fr;
DSTATUS ds;
flash_err_t flashErr;
TCHAR *tchar;

FILINFO fileinfo;
uint16_t counter = 0;
uint32_t filePointer;

uint8_t *add_p = (uint8_t*) FLASH_DF_BLOCK_0;

wav_header_t waf_file;

int *arr;

char line[100];
int startOfFileNames;
int startOfOutputMusic;

#define NAME_LEN 8
typedef struct file_name_pos
{
  char file_name[NAME_LEN];
} file_name_pos_t;

typedef struct output_struct
{
  uint8_t group;
  uint8_t repeat;
  uint8_t file_nr[8];
} output_struct_t;

file_name_pos_t *file_names;
output_struct_t *output_music;

void placeSongsToTable(output_struct_t*, char*);

void main(void)
{
  printf("Entered main()\n");

  PORTD.PDR.BIT.B6 = 1;
  PORTD.PDR.BIT.B7 = 1;

  /*arr = (int*) sbrk(10 * 4);
   arr[0] = 5;
   arr[5] = 8;
   arr[9] = 11;

   sbrk(-10 * 4);  //free allocated memory

   arr = (int*) malloc(11 * 4);
   arr[0] = 10;
   arr[10] = 8;
   //arr[150] = 18;*/

  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS;
  cfg.usb_mode = USB_HOST;
  usb_err = R_USB_Open(&ctrl, &cfg);

  //flashErr = R_FLASH_Open();
  while(1)
  {
    event = R_USB_GetEvent(&ctrl);  // Get event code

    switch(event)
    {
      case USB_STS_CONFIGURED:

        printf("Detected attached USB memory.\n");
        usb_err = R_USB_HmscGetDriveNo(&ctrl, &g_drv_no);

        //ds = usb_disk_initialize(g_drv_no);
        //tchar = f_gets((TCHAR*)&line, sizeof(line), &file);
        //f_putc(line, &file);
        fr = f_mount(&fs, "", 0); /* Create a file object. */
        fr = f_open(&file, (const TCHAR*) &g_msc_file, FA_READ);
        f_stat((const TCHAR*) g_msc_file, &fileinfo);

        while(f_gets((TCHAR*) &line, sizeof(line), &file))
        {
          if(strncmp(line, "#2", 2) == 0)
          {
            file_names = (file_name_pos_t*) sbrk(sizeof(file_name_pos_t));  //set heap address
            sbrk(-sizeof(file_name_pos_t));  //clear set heap address, because we increase heap in next step
            startOfFileNames = 1;
            continue;
          }

          if(strncmp(line, "#3", 2) == 0)
          {
            output_music = (output_struct_t*) sbrk(sizeof(output_music));  //set heap address
            sbrk(-sizeof(output_music));  //clear set heap address, because we increase heap in next step
            startOfOutputMusic = 1;
            startOfFileNames = 0;
            counter = 0;
            continue;
          }

          if(startOfFileNames)
          {
            if((int) sbrk(sizeof(file_name_pos_t)) != -1)
            {
              placeNameToTable((char*) &file_names[counter].file_name, (char*) &line);
              //wav_names[counter].file_name[0] = line[0];
              counter++;
            }
          }

          if(startOfOutputMusic)
          {
            if((int) sbrk(sizeof(output_music)) != -1)
            {
              placeSongsToTable((output_struct_t*) &output_music[counter], (char*) &line);
              //wav_names[counter].file_name[0] = line[0];
              counter++;
            }
          }

          printf(line);
        }

        /*R_FLASH_Erase(FLASH_DF_BLOCK_0, 512);

         while(add_p < (uint8_t*)(FLASH_DF_BLOCK_511 + FLASH_DF_BLOCK_SIZE))
         {
         f_read(&file, g_file_data, sizeof(g_file_data), &file_size);
         if(file_size == 0)
         {
         break;
         }
         R_FLASH_Write((uint32_t) &g_file_data, (uint32_t)add_p, FILE_SIZE);
         add_p += FLASH_DF_BLOCK_SIZE;
         }*/
        //fr = f_write(&file, g_file_data, sizeof(g_file_data), &file_size);
        //fr = f_read(&file, &g_file_data, sizeof(g_file_data), &file_size);
        f_close(&file);  // Close the file object.
        //f_unmount("0:");
        //R_Config_TMR0_TMR1_Start();
        break;

      case USB_STS_DETACH:
        printf("Detected detached USB memory.\n");
        break;

      case USB_STS_NOT_SUPPORT:
        printf("USB not supported.\n");
        break;

      case USB_STS_NONE:
        //printf("nekaj\n");

        break;

      default:
        break;
    }
  }
}

void placeNameToTable(char *dest, char *source)
{
  uint8_t startCopy = 0;

  for(;; source++)
  {
    if(('\r' == *source) || ('\n' == *source))
    {
      return;
    }

    if(',' == *source)
    {
      startCopy = 1;
      continue;
    }

    if(startCopy)
    {
      *dest = *source;
      dest++;
    }
  }
}

void placeSongsToTable(output_struct_t *dest, char *source)
{
  output_struct_t *output = dest;
  int commaCount = 0;
  uint8_t decimal = 1;
  uint8_t file_nr = 0;

  for(;; source++)
  {
    if(('\r' == *source) || ('\n' == *source))
    {
      return;
    }

    if(',' == *source)
    {
      commaCount++;
      decimal = 1;
      if(commaCount > 3)
      {
        file_nr++;
      }
      continue;
    }

    switch(commaCount)
    {
      case 0:
        break;

      case 1:
        output->group = (uint8_t) *source - '0';
        break;

      case 2:
        output->repeat = (output->repeat * decimal) + ((uint8_t) *source - '0');
        decimal = 10;
        break;

      default:
        output->file_nr[file_nr] = (output->file_nr[file_nr] * decimal) + ((uint8_t) *source - '0');
        decimal = 10;
        break;
    }
  }
}

