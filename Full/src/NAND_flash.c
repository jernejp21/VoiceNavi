#include <string.h>

#include "r_tfat_driver_rx_if.h"
#include "r_memdrv_rx_if.h"

#include "wav.h"
#include "NAND_flash.h"
#include "globals.h"

FATFS fs;
FIL file;
FIL file1;
FRESULT fr;
UINT size;

const uint8_t g_msc_file[15] = "0:sample02.wpj";
uint8_t flash_buffer[NAND_PAGE_SIZE];
uint8_t wav_buffer[NAND_PAGE_SIZE];
char line[100];

flash_custom_FAT_t flash_table[255];
wav_header_t wav_file;
int startOfFileNames;
int startOfPlaylist;

file_name_t file_name;

void NAND_CopyToFlash()
{

  int cnt = 0;
  char *fileName_p;
  uint32_t flash_address = NAND_PAGE_SIZE * 64;
  nand_flash_status_t flash_status;

  fr = f_mount(&fs, "", 0);
  fr = f_open(&file, (const TCHAR*) &g_msc_file, (FA_OPEN_ALWAYS | FA_READ));
  startOfFileNames = 0;
  startOfPlaylist = 0;
  while(f_gets((TCHAR*) &line, sizeof(line), &file))
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
      placeNameToTable((char*) &file_name.file_name, (char*) &line);
      fr = f_open(&file1, &file_name.file_name[0], FA_READ);
      fr = f_read(&file1, &wav_buffer[0], NAND_PAGE_SIZE, &size);
      WAV_Open(&wav_file, &wav_buffer[0]);
      flash_table[cnt].address = flash_address;
      flash_table[cnt].file_size = wav_file.file_size;

      do
      {
        flash_status = nand_copy_to_flash(flash_address, sizeof(wav_buffer), wav_buffer);
        flash_address += size;
        if(NAND_WRITE_NOK == flash_status)
        {
          break;
        }
        fr = f_read(&file1, &wav_buffer[0], NAND_PAGE_SIZE, &size);
      }
      while(size);

      flash_status = nand_copy_to_flash(flash_address, (wav_file.file_size % NAND_PAGE_SIZE), wav_buffer);
      flash_address += wav_file.file_size % NAND_PAGE_SIZE;
      f_close(&file1);
      cnt++;
    }

    if(startOfPlaylist)
    {
      placeSongsToTable((playlist_t*) &g_output_music[cnt], (char*) &line);
      cnt++;

      PORTD.PDR.BIT.B7 = 1;
      if(cnt > 254)
      {
        break;
      }
    }
  }
  f_close(&file);

  //Copy file table to NAND flash
  flash_status = nand_copy_to_flash(0, sizeof(flash_table), (uint8_t*) &flash_table[0]);

  //Copy playlist table to NAND flash
  flash_status = nand_copy_to_flash(NAND_PAGE_SIZE + 10, sizeof(g_output_music), (uint8_t*) &g_output_music[0]);

}

void NAND_ReadFromFlash(uint32_t address, uint32_t size, uint8_t *p_data)
{
  //nand_flash_status_t ret = NAND_WRITE_OK;
  uint16_t column_address;
  uint32_t row_address;
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];
  uint8_t rx_buff[4];

  if(0 == size)
  {
    return;  // ret;
  }

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;
  row_address = (uint16_t) (address / NAND_PAGE_SIZE);
  column_address = (uint16_t) (address % NAND_PAGE_SIZE);

  /* 1. Page Read to Cache */
  tx_buff[0] = NAND_PAGE_READ;
  tx_buff[1] = *((uint8_t*) &row_address + 2);
  tx_buff[2] = *((uint8_t*) &row_address + 1);
  tx_buff[3] = *((uint8_t*) &row_address);
  memdrv_info.cnt = 4;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  /* 2. Wait until read operation finishes */
  nand_wait_operation_complete();

  /* 3. Read from Cache */
  tx_buff[0] = NAND_READ_FROM_CACHE;
  tx_buff[1] = *((uint8_t*) &column_address + 1);
  tx_buff[2] = *((uint8_t*) &column_address);
  tx_buff[3] = 0;  //dummy byte
  memdrv_info.cnt = 4;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

  memdrv_info.cnt = size;
  memdrv_info.p_data = p_data;
  R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  return;  // ret;
}

int NAND_CheckDataInFlash()
{

  return 0;
}

