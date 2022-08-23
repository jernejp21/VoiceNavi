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
* File Name        : TPU0.c
* Component Version: 1.11.0
* Device(s)        : R5F565N9FxFP
* Description      : This file implements device driver for TPU0.
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
#include "TPU0.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TPU0_Create
* Description  : This function initializes the TPU0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TPU0_Create(void)
{
    /* Release TPU channel 0 from stop state */
    MSTP(TPU4) = 0U;

    /* Stop TPU channel 0 counter */
    TPUA.TSTR.BIT.CST0 = 0U;

    /* Set TGI0A interrupt priority level */
    ICU.SLIBXR130.BYTE = 0x0FU;
    IPR(PERIB, INTB130) = _0D_TPU_PRIORITY_LEVEL13;

    /* TPU channel 0 is used as normal mode */
    TPUA.TSYR.BIT.SYNC0 = 0U;
    TPU0.TCR.BYTE = _00_TPU_PCLK_1 | _20_TPU_CKCL_A;
    TPU0.TIER.BYTE |= (_01_TPU_TGIEA_ENABLE | _00_TPU_TGIEB_DISABLE | _00_TPU_TGIEC_DISABLE | _00_TPU_TGIED_DISABLE | 
                      _00_TPU_TCIEV_DISABLE | _00_TPU_TTGE_DISABLE | _40_TPU_TIER_DEFAULT);
    TPU0.TIORH.BYTE = _00_TPU_IOA_DISABLE | _00_TPU_IOB_DISABLE;
    TPU0.TIORL.BYTE = _00_TPU_IOC_DISABLE | _00_TPU_IOD_DISABLE;
    TPU0.TGRA = _09C3_TGRA0_VALUE;
    TPU0.TGRB = _176F_TGRB0_VALUE;
    TPU0.TGRC = _176F_TGRC0_VALUE;
    TPU0.TGRD = _176F_TGRD0_VALUE;

    R_TPU0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_TPU0_Start
* Description  : This function starts the TPU0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TPU0_Start(void)
{
    /* Enable TGI0A interrupt in ICU */
    IEN(PERIB, INTB130) = 1U;
    
    /* Start TPU channel 0 counter */
    TPUA.TSTR.BIT.CST0 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_TPU0_Stop
* Description  : This function stops the TPU0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TPU0_Stop(void)
{
    /* Disable TGI0A interrupt in ICU */
    IEN(PERIB, INTB130) = 0U;
    
    /* Stop TPU channel 0 counter */
    TPUA.TSTR.BIT.CST0 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
