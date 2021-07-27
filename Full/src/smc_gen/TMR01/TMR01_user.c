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
* File Name    : TMR01_user.c
* Version      : 1.7.0
* Device(s)    : R5F565NEDxFP
* Description  : This file implements device driver for TMR01.
* Creation Date: 2021-07-26
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
#include "TMR01.h"
/* Start user code for include. Do not edit comment generated here */
#include "globals.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
int16_t data;
int count;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TMR01_Create_UserInit
* Description  : This function adds user code after initializing TMR
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_TMR01_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
  /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_TMR01_cmia0_interrupt
* Description  : This function is CMIA0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_TMR01_cmia0_interrupt(void)
{
    /* Start user code for r_TMR01_cmia0_interrupt. Do not edit comment generated here */
  TMR01.TCNT = 0;
  if(g_wav_file.bps == 8)
  {
    data = g_file_data[g_counter] << 4;
    count = 1;
    g_current_byte += 1;
  }
  else
  {
    data = (((g_file_data[g_counter + 1] << 8) | g_file_data[g_counter]) >> 4) + 2048;
    count = 2;
    g_current_byte += 2;
  }

  DA.DADR1 = data;
  g_counter += count;

  if(g_counter > (FILE_SIZE - 1))
  {
    g_readBuffer = 1;
    g_counter = 0;
  }

  if(g_wav_file.data_size == g_current_byte)
  {
    //end of file
    g_playing = 0;
    g_current_byte = 0;
  }
  /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_TMR01_cmib0_interrupt
* Description  : This function is CMIB0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_TMR01_cmib0_interrupt(void)
{
    /* Start user code for r_TMR01_cmib0_interrupt. Do not edit comment generated here */
  TMR01.TCNT = 0;
  g_counter++;
  /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* R_TMR01_Set_Frequency
 * counter = 0 -> set counter A
 * counter = 1 -> set counter B
 */
void R_TMR01_Set_Frequency(uint32_t freq_hz, uint8_t counter)
{
  // Timer clock is 60000 kHz
  if(counter == 0)
  {
    TMR01.TCORA = (uint16_t) (60000000 / freq_hz);
  }
  else if(counter == 1)
  {
    TMR01.TCORB = (uint16_t) (60000000 / freq_hz);
  }

}

void R_TMR01_Stop_A(void)
{
  /* Disable interrupt for counter A */
  IEN(PERIB, INTB146)= 0U;
}

void R_TMR01_Stop_B(void)
{
  /* Disable interrupt for counter B */
  IEN(PERIB, INTB147)= 0U;
}

void R_TMR01_Start_A(void)
{
  /* Disable interrupt for counter A */
  IEN(PERIB, INTB146)= 1U;
}

void R_TMR01_Start_B(void)
{
  /* Disable interrupt for counter B */
  IEN(PERIB, INTB147)= 1U;
}
/* End user code. Do not edit comment generated here */