nand_flash_status_t NAND_Erase()
{
  nand_flash_status_t ret = NAND_ERASE_OK;
  uint8_t tx_buff[4];
  uint8_t rx_buff[4];
  uint32_t addr;
  st_memdrv_info_t memdrv_info;
  int block;

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  /* 0-0. Reset Flash */
  tx_buff[0] = NAND_RESET;
  memdrv_info.cnt = 1;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  /* 0-1. Wait until operation is completed */
  nand_wait_operation_complete();

  /* 0-2. Disable block lock bits */
  tx_buff[0] = NAND_SET_FEATURE;
  tx_buff[1] = NAND_BLOCK_LOCK_REG;
  tx_buff[2] = 0x00;  //Set block lock bits to 0, whole flash space is editable.
  memdrv_info.cnt = 3;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);

  //return;

  for(block = 0; block < (NAND_NR_OF_BLOCKS / 2); block++)
  {
    addr = block << 6;

    /* 1. Write Enable */
    tx_buff[0] = NAND_WRITE_ENABLE;
    memdrv_info.cnt = 1;
    memdrv_info.p_data = tx_buff;

    PORTA.PODR.BIT.B4 = 0;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    PORTA.PODR.BIT.B4 = 1;  //CS HIGH

    R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);

    /* 2. Block Erase */
    tx_buff[0] = NAND_BLOCK_ERASE;
    tx_buff[1] = *((uint8_t*) &addr + 2);
    tx_buff[2] = *((uint8_t*) &addr + 1);
    tx_buff[3] = *((uint8_t*) &addr);
    memdrv_info.cnt = 4;
    memdrv_info.p_data = tx_buff;

    PORTA.PODR.BIT.B4 = 0;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
    PORTA.PODR.BIT.B4 = 1;  //CS HIGH

    R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);

    /* 3. Check Status */
    tx_buff[0] = NAND_GET_FEATURE;
    tx_buff[1] = NAND_STATUS_REG;
    memdrv_info.cnt = 2;
    memdrv_info.p_data = tx_buff;

    PORTA.PODR.BIT.B4 = 0;  //CS LOW
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = 1;
    memdrv_info.p_data = rx_buff;
    R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
    PORTA.PODR.BIT.B4 = 1;  //CS HIGH

    if(0 != (rx_buff[0] & NAND_STATUS_E_FAIL))
    {
      ret = NAND_ERASE_NOK;
      break;
    }

    /* 4. Wait until erase operation finishes */
    nand_wait_operation_complete();
  }

  return ret;
}

nand_flash_status_t nand_copy_to_flash(uint32_t address, uint32_t size, uint8_t *p_data)
{
  nand_flash_status_t ret = NAND_WRITE_OK;
  uint16_t column_address;
  uint32_t row_address;
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];
  uint8_t rx_buff[4];

  if(0 == size)
  {
    return ret;
  }

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;
  row_address = (uint16_t) (address / NAND_PAGE_SIZE);
  column_address = (uint16_t) (address % NAND_PAGE_SIZE);

  /* 1. Write Enable */
  tx_buff[0] = NAND_WRITE_ENABLE;
  memdrv_info.cnt = 1;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);

  /* 2. Program Load */
  tx_buff[0] = NAND_PROGRAM_LOAD;
  tx_buff[1] = *((uint8_t*) &column_address + 1);
  tx_buff[2] = *((uint8_t*) &column_address);
  memdrv_info.cnt = 3;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

  memdrv_info.cnt = size;
  memdrv_info.p_data = p_data;
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);

  /* 3. Program Execute */
  tx_buff[0] = NAND_PROGRAM_EXECUTE;
  tx_buff[1] = *((uint8_t*) &row_address + 2);
  tx_buff[2] = *((uint8_t*) &row_address + 1);
  tx_buff[3] = *((uint8_t*) &row_address);
  memdrv_info.cnt = 4;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);

  /* 4. Check Status */
  tx_buff[0] = NAND_GET_FEATURE;
  tx_buff[1] = NAND_STATUS_REG;
  memdrv_info.cnt = 2;
  memdrv_info.p_data = tx_buff;

  PORTA.PODR.BIT.B4 = 0;  //CS LOW
  R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

  memdrv_info.cnt = 1;
  memdrv_info.p_data = rx_buff;
  R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
  PORTA.PODR.BIT.B4 = 1;  //CS HIGH

  if(0 != (rx_buff[0] & NAND_STATUS_P_FAIL))
  {
    ret = NAND_WRITE_NOK;
  }

  /* 3. Wait until write operation finishes */
  nand_wait_operation_complete();

  return ret;
}

void nand_wait_operation_complete()
{
  st_memdrv_info_t memdrv_info;
  uint8_t tx_buff[4];
  uint8_t rx_buff[4];

  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  tx_buff[0] = NAND_GET_FEATURE;
  tx_buff[1] = NAND_STATUS_REG;
  do
  {
    R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);
    PORTA.PODR.BIT.B4 = 0;  //CS LOW
    memdrv_info.cnt = 2;
    memdrv_info.p_data = tx_buff;
    R_MEMDRV_Tx(NAND_DEVNO, &memdrv_info);

    memdrv_info.cnt = 1;
    memdrv_info.p_data = rx_buff;
    R_MEMDRV_Rx(NAND_DEVNO, &memdrv_info);
    PORTA.PODR.BIT.B4 = 1;  //CS HIGH
  }
  while(0 != (rx_buff[0] & NAND_STATUS_OIP));
}
