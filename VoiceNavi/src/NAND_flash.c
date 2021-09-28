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

#include "globals.h"

FATFS fs;
DIR dir;
FILINFO filno;
FIL file;
FIL file1;
FRESULT fr;
UINT size;

uint8_t flash_buffer[NAND_PAGE_SIZE];
uint8_t wav_buffer[NAND_PAGE_SIZE];
char line[100];

flash_custom_FAT_t flash_table[255];
playlist_t output_music[255];
wav_header_t wav_file;
int startOfFileNames;
int startOfPlaylist;

file_name_t file_name;

int16_t bad_blocks[NAND_MAX_BAD_BLOCKS];

/* Check if block is usable or not - factory set. */
int NAND_CheckBlock()
{
  uint32_t addr;
  int block;
  int bad_block_cnt = 0;
  int isBadBlock = 0;
  uint8_t data[NAND_SPARE_AREA_SIZE];
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];
  uint16_t column_address = NAND_SPARE_AREA_ADD;

  memset(bad_blocks, -1, sizeof(bad_blocks));
  for(block = 0; block < NAND_NR_OF_BLOCKS; block++)
  {
    addr = block << 6;

    /* 1. Page Read to Cache */
    tx_buff[0] = NAND_PAGE_READ;
    tx_buff[1] = *((uint8_t*)&addr + 2);
    tx_buff[2] = *((uint8_t*)&addr + 1);
    tx_buff[3] = *((uint8_t*)&addr);
    memdrv_info.cnt = 4;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    /* 2. Wait until read operation finishes */
    nand_wait_operation_complete();

    /* 3. Read from Cache */
    tx_buff[0] = NAND_READ_FROM_CACHE;
    tx_buff[1] = *((uint8_t*)&column_address + 1);
    tx_buff[2] = *((uint8_t*)&column_address);
    tx_buff[3] = 0;  //dummy byte
    memdrv_info.cnt = 4;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = NAND_SPARE_AREA_SIZE;
    memdrv_info.p_data = &data[0];
    R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    for(int i = 0; i < NAND_SPARE_AREA_SIZE; i++)
    {
      if(0 == data[i])
      {
        isBadBlock = 1;
      }
    }

    if(isBadBlock)
    {
      bad_blocks[bad_block_cnt] = block;  //Mark block as bad
      bad_block_cnt++;
      isBadBlock = 0;
    }
  }
  return 0;
}

