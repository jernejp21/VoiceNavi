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
#include "sbrk.h"

#include "r_smc_entry.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"

#include "globals.h"

void main(void);

int counter;
int* arr;
int* arr1;

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

  /* Check if there is any data in flash */
  if(flash_data == 1)
  {


  }
  else
  {

  }

  while(1)
  {

  }

}
