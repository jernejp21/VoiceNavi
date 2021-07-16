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
#include "r_flash_spi_if.h"
#include "r_memdrv_rx_if.h"
#include "r_rspi_rx_if.h"

#include "wav.h"

void main(void);
void placeNameToTable(char*, char*);

#define FILE_SIZE (6400)

usb_ctrl_t ctrl;
usb_cfg_t cfg;
usb_err_t usb_err;
uint16_t event;
uint8_t g_drv_no;
char g_file_data1[FILE_SIZE];
char g_file_data2[FILE_SIZE];
static FATFS fs; /* File system object structure */
const static uint8_t g_msc_file[14] = "0:1644100m.wav";
FIL file;
UINT file_size;
FRESULT fr;
DSTATUS ds;
TCHAR *tchar;

FILINFO fileinfo;
uint16_t counter = 0;
uint32_t filePointer;

//uint8_t *add_p = (uint8_t*) FLASH_DF_BLOCK_0;

wav_header_t wav_file;

int *arr;

char line[100];
int startOfFileNames;
int startOfOutputMusic;

int g_readBuffer1 = 0;
int g_readBuffer = 0;

#define NAME_LEN 11
typedef struct file_name_pos
{
  char file_name[NAME_LEN + 1];
} file_name_pos_t;

typedef struct output_struct
{
  uint8_t group;
  uint8_t repeat;
  uint8_t file_nr[8];
  uint8_t playlist_len;
} output_struct_t;

file_name_pos_t file_names[255];
output_struct_t output_music[255];

void placeSongsToTable(output_struct_t*, char*);
void playFromPlaylist(uint8_t);

int g_isUSBRead = 0;

flash_spi_status_t flashStatus;

memdrv_err_t memdrv_err;
st_memdrv_info_t memdrv_info;
uint8_t mem_data[4] = {0x9F, 0x00, 0, 0};

static rspi_command_word_t my_rspi_command;

/* start of main func */

void main(void)
{
  printf("Entered main()\n");

  my_rspi_command.cpha = RSPI_SPCMD_CPHA_SAMPLE_EVEN;
  my_rspi_command.cpol = RSPI_SPCMD_CPOL_IDLE_HI;
  my_rspi_command.br_div = RSPI_SPCMD_BR_DIV_1;
  my_rspi_command.ssl_assert = RSPI_SPCMD_ASSERT_SSL0;
  my_rspi_command.ssl_negate = RSPI_SPCMD_SSL_KEEP;
  my_rspi_command.bit_length = RSPI_SPCMD_BIT_LENGTH_8;
  my_rspi_command.bit_order = RSPI_SPCMD_ORDER_MSB_FIRST;
  my_rspi_command.next_delay = RSPI_SPCMD_NEXT_DLY_SSLND;
  my_rspi_command.ssl_neg_delay = RSPI_SPCMD_SSL_NEG_DLY_SSLND;
  my_rspi_command.clock_delay = RSPI_SPCMD_CLK_DLY_SPCKD;
  my_rspi_command.dummy = RSPI_SPCMD_DUMMY;

  flashStatus = R_FLASH_SPI_Open(0);

  flashStatus = R_FLASH_SPI_Read_ID(0, &mem_data[0]);
  /*ctrl.module = USB_IP0;
   ctrl.type = USB_HMSC;
   cfg.usb_speed = USB_FS;
   cfg.usb_mode = USB_HOST;
   usb_err = R_USB_Open(&ctrl, &cfg);*/
  while(1)
  {
    /*while(!g_isUSBRead)
     {
     event = R_USB_GetEvent(&ctrl);  // Get event code

     switch(event)
     {
     case USB_STS_CONFIGURED:

     printf("Detected attached USB memory.\n");
     usb_err = R_USB_HmscGetDriveNo(&ctrl, &g_drv_no);

     fr = f_mount(&fs, "0:", 0); // Create a file object.
     //playFromPlaylist(6);
     fr = f_open(&file, (const TCHAR*) &g_msc_file, (FA_OPEN_ALWAYS | FA_READ));
     f_stat((const TCHAR*) g_msc_file, &fileinfo);
     f_read(&file, &g_file_data1, 44, &file_size);
     //f_read(&file, &g_file_data, sizeof(g_file_data), &file_size);

     f_close(&file);  // Close the file object.

     f_unmount("0:");

     WAV_Open(&wav_file, (uint8_t*) &g_file_data1);
     f_read(&file, &g_file_data1, sizeof(g_file_data1), &file_size);
     R_Config_TMR0_TMR1_Set_Frequency(wav_file.sample_rate);
     R_Config_TMR0_TMR1_Start();
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
     }*/

  }
}