void NAND_CopyToFlash()
{

  int cnt = 0;
  uint32_t flash_address = NAND_DATA_PAGE;
  nand_flash_status_t flash_status;

  fr = f_mount(&fs, "", 0);
  if(FR_OK != fr)
  {
    //Error if FAT is not present on USB.
    ERROR_FileSystem();
  }

  fr = f_findfirst(&dir, &filno, "", "*.wpj");
  if(FR_OK != fr)
  {
    //Error if .wpj file is not present.
    ERROR_FileSystem();
  }

  fr = f_open(&file, (const TCHAR*)&filno.fname, (FA_OPEN_ALWAYS | FA_READ));
  if(FR_OK != fr)
  {
    //Error if .wpj file cannot be opened (bad file system or bad file).
    ERROR_FileSystem();
  }

  nand_flash_status_t nand_status;
  nand_status = NAND_Erase();
  if(NAND_ERASE_NOK == nand_status)
  {
    ERROR_FlashECS();
  }
  NAND_Reset();

  startOfFileNames = 0;
  startOfPlaylist = 0;
  while(f_gets((TCHAR*)&line, sizeof(line), &file))
  {
    /* Section #2 in wpj file represents file name order.
     * 0,file1.wav
     * 1,file50.wav
     * 2,file8.wav
     */
    if(strncmp(line, "#2", 2) == 0)
    {
      startOfFileNames = 1;
      cnt = 0;
      continue;
    }

    /* Section #3 in wpj file represents playlist.
     * input channel, output channel, repetition, file to play (up to 8 files)
     * 0,1,1,2
     * 1,1,1,2
     * 2,1,2,0
     */
    if(strncmp(line, "#3", 2) == 0)
    {
      startOfPlaylist = 1;
      startOfFileNames = 0;
      cnt = 0;
      continue;
    }

    if(startOfFileNames)
    {
      WAV_PlaceNameToTable((char*)&file_name.file_name, (char*)&line);
      fr = f_open(&file1, &file_name.file_name[0], FA_READ);
      if(FR_OK != fr)
      {
        ERROR_WAVEFile();
      }
      fr = f_read(&file1, &wav_buffer[0], sizeof(wav_buffer), &size);
      WAV_Open(&wav_file, &wav_buffer[0]);
      flash_table[cnt].address = flash_address;
      flash_table[cnt].file_size = wav_file.file_size;

      do
      {
        //LED_USBToggle();
        flash_status = NAND_WriteToFlash(flash_address, size, wav_buffer);
        flash_address += size;
        if(NAND_WRITE_NOK == flash_status)
        {
          ERROR_FlashECS();
        }
        fr = f_read(&file1, &wav_buffer[0], sizeof(wav_buffer), &size);
      }
      while(size == sizeof(wav_buffer));

      /* Copy last part of read file to flash */
      flash_status = NAND_WriteToFlash(flash_address, size, wav_buffer);
      // Write always on new page
      flash_address += sizeof(wav_buffer);
      if(NAND_WRITE_NOK == flash_status)
      {
        ERROR_FlashECS();
      }
      f_close(&file1);
      cnt++;
    }

    if(startOfPlaylist)
    {
      WAV_PlaceSongsToTable((playlist_t*)&output_music[cnt], (char*)&line);
      cnt++;
      if(cnt > 254)
      {
        break;
      }
    }
  }
  f_close(&file);

  //Mark that data is in flash
  uint8_t data[4] = {0xEF, 0xBE, 0xAD, 0xDE};  //0xdeadbeef in little endian
  flash_status = NAND_WriteToFlash(0, sizeof(data), data);
  if(NAND_WRITE_NOK == flash_status)
  {
    ERROR_FlashECS();
  }

  //Copy file table to NAND flash
  flash_status = NAND_WriteToFlash(NAND_FILE_LIST_PAGE, sizeof(flash_table), (uint8_t*)&flash_table[0]);
  if(NAND_WRITE_NOK == flash_status)
  {
    ERROR_FlashECS();
  }

  //Copy playlist table to NAND flash
  flash_status = NAND_WriteToFlash(NAND_PLAYLIST_PAGE, sizeof(output_music), (uint8_t*)&output_music[0]);
  if(NAND_WRITE_NOK == flash_status)
  {
    ERROR_FlashECS();
  }

  /* After finishing wiriting to flash, lock flash for write protection */
  NAND_LockFlash();

}

nand_flash_status_t NAND_ReadFromFlash(uint32_t address, uint32_t size, uint8_t *p_data)
{
  nand_flash_status_t ret;
  uint16_t column_address;
  uint32_t row_address;
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];
  uint32_t read_size;

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;
  row_address = (uint16_t)(address / NAND_PAGE_SIZE);
  column_address = (uint16_t)(address % NAND_PAGE_SIZE);

  while(1)
  {
    if(0 == size)
    {
      return NAND_READ_OK;
    }

    if(size <= (NAND_PAGE_SIZE - column_address))
    {
      read_size = size;
    }
    else
    {
      read_size = NAND_PAGE_SIZE - column_address;
    }

    /* Do not read from bad blocks */
    for(int i = 0; i < NAND_MAX_BAD_BLOCKS; i++)
    {
      if((row_address >> 6) == bad_blocks[i])
      {
        // Bits 15:6 are block - mask is 0x7FC0
        // When we add one block we have to add bit 6 - 0x40
        row_address = (row_address & 0x7FC0) + 0x40;
        continue;
      }
    }

    /* 1. Page Read to Cache */
    tx_buff[0] = NAND_PAGE_READ;
    tx_buff[1] = *((uint8_t*)&row_address + 2);
    tx_buff[2] = *((uint8_t*)&row_address + 1);
    tx_buff[3] = *((uint8_t*)&row_address);
    memdrv_info.cnt = 4;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    /* 2. Wait until read operation finishes */
    ret = nand_wait_operation_complete();
    if(NAND_READ_NOK == ret)
    {
      return NAND_READ_NOK;
    }

    /* 3. Read from Cache */
    tx_buff[0] = NAND_READ_FROM_CACHE;
    tx_buff[1] = *((uint8_t*)&column_address + 1);
    tx_buff[2] = *((uint8_t*)&column_address);
    tx_buff[3] = 0;  //dummy byte
    memdrv_info.cnt = 4;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = read_size;
    memdrv_info.p_data = p_data;
    R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    /* Prepare for next read if needed */
    row_address++;
    column_address = 0;
    size = size - read_size;
    p_data += read_size;
  }

}

