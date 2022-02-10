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
* File Name        : PORT_config.c
* Component Version: 2.3.0
* Device(s)        : R5F565N9FxFP
* Description      : This file implements device driver for PORT_config.
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
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_PORT_config_Create
* Description  : This function initializes the PORT
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_PORT_config_Create(void)
{
    /* Set PORT4 registers */
    PORT4.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT;
    PORT4.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORT4.PCR.BYTE = _04_Pm2_PULLUP_ON | _08_Pm3_PULLUP_ON;
    PORT4.PMR.BYTE = _00_Pm2_PIN_GPIO | _00_Pm3_PIN_GPIO;
    PORT4.PDR.BYTE = _00_Pm2_MODE_INPUT | _00_Pm3_MODE_INPUT;

    /* Set PORT5 registers */
    PORT5.PODR.BYTE = _00_Pm0_OUTPUT_0 | _00_Pm1_OUTPUT_0 | _00_Pm2_OUTPUT_0 | _00_Pm3_OUTPUT_0 | _00_Pm4_OUTPUT_0 | 
                      _00_Pm5_OUTPUT_0;
    PORT5.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORT5.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT;
    PORT5.DSCR.BYTE = _00_Pm0_HIDRV_OFF | _00_Pm1_HIDRV_OFF | _00_Pm2_HIDRV_OFF | _08_Pm3_HIDRV_ON | 
                      _10_Pm4_HIDRV_ON | _20_Pm5_HIDRV_ON;
    PORT5.DSCR2.BYTE = _00_Pm0_HISPEED_OFF | _00_Pm1_HISPEED_OFF | _00_Pm2_HISPEED_OFF | _00_Pm3_HISPEED_OFF | 
                       _00_Pm4_HISPEED_OFF | _00_Pm5_HISPEED_OFF;
    PORT5.PMR.BYTE = _00_Pm0_PIN_GPIO | _00_Pm1_PIN_GPIO | _00_Pm2_PIN_GPIO | _00_Pm3_PIN_GPIO | _00_Pm4_PIN_GPIO | 
                     _00_Pm5_PIN_GPIO;
    PORT5.PDR.BYTE = _01_Pm0_MODE_OUTPUT | _02_Pm1_MODE_OUTPUT | _04_Pm2_MODE_OUTPUT | _08_Pm3_MODE_OUTPUT | 
                     _10_Pm4_MODE_OUTPUT | _20_Pm5_MODE_OUTPUT | _C0_PDR5_DEFAULT;

    /* Set PORTA registers */
    PORTA.PCR.BYTE = _00_Pm0_PULLUP_OFF | _00_Pm1_PULLUP_OFF | _00_Pm2_PULLUP_OFF | _00_Pm3_PULLUP_OFF | 
                     _00_Pm4_PULLUP_OFF | _00_Pm5_PULLUP_OFF | _00_Pm6_PULLUP_OFF | _00_Pm7_PULLUP_OFF;
    PORTA.PMR.BYTE = _00_Pm0_PIN_GPIO | _00_Pm1_PIN_GPIO | _00_Pm2_PIN_GPIO | _00_Pm3_PIN_GPIO | _00_Pm4_PIN_GPIO | 
                     _00_Pm5_PIN_GPIO | _00_Pm6_PIN_GPIO | _00_Pm7_PIN_GPIO;
    PORTA.PDR.BYTE = _00_Pm0_MODE_INPUT | _00_Pm1_MODE_INPUT | _00_Pm2_MODE_INPUT | _00_Pm3_MODE_INPUT | 
                     _00_Pm4_MODE_INPUT | _00_Pm5_MODE_INPUT | _00_Pm6_MODE_INPUT | _00_Pm7_MODE_INPUT;

    /* Set PORTB registers */
    PORTB.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT;
    PORTB.ODR1.BYTE = _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORTB.PCR.BYTE = _08_Pm3_PULLUP_ON | _10_Pm4_PULLUP_ON;
    PORTB.DSCR.BYTE = _00_Pm0_HIDRV_OFF | _00_Pm1_HIDRV_OFF | _00_Pm2_HIDRV_OFF | _00_Pm5_HIDRV_OFF | 
                      _00_Pm6_HIDRV_OFF | _00_Pm7_HIDRV_OFF;
    PORTB.DSCR2.BYTE = _00_Pm0_HISPEED_OFF | _00_Pm1_HISPEED_OFF | _00_Pm2_HISPEED_OFF | _00_Pm5_HISPEED_OFF | 
                       _00_Pm6_HISPEED_OFF | _00_Pm7_HISPEED_OFF;
    PORTB.PMR.BYTE = _00_Pm3_PIN_GPIO | _00_Pm4_PIN_GPIO;
    PORTB.PDR.BYTE = _00_Pm3_MODE_INPUT | _00_Pm4_MODE_INPUT;

    /* Set PORTC registers */
    PORTC.PODR.BYTE = _00_Pm4_OUTPUT_0 | _00_Pm5_OUTPUT_0;
    PORTC.ODR0.BYTE = _00_Pm1_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORTC.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORTC.PCR.BYTE = _00_Pm0_PULLUP_OFF | _00_Pm2_PULLUP_OFF;
    PORTC.DSCR.BYTE = _00_Pm1_HIDRV_OFF | _00_Pm3_HIDRV_OFF | _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF | 
                      _00_Pm6_HIDRV_OFF | _00_Pm7_HIDRV_OFF;
    PORTC.DSCR2.BYTE = _00_Pm1_HISPEED_OFF | _00_Pm3_HISPEED_OFF | _00_Pm4_HISPEED_OFF | _00_Pm5_HISPEED_OFF | 
                       _00_Pm6_HISPEED_OFF | _00_Pm7_HISPEED_OFF;
    PORTC.PMR.BYTE = _00_Pm0_PIN_GPIO | _00_Pm2_PIN_GPIO | _00_Pm4_PIN_GPIO | _00_Pm5_PIN_GPIO;
    PORTC.PDR.BYTE = _00_Pm0_MODE_INPUT | _00_Pm2_MODE_INPUT | _10_Pm4_MODE_OUTPUT | _20_Pm5_MODE_OUTPUT;

    /* Set PORTE registers */
    PORTE.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORTE.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT;
    PORTE.PCR.BYTE = _40_Pm6_PULLUP_ON | _80_Pm7_PULLUP_ON;
    PORTE.DSCR.BYTE = _00_Pm0_HIDRV_OFF | _00_Pm1_HIDRV_OFF | _00_Pm2_HIDRV_OFF | _00_Pm3_HIDRV_OFF | 
                      _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF;
    PORTE.DSCR2.BYTE = _00_Pm0_HISPEED_OFF | _00_Pm1_HISPEED_OFF | _00_Pm2_HISPEED_OFF | _00_Pm3_HISPEED_OFF | 
                       _00_Pm4_HISPEED_OFF | _00_Pm5_HISPEED_OFF;
    PORTE.PMR.BYTE = _00_Pm6_PIN_GPIO | _00_Pm7_PIN_GPIO;
    PORTE.PDR.BYTE = _00_Pm6_MODE_INPUT | _00_Pm7_MODE_INPUT;

    R_PORT_config_Create_UserInit();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
