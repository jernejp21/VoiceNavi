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
* File Name    : TMR_various.h
* Version      : 1.7.0
* Device(s)    : R5F5651EHxFP
* Description  : This file implements device driver for TMR_various.
* Creation Date: 2021-08-23
***********************************************************************************************************************/

#ifndef CFG_TMR_various_H
#define CFG_TMR_various_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_tmr.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define TMR2_PCLK_COUNTER_DIVISION                    (1024) 
#define _003A_TMR23_COMP_MATCH_VALUE_A                (0x003AU)
#define _0074_TMR23_COMP_MATCH_VALUE_B                (0x0074U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_TMR_various_Create(void);
void R_TMR_various_Create_UserInit(void);
void R_TMR_various_Start(void);
void R_TMR_various_Stop(void);
/* Start user code for function. Do not edit comment generated here */
void R_TMR_various_Set_Frequency(uint32_t freq_hz);
/* End user code. Do not edit comment generated here */
#endif
