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
* File Name    : TMR23.c
* Version      : 1.7.0
* Device(s)    : R5F565NEDxFP
* Description  : This file implements device driver for TMR23.
* Creation Date: 2021-08-12
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
#include "TMR23.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TMR23_Create
* Description  : This function initializes the TMR2 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TMR23_Create(void)
{
    /* Disable TMR2 interrupts */
    IEN(PERIB, INTB152) = 0U;

    /* Cancel TMR module stop state */
    MSTP(TMR23) = 0U; 

    /* Set timer counter control setting */
    TMR2.TCCR.BYTE = _18_TMR_CLK_TMR1_OVRF | _00_TMR_CLK_DISABLED;

    /* Set counter clear and interrupt */
    TMR2.TCR.BYTE = _40_TMR_CMIA_INT_ENABLE | _08_TMR_CNT_CLR_COMP_MATCH_A | _00_TMR_CMIB_INT_DISABLE | 
                    _00_TMR_OVI_INT_DISABLE;

    /* Set A/D trigger and output */
    TMR2.TCSR.BYTE = _00_TMR_AD_TRIGGER_DISABLE | _E0_TMR02_TCSR_DEFAULT;

    /* Set compare match value */ 
    TMR23.TCORA = _003A_TMR23_COMP_MATCH_VALUE_A;
    TMR23.TCORB = _0074_TMR23_COMP_MATCH_VALUE_B;

    /* Configure TMR2 interrupts */ 
    ICU.SLIBR152.BYTE = 0x09U;
    IPR(PERIB, INTB152) = _03_TMR_PRIORITY_LEVEL3;

    R_TMR23_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_TMR23_Start
* Description  : This function starts the TMR2 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TMR23_Start(void)
{
    /*Enable TMR2 interrupt*/
    IR(PERIB, INTB152) = 0U;
    IEN(PERIB, INTB152) = 1U;

    /*Start counting*/
    TMR3.TCCR.BYTE = _08_TMR_CLK_SRC_PCLK | _05_TMR_PCLK_DIV_1024;
}

/***********************************************************************************************************************
* Function Name: R_TMR23_Stop
* Description  : This function stop the TMR2 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TMR23_Stop(void)
{
    /*Enable TMR2 interrupt*/ 
    IEN(PERIB, INTB152) = 0U;

    /*Stop counting*/ 
    TMR3.TCCR.BYTE = _00_TMR_CLK_DISABLED;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