int NAND_CheckDataInFlash()
{
  uint8_t data[4];
  uint32_t cast_data;
  NAND_ReadFromFlash(0, sizeof(data), data);
  cast_data = *(uint32_t*)&data;

  if(0xDEADBEEF == cast_data)
  {
    return 1;
  }

  return 0;
}

void NAND_Reset()
{
  uint8_t tx_buff[2];
  st_memdrv_info_t memdrv_info;

  /* 0-0. Reset Flash */
  tx_buff[0] = NAND_RESET;
  memdrv_info.cnt = 1;
  memdrv_info.p_data = tx_buff;

  NAND_CS_LOW;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  NAND_CS_HIGH;  //CS HIGH

  nand_wait_operation_complete();
}

nand_flash_status_t NAND_Erase()
{
  uint8_t tx_buff[4];
  uint8_t rx_buff[4];
  uint32_t addr;
  st_memdrv_info_t memdrv_info;
  int block;
  int isBadBlock = 0;

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  /* 0-0. Reset Flash */
  tx_buff[0] = NAND_RESET;
  memdrv_info.cnt = 1;
  memdrv_info.p_data = tx_buff;

  NAND_CS_LOW;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  NAND_CS_HIGH;  //CS HIGH

  /* 0-1. Wait until operation is completed */
  nand_wait_operation_complete();

  /* 0-2. Disable block lock bits */
  tx_buff[0] = NAND_SET_FEATURE;
  tx_buff[1] = NAND_BLOCK_LOCK_REG;
  tx_buff[2] = 0x00;  //Set block lock bits to 0, whole flash space is editable.
  memdrv_info.cnt = 3;
  memdrv_info.p_data = tx_buff;

  NAND_CS_LOW;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  NAND_CS_HIGH;  //CS HIGH

  R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

  //return;

  for(block = 0; block < NAND_NR_OF_BLOCKS; block++)
  {
    addr = block << 6;

    /* Do not erase bad blocks */
    for(int i = 0; i < NAND_MAX_BAD_BLOCKS; i++)
    {
      if(block == bad_blocks[i])
      {
        isBadBlock = 1;
        break;
      }
    }
    if(isBadBlock)
    {
      isBadBlock = 0;
      continue;
    }

    /* 1. Write Enable */
    tx_buff[0] = NAND_WRITE_ENABLE;
    memdrv_info.cnt = 1;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

    /* 2. Block Erase */
    tx_buff[0] = NAND_BLOCK_ERASE;
    tx_buff[1] = *((uint8_t*)&addr + 2);
    tx_buff[2] = *((uint8_t*)&addr + 1);
    tx_buff[3] = *((uint8_t*)&addr);
    memdrv_info.cnt = 4;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

    /* 3. Check Status */
    tx_buff[0] = NAND_GET_FEATURE;
    tx_buff[1] = NAND_STATUS_REG;
    memdrv_info.cnt = 2;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = 1;
    memdrv_info.p_data = rx_buff;
    R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    if(0 != (rx_buff[0] & NAND_STATUS_E_FAIL))
    {
      return NAND_ERASE_NOK;
      break;
    }

    /* 4. Wait until erase operation finishes */
    nand_wait_operation_complete();
  }

  return NAND_ERASE_OK;
}

