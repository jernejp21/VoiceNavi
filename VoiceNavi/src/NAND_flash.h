/**
 * Voice Navi Firmware
 *
 * MIT License
 *
 * Copyright (c) 2021 Azurtest
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __NAND_FLASH_H
#define __NAND_FLASH_H

/* This are device dependent values. Change if needed */
#define NAND_NR_OF_BLOCKS 2048
#define NAND_NR_OF_PAGES 64
#define NAND_PAGE_SIZE 2048
#define NAND_MAX_BAD_BLOCKS 40  //Size of all blocks minus min. number of valid blocks
#define NAND_SPARE_AREA_SIZE 0x11  //Size of Spare Area 01
#define NAND_SPARE_AREA_ADD 0x800  //Size of Spare 01 Column Address
#define NAND_DELAY_TIME 1
#define NAND_DELAY_UNIT BSP_DELAY_MICROSECS
#define NAND_FLASH_SIZE (NAND_PAGE_SIZE * NAND_NR_OF_PAGES * NAND_NR_OF_BLOCKS) // Flash size in bits

/* Macro for MEMDEV library */
#define NAND_DEVNO 0

/* NAND Flash commands */
#define NAND_WRITE_DISABLE 0x04
#define NAND_WRITE_ENABLE 0x06
#define NAND_BLOCK_ERASE 0xD8
#define NAND_PROGRAM_LOAD 0x02
#define NAND_PROGRAM_EXECUTE 0x10
#define NAND_PAGE_READ 0x13
#define NAND_READ_FROM_CACHE 0x03
#define NAND_READ_ID 0x9F
#define NAND_RESET 0xFF
#define NAND_GET_FEATURE 0x0F
#define NAND_SET_FEATURE 0x1F

/* NAND Flash registers */
#define NAND_BLOCK_LOCK_REG 0xA0
#define NAND_STATUS_REG 0xC0

/* NAND Flash status register values */
#define NAND_STATUS_OIP 0x01
#define NAND_STATUS_WEL 0x02
#define NAND_STATUS_E_FAIL 0x04
#define NAND_STATUS_P_FAIL 0x08
#define NAND_STATUS_ECCS0 0x10
#define NAND_STATUS_ECCS1 0x20

typedef struct flash_custom_FAT
{
  uint32_t address;
  uint32_t file_size;
} flash_custom_FAT_t;

typedef enum nand_flash_status
{
  NAND_WRITE_OK = 0,
  NAND_WRITE_NOK,
  NAND_WRITE_OVERFLOW,
  NAND_READ_OK,
  NAND_READ_NOK,
  NAND_ERASE_OK,
  NAND_ERASE_NOK,
} nand_flash_status_t;

/** Global functions */
int NAND_CheckBlock();
void NAND_Reset(void);
nand_flash_status_t NAND_Erase(void);
nand_flash_status_t NAND_CopyToFlash(void);
nand_flash_status_t NAND_ReadFromFlash(uint32_t*, uint32_t, uint8_t*);
int NAND_CheckDataInFlash(void);
nand_flash_status_t NAND_WriteToFlash(uint32_t*, uint32_t, uint8_t*);
nand_flash_status_t nand_wait_operation_complete(void);
void NAND_LockFlash(void);
void NAND_UnlockFlash(void);
nand_flash_status_t nand_check_if_write_ok(uint8_t*, uint8_t*, uint32_t);

#endif //__NAND_FLASH_H
