/***********************************************************************
 *
 *  FILE        : Demo.c
 *  DATE        : 2021-07-02
 *  DESCRIPTION : Main Program
 *
 *  NOTE:THIS IS A TYPICAL EXAMPLE.
 *
 ***********************************************************************/
#include "stdlib.h"
#include "r_smc_entry.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"
#include "r_usb_basic_pinset.h"
#include "r_tfat_drv_if_dev.h"
//#include "r_tfat_driver_rx_config.h"
//#include "r_tfat_driver_rx_if.h"
#include "r_flash_rx_if.h"

#include "wav.h"

void main(void);


#define FILE_SIZE (FLASH_DF_BLOCK_SIZE)

usb_ctrl_t ctrl;
usb_cfg_t cfg;
usb_err_t usb_err;
uint16_t event;
uint8_t g_drv_no;
static char g_file_data[FILE_SIZE];
static FATFS fs; /* File system object structure */
const static uint8_t g_msc_file[9] = "0:002.wav";
FIL file;
UINT file_size;
FRESULT fr;
DSTATUS ds;
flash_err_t flashErr;

FILINFO fileinfo;
uint16_t counter = 0;
uint32_t filePointer;

uint8_t *add_p = (uint8_t*)FLASH_DF_BLOCK_0;

wav_header_t waf_file;

int* arr;

void main(void) {
	printf("Entered main()\n");

	PORTD.PDR.BIT.B6 = 1;
	PORTD.PDR.BIT.B7 = 1;

	arr = (int*)malloc(11*4);
	arr[0] = 10;
	arr[10] = 8;
	//arr[150] = 18;

	/*WAV_Open(&waf_file, (uint8_t*)FLASH_DF_BLOCK_0);

	R_Config_DA1_Start();
	R_Config_TMR0_TMR1_Start();
	R_USB_PinSet_USB0_HOST();

	ctrl.module = USB_IP0;
	ctrl.type = USB_HMSC;
	cfg.usb_speed = USB_FS;
	cfg.usb_mode = USB_HOST;
	usb_err = R_USB_Open(&ctrl, &cfg);*/

	//flashErr = R_FLASH_Open();

	while (1) {
		//event = R_USB_GetEvent(&ctrl); // Get event code

		switch (event) {
		case USB_STS_CONFIGURED:

			printf("Detected attached USB memory.\n");
			usb_err = R_USB_HmscGetDriveNo(&ctrl, &g_drv_no);

			//ds = usb_disk_initialize(g_drv_no);
			fr = f_mount(&fs, "", 0); /* Create a file object. */
			fr = f_open(&file, (const TCHAR*)&g_msc_file, FA_READ);
			f_stat((const TCHAR*)g_msc_file, &fileinfo);

			//R_FLASH_Erase(FLASH_DF_BLOCK_0, 512);

			/*while(add < (FLASH_DF_BLOCK_511 + FLASH_DF_BLOCK_SIZE))
			{
				f_read(&file, g_file_data, sizeof(g_file_data), &file_size);
				if(file_size == 0)
				{
					break;
				}
				//R_FLASH_Write((uint32_t)&g_file_data, add, FILE_SIZE);
				add += FLASH_DF_BLOCK_SIZE;
			}*/
			//fr = f_write(&file, g_file_data, sizeof(g_file_data), &file_size);
			//fr = f_read(&file, &g_file_data, sizeof(g_file_data), &file_size);
			//f_close(&file); /* Close the file object. */
			//f_unmount("0:");

			//R_Config_TMR0_TMR1_Start();

			break;

		case USB_STS_DETACH:
			printf("Detected detached USB memory.\n");
			R_Config_TMR0_TMR1_Stop();
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

#pragma interrupt (Timer_IRQ(vect=_VECT( _TMR0_CMIA0 ), enable))   /* IRQ1: Enable the nested interrupt */
static void Timer_IRQ(void)
{
	DA.DADR1 = *add_p;
	add_p++;

	if(add_p > (uint8_t*)(FLASH_DF_BLOCK_430 + FLASH_DF_BLOCK_SIZE))
	{
		add_p = (uint8_t*)FLASH_DF_BLOCK_0;
	}
	/*DA.DADR1 = g_file_data[counter];
	if(counter < FILE_SIZE)
	{
		counter++;
	}
	else
	{
		//TMR0.TCR.BIT.CMIEA = 0;  //Disable interrupt for compare match A
		counter=0;
		filePointer += FILE_SIZE;
		f_read(&file, &g_file_data, sizeof(g_file_data), &file_size);
		//TMR0.TCR.BIT.CMIEA = 1;  //Enable interrupt for compare match A
	}

	if(filePointer > fileinfo.fsize)
	{
		R_Config_TMR0_TMR1_Stop();  //Disable interrupt for compare match A
		f_close(&file); // Close the file object.
	}*/
}
