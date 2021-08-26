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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : Pin.c
* Version      : 1.0.2
* Device(s)    : R5F5651EHxFP
* Description  : This file implements SMC pin code generation.
* Creation Date: 2021-08-26
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
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Pins_Create
* Description  : This function initializes Smart Configurator pins
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Pins_Create(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set AN006 pin */
    MPC.P46PFS.BYTE = 0x80U;
    PORT4.PMR.BYTE &= 0xBFU;
    PORT4.PDR.BYTE &= 0xBFU;

    /* Set AN007 pin */
    MPC.P47PFS.BYTE = 0x80U;
    PORT4.PMR.BYTE &= 0x7FU;
    PORT4.PDR.BYTE &= 0x7FU;

    /* Set DA1 pin */
    MPC.P05PFS.BYTE = 0x80U;
    PORT0.PMR.BYTE &= 0xDFU;
    PORT0.PDR.BYTE &= 0xDFU;

    /* Set IRQ13 pin */
    MPC.PC6PFS.BYTE = 0x40U;
    PORTC.PMR.BYTE &= 0xBFU;
    PORTC.PDR.BYTE &= 0xBFU;

    /* Set MISOC pin */
    MPC.PD2PFS.BYTE = 0x0DU;
    PORTD.PMR.BYTE |= 0x04U;

    /* Set MOSIC pin */
    MPC.PD1PFS.BYTE = 0x0DU;
    PORTD.PMR.BYTE |= 0x02U;

    /* Set RSPCKC pin */
    MPC.PD3PFS.BYTE = 0x0DU;
    PORTD.PMR.BYTE |= 0x08U;

    /* Set SSCL11 pin */
    MPC.PB6PFS.BYTE = 0x24U;
    PORTB.PMR.BYTE |= 0x40U;

    /* Set SSDA11 pin */
    MPC.PB7PFS.BYTE = 0x24U;
    PORTB.PMR.BYTE |= 0x80U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

