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
#include "r_rspi_rx_pinset.h"

#include "wav.h"

void main(void);
void placeNameToTable(char*, char*);
void eraseFlash();

#define FILE_SIZE (6400)

usb_ctrl_t ctrl;
usb_cfg_t cfg;
usb_err_t usb_err;
uint16_t event;
uint8_t g_drv_no;
char g_file_data1[FILE_SIZE];
char g_file_data2[FILE_SIZE];
static FATFS fs; /* File system object structure */
static FATFS fs1; /* File system object structure */
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
/* start of main func */

void main(void)
{
  printf("Entered main()\n");

  BYTE work[FF_MAX_SS];
  //eraseFlash();
  //fr = f_mount(&fs, "0:", 0);
  //fr = f_mkfs("1:", 0, work, sizeof(work));
  fr = f_mount(&fs1, "1:", 1);

  //flashStatus = R_FLASH_SPI_Open(0);

  //flashStatus = R_FLASH_SPI_Read_Status p_status)(0, &mem_data[0]);
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

static rspi_command_word_t my_rspi_command;

static rspi_err_t           my_rspi_err;
static rspi_handle_t        my_rspi_handle;
static rspi_chnl_settings_t my_rspi_setting;

static void my_rspi_callback(void *p_data);
#define NR_OF_BLOCK 2045
int transfer_busy;

void eraseFlash()
{
  uint8_t data[4];
  uint8_t status[3];

  my_rspi_command.cpha          = RSPI_SPCMD_CPHA_SAMPLE_EVEN;
  my_rspi_command.cpol          = RSPI_SPCMD_CPOL_IDLE_HI;
  my_rspi_command.br_div        = RSPI_SPCMD_BR_DIV_1;
  my_rspi_command.ssl_assert    = RSPI_SPCMD_ASSERT_SSL0;
  my_rspi_command.ssl_negate    = RSPI_SPCMD_SSL_KEEP;
  my_rspi_command.bit_length    = RSPI_SPCMD_BIT_LENGTH_8;
  my_rspi_command.bit_order     = RSPI_SPCMD_ORDER_MSB_FIRST;
  my_rspi_command.next_delay    = RSPI_SPCMD_NEXT_DLY_SSLND;
  my_rspi_command.ssl_neg_delay = RSPI_SPCMD_SSL_NEG_DLY_SSLND;
  my_rspi_command.clock_delay   = RSPI_SPCMD_CLK_DLY_SPCKD;
  my_rspi_command.dummy         = RSPI_SPCMD_DUMMY;

  my_rspi_setting.bps_target = 1000000;                    // Ask for 1Mbps bit-rate.
  my_rspi_setting.master_slave_mode = RSPI_MS_MODE_MASTER; // Configure the RSPI as SPI Master.
  my_rspi_setting.gpio_ssl = RSPI_IF_MODE_4WIRE;           // Set interface mode to 4-wire.

  my_rspi_err = R_RSPI_Open (0,                   // RSPI channel number
                             &my_rspi_setting,    // Address of the RSPI settings structure.
                             my_rspi_command,
                             &my_rspi_callback,   // Address of user-defined callback function.
                             &my_rspi_handle);    // Address of where the handle is to be stored

  if (RSPI_SUCCESS != my_rspi_err)
  {
      while(1); // Your error handling code would go here.
  }
  transfer_busy = 1;

  R_RSPI_PinSet_RSPI0();

  // Reset flash
    data[0] = 0xFF;
    my_rspi_err = R_RSPI_Write(my_rspi_handle, my_rspi_command, data, 1);
    while (transfer_busy)
    {
        R_BSP_NOP(); // Application could do something useful here while waiting for transfer to complete.
    }
    transfer_busy = 1;

  int tmp;
  for(int i = 0; i < NR_OF_BLOCK; i++)
  {
    // Write enable
    data[0] = 0x06;
    my_rspi_err = R_RSPI_Write(my_rspi_handle, my_rspi_command, data, 1);
    while (transfer_busy)
    {
        R_BSP_NOP(); // Application could do something useful here while waiting for transfer to complete.
    }
    transfer_busy = 1;

    do
    {
      data[0] = 0x0F;
      data[1] = 0xC0;
      data[2] = 0;
      my_rspi_err = R_RSPI_WriteRead(my_rspi_handle, my_rspi_command, &data[0], &status[0], 3);
      while (transfer_busy)
      {
          R_BSP_NOP(); // Application could do something useful here while waiting for transfer to complete.
      }
      transfer_busy = 1;
    }while(0 == (status[2] & 2));

    // Bit lock disable
    data[0] = 0x1F;
    data[1] = 0xA0;
    data[2] = 0x00;
    my_rspi_err = R_RSPI_Write(my_rspi_handle, my_rspi_command, data, 3);
    while (transfer_busy)
    {
        R_BSP_NOP(); // Application could do something useful here while waiting for transfer to complete.
    }
    transfer_busy = 1;

    do
    {
      data[0] = 0x0F;
      data[1] = 0xC0;
      data[2] = 0;
      my_rspi_err = R_RSPI_WriteRead(my_rspi_handle, my_rspi_command, &data[0], &status[0], 3);
      while (transfer_busy)
      {
          R_BSP_NOP(); // Application could do something useful here while waiting for transfer to complete.
      }
      transfer_busy = 1;
    }while(0 != (status[2] & 1));

    //Erase block
    tmp = i <<6;
    data[0] = 0xD8;
    data[1] = *(((uint8_t*)&tmp)+2);
    data[2] = *(((uint8_t*)&tmp)+1);
    data[3] = *((uint8_t*)&tmp);
    my_rspi_err = R_RSPI_Write(my_rspi_handle, my_rspi_command, data, 4);

    while (transfer_busy)
    {
        R_BSP_NOP(); // Application could do something useful here while waiting for transfer to complete.
    }
    transfer_busy = 1;

    do
    {
      data[0] = 0x0F;
      data[1] = 0xC0;
      data[2] = 0;
      my_rspi_err = R_RSPI_WriteRead(my_rspi_handle, my_rspi_command, &data[0], &status[0], 3);
      while (transfer_busy)
      {
          R_BSP_NOP(); // Application could do something useful here while waiting for transfer to complete.
      }
      transfer_busy = 1;
    }while(0 != (status[2] & 0x05));

  }
  R_RSPI_Close(my_rspi_handle);
}

/**********************************************************************************************************************
* Function Name: my_rspi_callback
* Description  : This is an example of an RSPI callback function.
* Arguments    : p_data - pointer to RSPI event code data.
* Return Value : None
**********************************************************************************************************************/
static void my_rspi_callback(void *p_data)
{
  transfer_busy = 0;

}
/* End of function my_rspi_callback. */
