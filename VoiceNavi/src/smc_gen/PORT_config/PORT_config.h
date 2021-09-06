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
* File Name    : PORT_config.h
* Version      : 2.1.3
* Device(s)    : R5F565N9FxFP
* Description  : This file implements device driver for PORT_config.
* Creation Date: 2021-09-06
***********************************************************************************************************************/

#ifndef CFG_PORT_config_H
#define CFG_PORT_config_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_port.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_PORT_config_Create(void);
void R_PORT_config_Create_UserInit(void);
/* Start user code for function. Do not edit comment generated here */
void LED_BusyOn();
void LED_BusyOff();
void LED_BusyToggle();
void LED_PowOn();
void LED_PowOff();
void LED_USBOn();
void LED_USBOff();
void LED_USBToggle();
void LED_AlarmOn();
void LED_AlarmOff();
void LED_Init();
void LED_Toggle();
uint8_t DIP_ReadState();
void PIN_ShutdownSet();
void PIN_ShutdownReset();
void PIN_RstSet();
void PIN_RstReset();
uint8_t PIN_Get6dB();
uint8_t PIN_Get14dB();
uint8_t PIN_GetExtIRQ();
/* End user code. Do not edit comment generated here */
#endif