nand_flash_status_t NAND_WriteToFlash(uint32_t address, uint32_t size, uint8_t *p_data)
{
  uint16_t column_address;
  uint32_t row_address;
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];
  uint8_t rx_buff[4];
  uint32_t write_size;

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;
  row_address = (uint16_t)(address / NAND_PAGE_SIZE);
  column_address = (uint16_t)(address % NAND_PAGE_SIZE);

  while(1)
  {
    if(0 == size)
    {
      return NAND_WRITE_OK;
    }

    if(size <= (NAND_PAGE_SIZE - column_address))
    {
      write_size = size;
    }
    else
    {
      write_size = NAND_PAGE_SIZE - column_address;
    }

    /* Do not write to bad blocks */
    for(int i = 0; i < NAND_MAX_BAD_BLOCKS; i++)
    {
      if((row_address >> 6) == bad_blocks[i])
      {
        // Bits 15:6 are block - mask is 0x7FC0
        // When we add one block we have to add bit 6 - 0x40
        row_address = (row_address & 0x7FC0) + 0x40;
        continue;
      }
    }

    /* 1. Write Enable */
    tx_buff[0] = NAND_WRITE_ENABLE;
    memdrv_info.cnt = 1;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

    /* 2. Program Load */
    tx_buff[0] = NAND_PROGRAM_LOAD;
    tx_buff[1] = *((uint8_t*)&column_address + 1);
    tx_buff[2] = *((uint8_t*)&column_address);
    memdrv_info.cnt = 3;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = write_size;
    memdrv_info.p_data = p_data;
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

    /* 3. Program Execute */
    tx_buff[0] = NAND_PROGRAM_EXECUTE;
    tx_buff[1] = *((uint8_t*)&row_address + 2);
    tx_buff[2] = *((uint8_t*)&row_address + 1);
    tx_buff[3] = *((uint8_t*)&row_address);
    memdrv_info.cnt = 4;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

    /* 4. Check Status */
    tx_buff[0] = NAND_GET_FEATURE;
    tx_buff[1] = NAND_STATUS_REG;
    memdrv_info.cnt = 2;
    memdrv_info.p_data = tx_buff;

    NAND_CS_LOW;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = 1;
    memdrv_info.p_data = rx_buff;
    R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH

    if(0 != (rx_buff[0] & (NAND_STATUS_P_FAIL | NAND_STATUS_ECCS0 | NAND_STATUS_ECCS1)))
    {
      return NAND_WRITE_NOK;
    }

    /* 3. Wait until write operation finishes */
    nand_flash_status_t status = nand_wait_operation_complete();
    if(status == NAND_READ_NOK)
    {
      return NAND_WRITE_NOK;
    }

    /* Prepare for next write if needed */
    row_address++;
    column_address = 0;
    size = size - write_size;
    p_data += write_size;
  }

}

nand_flash_status_t nand_wait_operation_complete()
{
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];
  uint8_t rx_buff[4];

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  tx_buff[0] = NAND_GET_FEATURE;
  tx_buff[1] = NAND_STATUS_REG;
  do
  {
    R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);
    NAND_CS_LOW;  //CS LOW
    memdrv_info.cnt = 2;
    memdrv_info.p_data = tx_buff;
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = 1;
    memdrv_info.p_data = rx_buff;
    R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
    NAND_CS_HIGH;  //CS HIGH
  }
  while(0 != (rx_buff[0] & NAND_STATUS_OIP));

  if(rx_buff[0] & NAND_STATUS_ECCS1)
    return NAND_READ_NOK;

  return NAND_READ_OK;
}

void NAND_LockFlash()
{
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  /* 1. Write Disable */
  tx_buff[0] = NAND_WRITE_DISABLE;
  memdrv_info.cnt = 1;
  memdrv_info.p_data = tx_buff;

  NAND_CS_LOW;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  NAND_CS_HIGH;  //CS HIGH

  R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

  /* 2. Enable block lock bits */
  tx_buff[0] = NAND_SET_FEATURE;
  tx_buff[1] = NAND_BLOCK_LOCK_REG;
  tx_buff[2] = 0xFF;  //Set block lock bits to 1, whole flash space is locked.
  memdrv_info.cnt = 3;
  memdrv_info.p_data = tx_buff;

  NAND_CS_LOW;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  NAND_CS_HIGH;  //CS HIGH

  nand_wait_operation_complete();
}

void NAND_UnlockFlash()
{
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  /* 1. Write Disable */
  tx_buff[0] = NAND_WRITE_DISABLE;
  memdrv_info.cnt = 1;
  memdrv_info.p_data = tx_buff;

  NAND_CS_LOW;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  NAND_CS_HIGH;  //CS HIGH

  R_BSP_SoftwareDelay(NAND_DELAY_TIME, NAND_DELAY_UNIT);

  /* 2. Disable block lock bits */
  tx_buff[0] = NAND_SET_FEATURE;
  tx_buff[1] = NAND_BLOCK_LOCK_REG;
  tx_buff[2] = 0;  //Set block lock bits to 1, whole flash space is locked.
  memdrv_info.cnt = 3;
  memdrv_info.p_data = tx_buff;

  NAND_CS_LOW;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  NAND_CS_HIGH;  //CS HIGH

  nand_wait_operation_complete();
}

nand_flash_status_t nand_check_if_write_ok(uint8_t *w_buff, uint8_t *r_buff, uint32_t size)
{
  nand_flash_status_t ret = NAND_READ_OK;

  for(uint32_t index = 0; index < size; index++)
  {
    if(*(w_buff + index) != *(r_buff + index))
    {
      ret = NAND_READ_NOK;
    }
  }

  return ret;
}
