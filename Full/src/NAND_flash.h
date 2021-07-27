/*
 * Description here
 */

#ifndef __NAND_FLASH_H
#define __NAND_FLASH_H

#define NAND_NR_OF_BLOCKS 2048
#define NAND_PAGE_SIZE 2048
#define NAND_DEVNO 0

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

#define NAND_BLOCK_LOCK_REG 0xA0
#define NAND_STATUS_REG 0xC0
#define NAND_STATUS_OIP 0x01
#define NAND_STATUS_WEL 0x02
#define NAND_STATUS_E_FAIL 0x04
#define NAND_STATUS_P_FAIL 0x08

typedef struct flash_custom_FAT
{
  uint32_t address;
  uint32_t file_size;
} flash_custom_FAT_t;

typedef enum nand_flash_status
{
  NAND_WRITE_OK = 0,
  NAND_WRITE_NOK,
  NAND_READ_OK,
  NAND_READ_NOK,
  NAND_ERASE_OK,
  NAND_ERASE_NOK,
} nand_flash_status_t;

nand_flash_status_t NAND_Erase(void);
void NAND_CopyToFlash(void);
void NAND_ReadFromFlash(uint32_t, uint32_t, uint8_t*);
int NAND_CheckDataInFlash(void);
nand_flash_status_t nand_copy_to_flash(uint32_t, uint32_t, uint8_t*);
void nand_wait_operation_complete(void);

#endif //__NAND_FLASH_H
