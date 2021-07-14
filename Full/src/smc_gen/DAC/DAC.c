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
* File Name    : DAC.c
* Version      : 1.8.4
* Device(s)    : R5F565NEDxFP
* Description  : This file implements device driver for DAC.
* Creation Date: 2021-07-14
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
#include "DAC.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_DAC_Create
* Description  : This function initializes the DA converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_DAC_Create(void)
{
    /* Cancel DA stop state in LPC */
    MSTP(DA) = 0U;

    /* Set DA format select register */
    DA.DADPR.BYTE = _00_DA_DPSEL_R;

    /* Set D/A-A/D synchronous control register */
    DA.DAADSCR.BYTE = _00_DA_DAADSYNC_DISABLE;

    /* Set DA1 pin */
    PORT0.PMR.BYTE &= 0xDFU;
    PORT0.PDR.BYTE &= 0xDFU;
    MPC.P05PFS.BIT.ASEL = 1U;

    R_DAC_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_DAC1_Start
* Description  : This function enables the DA1 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_DAC1_Start(void)
{
    DA.DACR.BIT.DAE = 0U;
    DA.DACR.BIT.DAOE1 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_DAC1_Stop
* Description  : This function stops the DA1 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_DAC1_Stop(void)
{
    DA.DACR.BIT.DAOE1 = 0U;
}

/***********************************************************************************************************************
* Function Name: R_DAC1_Set_ConversionValue
* Description  : This function sets the DA1 converter value
* Arguments    : reg_value -
*                    value of conversion
* Return Value : None
***********************************************************************************************************************/

void R_DAC1_Set_ConversionValue(uint16_t reg_value)
{
    DA.DADR1 = (uint16_t)(reg_value & 0x0FFFU);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
