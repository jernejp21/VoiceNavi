/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014(2015-2020) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_tfat_drv_if_sdmem.c
* Description  : TFAT driver Interface for SD memory card.
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*              : 01.12.2014 1.00     First Release
*              : 22.06.2015 1.02     Added support MCU RX231.
*              : 01.04.2016 1.03     Updated the xml file.
*              : 29.06.2018 1.04     Modified SD card API.
*              : 08.08.2019 2.00     Added support for FreeRTOS and 
*                                    Renesas uITRON (RI600V4).
*                                    Added support for GNUC and ICCRX.
*              : 10.09.2020 2.20     Added support for the format function.
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <string.h>
#include "r_tfat_drv_if_dev.h"
#include "r_tfat_driver_rx_config.h"

#if (TFAT_SDMEM_DRIVE_NUM > 0)
#include "ff.h"              /* TFAT define */
#include "diskio.h"          /* TFAT define */

#include "r_sdc_sd_rx_if.h"
#include "src/r_sdc_sd_rx_private.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
#define SDMEM_PRV_CSD_SHIT_7                  (7)
#define SDMEM_PRV_CSD_ERASE_BLOCK_MASK        (0x0000007f)

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
*******************************************************************************/

/******************************************************************************
* Function Name : sdmem_disk_initialize
* Description   : This function initializes the memory medium
*               :    for file operations
* Arguments     :  uint8_t  drive        : Physical drive number
* Return value  : Status of the memory medium
******************************************************************************/
DSTATUS sdmem_disk_initialize(uint8_t drive)
{
    return RES_OK;
}

/******************************************************************************
* Function Name : sdmem_disk_read
* Description   : This function reads data from the specified location
*               :    of the memory medium
* Arguments     : uint8_t drive          : Physical drive number
*               : uint8_t* buffer        : Pointer to the read data buffer
*               : uint32_t sector_number : uint32_t SectorNumber
*               : uint32_t sector_count  : Number of sectors to read
* Return value  : Result of function execution
******************************************************************************/
DRESULT sdmem_disk_read (
    uint8_t drive,               /* Physical drive number            */
    uint8_t* buffer,             /* Pointer to the read data buffer  */
    uint32_t sector_number,      /* Start sector number              */
    uint32_t sector_count        /* Number of sectors to read        */
)
{
    sdc_sd_access_t   sdmem_Access;
    sdc_sd_status_t   res = SDC_SD_SUCCESS;

    /* parameter check */
    if ((NULL == buffer) || (0 == sector_count))
    {
        return RES_ERROR;
    }

    sdmem_Access.p_buff = (uint8_t *)buffer;
    sdmem_Access.lbn    = sector_number;
    sdmem_Access.cnt    = sector_count;

    /* TRANS mode : Software */
    res = R_SDC_SDMEM_ReadSoftwareTrans(drive, &sdmem_Access);
    if ( SDC_SD_SUCCESS != res )
    {
        return RES_ERROR;
    }

    return RES_OK;
}

/******************************************************************************
* Function Name : sdmem_disk_write
* Description   : This function writes data to a specified location
*               :    of the memory medium
* Arguments     : uint8_t drive          : Physical drive number
*               : const uint8_t* buffer  : Pointer to the write data
*               : uint32_t sector_number : Sector number to write
*               : uint32_t sector_count  : Number of sectors to write
* Return value  : Result of function execution
******************************************************************************/
DRESULT sdmem_disk_write (
    uint8_t drive,                /* Physical drive number           */
    const uint8_t* buffer,        /* Pointer to the write data       */
    uint32_t sector_number,       /* Sector number to write          */
    uint32_t sector_count         /* Number of sectors to write      */
)
{

    sdc_sd_access_t   sdmem_Access;
    sdc_sd_status_t   res;

    /* parameter check */
    if ((NULL == buffer) || (0 == sector_count))
    {
        return RES_ERROR;
    }

    sdmem_Access.p_buff     = (uint8_t *)buffer;
    sdmem_Access.lbn        = sector_number;
    sdmem_Access.cnt        = sector_count;
    sdmem_Access.write_mode = SDC_SD_WRITE_OVERWRITE;

    /* TRANS mode : Software */
    res = R_SDC_SDMEM_WriteSoftwareTrans(drive, &sdmem_Access);
    if ( SDC_SD_SUCCESS != res )
    {
        return RES_ERROR;
    }

    return RES_OK;
}

/******************************************************************************
* Function Name : sdmem_disk_ioctl
* Description   : This function is used to execute memory operations
*               :    other than read\write
* Arguments     : uint8_t drive   : Drive number
*               : uint8_t command : Control command code
*               : void*   buffer  : Data transfer buffer
* Return value  : Result of function execution
******************************************************************************/
DRESULT sdmem_disk_ioctl (
    uint8_t drive,                /* Drive number             */
    uint8_t command,              /* Control command code     */
    void* buffer                  /* Data transfer buffer     */
)
{
    uint8_t                cmd = command;
#if FF_USE_MKFS == 1
    sdc_sd_card_status_t   sdmem_cardstatus;
    sdc_sd_card_reg_t      sdmem_cardinfo;
    sdc_sd_status_t        ret;
#endif

    if ((NULL == buffer) && (CTRL_SYNC != cmd))
    {
        return RES_PARERR;
    }

    switch (cmd)
    {
        case CTRL_SYNC:

        break;

        case GET_SECTOR_COUNT:
#if FF_USE_MKFS
            /* Get SD card sector count */
            ret = R_SDC_SD_GetCardStatus(drive, &sdmem_cardstatus);
            if (SDC_SD_SUCCESS != ret)
            {
                return RES_ERROR;
            }

            ((uint32_t *)buffer)[0] = sdmem_cardstatus.card_sector_size;
#else
            return RES_PARERR;
#endif
        break;

        case GET_SECTOR_SIZE:
#if FF_MAX_SS == FF_MIN_SS
            return RES_PARERR;
#else
            ((uint32_t *)buffer)[0] = (uint32_t)SDC_SD_TRANS_BLOCK_SIZE;
#endif
        break;

        case GET_BLOCK_SIZE:
#if FF_USE_MKFS
            /* Get SD card block size */
            ret = R_SDC_SD_GetCardInfo(drive, &sdmem_cardinfo);
            if (SDC_SD_SUCCESS != ret)
            {
                return RES_ERROR;
            }

            /* Erase sector size are [45:39] bits of CSD register */
            ((uint32_t *)buffer)[0]  = ((sdmem_cardinfo.csd[1]>>SDMEM_PRV_CSD_SHIT_7) & SDMEM_PRV_CSD_ERASE_BLOCK_MASK) + 1;
#else
            return RES_PARERR;
#endif
        break;

        case CTRL_TRIM:

        break;

        default:
            return RES_PARERR;
        break;
    }
    return RES_OK;
}

/******************************************************************************
* Function Name : sdmem_disk_status
* Description   : This function is used to retrieve the current status
*               :    of the disk
* Arguments     : uint8_t drive : Physical drive number
* Return value  : Status of the disk
******************************************************************************/
DSTATUS sdmem_disk_status (
    uint8_t drive                  /* Physical drive number    */
)
{

    /*  Please put the code for disk_status driver interface
         function over here.  */
    /*  Please refer the application note for details.  */
    return RES_OK;
}
#endif /* (TFAT_SDMEM_DRIVE_NUM > 0) */
/******************************************************************************
End  Of File
******************************************************************************/
