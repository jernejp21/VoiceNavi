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
* File Name    : TMR_play.c
* Version      : 1.7.0
* Device(s)    : R5F5651EHxFP
* Description  : This file implements device driver for TMR_play.
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
#include "TMR_play.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TMR_play_Create
* Description  : This function initializes the TMR0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TMR_play_Create(void)
{
    /* Disable TMR0 interrupts */
    IEN(PERIB, INTB146) = 0U;

    /* Cancel TMR module stop state */
    MSTP(TMR01) = 0U; 

    /* Set timer counter control setting */
    TMR0.TCCR.BYTE = _18_TMR_CLK_TMR1_OVRF | _00_TMR_CLK_DISABLED;

    /* Set counter clear and interrupt */
    TMR0.TCR.BYTE = _40_TMR_CMIA_INT_ENABLE | _08_TMR_CNT_CLR_COMP_MATCH_A | _00_TMR_CMIB_INT_DISABLE | 
                    _00_TMR_OVI_INT_DISABLE;

    /* Set A/D trigger and output */
    TMR0.TCSR.BYTE = _00_TMR_AD_TRIGGER_DISABLE | _E0_TMR02_TCSR_DEFAULT;

    /* Set compare match value */ 
    TMR01.TCORA = _0001_TMR01_COMP_MATCH_VALUE_A;
    TMR01.TCORB = _0001_TMR01_COMP_MATCH_VALUE_B;

    /* Configure TMR0 interrupts */ 
    ICU.SLIBR146.BYTE = 0x03U;
    IPR(PERIB, INTB146) = _0F_TMR_PRIORITY_LEVEL15;

    R_TMR_play_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_TMR_play_Start
* Description  : This function starts the TMR0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TMR_play_Start(void)
{
    /*Enable TMR0 interrupt*/
    IR(PERIB, INTB146) = 0U;
    IEN(PERIB, INTB146) = 1U;

    /*Start counting*/
    TMR1.TCCR.BYTE = _08_TMR_CLK_SRC_PCLK | _00_TMR_PCLK_DIV_1;
}

/***********************************************************************************************************************
* Function Name: R_TMR_play_Stop
* Description  : This function stop the TMR0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TMR_play_Stop(void)
{
    /*Enable TMR0 interrupt*/ 
    IEN(PERIB, INTB146) = 0U;

    /*Stop counting*/ 
    TMR1.TCCR.BYTE = _00_TMR_CLK_DISABLED;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
