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
* File Name    : DMAC0.h
* Version      : 1.6.2
* Device(s)    : R5F5651EHxFP
* Description  : This file implements device driver for DMAC0.
* Creation Date: 2021-08-26
***********************************************************************************************************************/

#ifndef CFG_DMAC0_H
#define CFG_DMAC0_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_dmac.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _BA_DMAC0_ACTIVATION_SOURCE                             (0xBAU) /* Activation source select */
#define _00000000_DMAC0_SRC_ADDR                                (0x00000000UL) /* Source address */
#define _00000000_DMAC0_DST_ADDR                                (0x00000000UL) /* Destination address */
#define _00000000_DMAC0_DMCRA_COUNT                             (0x00000000UL) /* Transfer count */
#define _0200_DMAC0_SRC_EXT_RPT_AREA                            (0x0200U) /* Source address extended repeat area */
#define _0002_DMAC0_DST_EXT_RPT_AREA                            (0x0002U) /* Destination address extended repeat area */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_DMAC0_Create(void);
void R_DMAC0_Create_UserInit(void);
void R_DMAC0_Start(void);
void R_DMAC0_Stop(void);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
