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
* File Name    : EXT_IRQ.c
* Version      : 2.0.1
* Device(s)    : R5F5651EHxFP
* Description  : This file implements device driver for EXT_IRQ.
* Creation Date: 2021-08-31
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
#include "EXT_IRQ.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_EXT_IRQ_Create
* Description  : This function initializes the ICU module
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_EXT_IRQ_Create(void)
{
    /* Disable IRQ interrupts */
    ICU.IER[0x08].BYTE = 0x00U;
    ICU.IER[0x09].BYTE = 0x00U;

    /* Disable software interrupt */
    IEN(ICU,SWINT) = 0U;
    IEN(ICU,SWINT2) = 0U;

    /* Disable IRQ digital filter */
    ICU.IRQFLTE1.BYTE &= ~(_20_ICU_IRQ13_FILTER_ENABLE);

    /* Set IRQ13 pin */
    MPC.PC6PFS.BYTE = 0x40U;
    PORTC.PDR.BYTE &= 0xBFU;
    PORTC.PMR.BYTE &= 0xBFU;

    /* Set IRQ detection type */
    ICU.IRQCR[13].BYTE = _0C_ICU_IRQ_EDGE_BOTH;
    IR(ICU,IRQ13) = 0U;

    /* Set IRQ13 priority level */
    IPR(ICU,IRQ13) = _06_ICU_PRIORITY_LEVEL6;

    R_EXT_IRQ_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_EXT_IRQ_IRQ13_Start
* Description  : This function enables IRQ13 interrupt
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_EXT_IRQ_IRQ13_Start(void)
{
    /* Enable IRQ13 interrupt */
    IEN(ICU,IRQ13) = 1U;
}

/***********************************************************************************************************************
* Function Name: R_EXT_IRQ_IRQ13_Stop
* Description  : This function disables IRQ13 interrupt
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_EXT_IRQ_IRQ13_Stop(void)
{
    /* Disable IRQ13 interrupt */
    IEN(ICU,IRQ13) = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
