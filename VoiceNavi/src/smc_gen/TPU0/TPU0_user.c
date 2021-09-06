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
* File Name    : TPU0_user.c
* Version      : 1.9.2
* Device(s)    : R5F565N9FxFP
* Description  : This file implements device driver for TPU0.
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
#include "TPU0.h"
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
* Function Name: R_TPU0_Create_UserInit
* Description  : This function adds user code after initializing the TPU0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TPU0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
  /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_TPU0_tgi0a_interrupt
* Description  : This function is TGI0A interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_TPU0_tgi0a_interrupt(void)
{
    /* Start user code for r_TPU0_tgi0a_interrupt. Do not edit comment generated here */

  /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
void R_TPU0_SetFrequency(uint32_t freq_hz)
{
  // Timer clock is 60000 kHz
  //TMR01.TCORA = (uint16_t) (60000000 / freq_hz);
  TPU0.TGRA = (uint16_t) (60000000 / freq_hz);

}
/* End user code. Do not edit comment generated here */
