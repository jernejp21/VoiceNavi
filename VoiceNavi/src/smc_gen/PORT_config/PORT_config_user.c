/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : PORT_config_user.c
* Version      : 2.1.3
* Device(s)    : R5F565N9FxFP
* Description  : This file implements device driver for PORT_config.
* Creation Date: 2021-09-06
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "PORT_config.h"
/* Start user code for include. Do not edit comment generated here */
#include "globals.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_PORT_config_Create_UserInit
* Description  : This function adds user code after initializing PORT
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_PORT_config_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
  LED_PowOff();
  LED_BusyOff();
  LED_USBOff();
  LED_AlarmOff();
  PIN_BusySet();
  PIN_AlarmSet();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
void LED_BusyOn()
{
  PORT5.PODR.BIT.B2 = 0;
}

void LED_BusyOff()
{
  PORT5.PODR.BIT.B2 = 1;
}

void LED_BusyToggle()
{
  PORT5.PODR.BIT.B2 ^= 1;
}

void LED_PowOn()
{
  PORT5.PODR.BIT.B3 = 0;
}

void LED_PowOff()
{
  PORT5.PODR.BIT.B3 = 1;
}

void LED_USBOn()
{
  PORT5.PODR.BIT.B1 = 0;
}

void LED_USBOff()
{
  PORT5.PODR.BIT.B1 = 1;
}

void LED_USBToggle()
{
  PORT5.PODR.BIT.B1 ^= 1;
}

void LED_AlarmOn()
{
  PORT5.PODR.BIT.B4 = 0;
}

void LED_AlarmOff()
{
  PORT5.PODR.BIT.B4 = 1;
}

void LED_Toggle()
{
  PORT5.PODR.BIT.B3 ^= 1;
}

uint8_t DIP_ReadState()
{
  return PORTA.PIDR.BYTE;
}

modeSelect_t PIN_BoardSelection()
{
  modeSelect_t ret;
  ret = PORT4.PIDR.BIT.B2; //MS1
  ret |= (1 << PORT4.PIDR.BIT.B3); //MS2

  return ret;
}

void PIN_ShutdownSet()
{
  PORT5.PODR.BIT.B5 = 1;
}

void PIN_ShutdownReset()
{
  PORT5.PODR.BIT.B5 = 0;
}

void PIN_RstSet()
{
  PORT5.PODR.BIT.B0 = 1;
}

void PIN_RstReset()
{
  PORT5.PODR.BIT.B0 = 0;
}

void PIN_BusySet()
{
  PORTC.PODR.BIT.B5 = 1;
}

void PIN_BusyReset()
{
  PORTC.PODR.BIT.B5 = 0;
}

void PIN_AlarmSet()
{
  PORTC.PODR.BIT.B4 = 1;
}

void PIN_AlarmReset()
{
  PORTC.PODR.BIT.B4 = 0;
}

uint8_t PIN_Get6dB()
{
  return PORTC.PIDR.BIT.B2;
}

uint8_t PIN_Get14dB()
{
  return PORTC.PIDR.BIT.B0;
}

uint8_t PIN_GetExtIRQ()
{
  return PORTC.PIDR.BIT.B6;
}

/* End user code. Do not edit comment generated here */
