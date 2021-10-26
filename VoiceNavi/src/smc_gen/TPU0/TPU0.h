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
* File Name        : TPU0.h
* Component Version: 1.11.0
* Device(s)        : R5F565N9FxFP
* Description      : This file implements device driver for TPU0.
***********************************************************************************************************************/

#ifndef CFG_TPU0_H
#define CFG_TPU0_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_tpu.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define TPU0_PCLK_COUNTER_DIVISION         (1)
#define _09C3_TGRA0_VALUE                  (0x09C3U) /* TGRA0 value */
#define _176F_TGRB0_VALUE                  (0x176FU) /* TGRB0 value */
#define _176F_TGRC0_VALUE                  (0x176FU) /* TGRC0 value */
#define _176F_TGRD0_VALUE                  (0x176FU) /* TGRD0 value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_TPU0_Create(void);
void R_TPU0_Create_UserInit(void);
void R_TPU0_Start(void);
void R_TPU0_Stop(void);
/* Start user code for function. Do not edit comment generated here */
void R_TPU0_SetFrequency(uint32_t);
/* End user code. Do not edit comment generated here */
#endif
