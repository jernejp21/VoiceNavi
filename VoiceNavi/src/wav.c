/**
 * Voice Navi Firmware
 *
 * MIT License
 *
 * Copyright (c) 2022 Azurtest
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

/* WAV_Open parses wave header and inputs into into wav_header_t struct.
 *
 * Parameters: header - pointer to wav_header_t struct
 *             wav_address - pointer to start address of wav file.
 *             flash_address - pointer to flash address.
 */
wav_err_t WAV_Open(wav_header_t *header, uint8_t *wav_address, uint32_t *flash_address, uint32_t file_size)
{
  uint8_t isFmtAndData = 0;
  uint32_t cksize;
  uint32_t address = 0;
  uint32_t start_ck_address;
  uint32_t file_address = 0;

  uint8_t *temp_array = wav_address;  // size of this array is WAV_HEADER_SIZE

  if(0 != strncmp((const char*)&temp_array[address], "RIFF", 4))
  {
    return WAV_RIFF_ERR;
  }
  address += 4;
  memcpy((void*)&header->wave_cksize, &temp_array[address], 4);
  address += 4;

  if(0 != strncmp((const char*)&temp_array[address], "WAVE", 4))
  {
    return WAV_WAVE_ERR;
  }

  address += 4;
  do
  {
    if(0 == strncmp((const char*)&temp_array[address], "fmt ", 4))
    {
      //if fmt chunk

      /* Read new data from flash so we don't run out of data before parsing chunk.
       * Substitute WAV_HEADER_SIZE, because we added this in NAND_ReadFromFlash.
       * Next we add current address + 4 to get to the flash address of chunk size.
       */
      *flash_address = (*flash_address - WAV_HEADER_SIZE) + address + 4;
      NAND_ReadFromFlash(flash_address, WAV_HEADER_SIZE, temp_array);
      file_address += address;
      address = 0;

      memcpy((void*)&header->fmt_cksize, &temp_array[address], 4);
      start_ck_address = address + 4;
      cksize = header->fmt_cksize;
      address += 4;
      memcpy((void*)&header->wave_type, &temp_array[address], 2);
      address += 2;
      memcpy((void*)&header->channel, &temp_array[address], 2);
      address += 2;
      memcpy((void*)&header->sample_rate, &temp_array[address], 4);
      address += 4;
      //Average bytes per second. We don't need this.
      address += 4;
      //Byte align. We don't need this.
      address += 2;
      memcpy((void*)&header->bps, &temp_array[address], 2);
      address = start_ck_address + cksize;
      isFmtAndData++;
    }
    else
    {
      if(0 == strncmp((const char*)&temp_array[address], "data", 4))
      {
        //if data chunk

        /* Read new data from flash so we don't run out of data before parsing chunk.
         * Substitute WAV_HEADER_SIZE, because we added this in NAND_ReadFromFlash.
         * Next we add current address + 4 to get to the flash address of chunk size.
         */
        *flash_address = (*flash_address - WAV_HEADER_SIZE) + address + 4;
        NAND_ReadFromFlash(flash_address, WAV_HEADER_SIZE, temp_array);
        file_address += address;
        address = 0;

        memcpy((void*)&header->data_cksize, &temp_array[address], 4);
        start_ck_address = address + 4;
        header->data_address = *flash_address + start_ck_address - WAV_HEADER_SIZE;
        cksize = header->data_cksize;
        address = start_ck_address + cksize;
        isFmtAndData++;
      }
      else
      {
        address += 4;
        memcpy((void*)&cksize, &temp_array[address], 4);
        address += 4;

        // Check if next chunk is in array. If not, read new data from flash.
        if((address + cksize) > WAV_HEADER_SIZE)
        {
          // Calculate size from top of array to end of current chunk.
          cksize = address + cksize;
          file_address += address;
          address = 0;

          // Substitute WAV_HEADER_SIZE, because we added this in NAND_ReadFromFlash
          *flash_address = *flash_address + cksize - WAV_HEADER_SIZE;
          NAND_ReadFromFlash(flash_address, WAV_HEADER_SIZE, temp_array);
        }
        else
        {
          address += cksize;
        }

      }
    }

  }
  while((2 != isFmtAndData) && (file_address < file_size));

  if(isFmtAndData == 2)
  {
    if(header->fmt_cksize == 0)
    {
      return WAV_FMT_ERR;
    }
    if(header->data_cksize == 0)
    {
      return WAV_DATASIZE_ERR;
    }

    return WAV_NO_ERR;
  }
  else
  {
    return WAV_FILESIZE_ERR;
  }
}

/* placeNameToTable maps wav file name to corresponding position.
 *
 * Example:
 * 0,test1.wav
 *
 * Parameters: dest - pointer to destination array
 *             source - pointer to file line array
 */
void WAV_PlaceNameToTable(char *dest, char *source)
{
  uint8_t startCopy = 0;

  for(;; source++)
  {
    if(('\r' == *source) || ('\n' == *source))
    {
      //End of line, parsing completed.
      *dest = 0;
      return;
    }

    if(',' == *source)
    {
      //Relevant data starts after ",".
      startCopy = 1;
      continue;
    }

    if(startCopy)
    {
      //Copy char from source to destination array.
      *dest = *source;
      dest++;
    }
  }
}

/* placeSongsToTable creates playlist according to wpj file.
 *
 * Example:
 * 2,1,1,2,4
 * ch,vol_ctrl,replay,file_nr1 [,file_nr2...file_nr8]
 * Parameters: dest - pointer to destination array
 *             source - pointer to file line array
 */
void WAV_PlaceSongsToTable(playlist_t *dest, char *source)
{
  int commaCount = 0;
  uint8_t decimal = 1;
  uint8_t file_nr = 0;

  uint8_t multi;
  uint8_t newDec;

  for(;; source++)
  {
    if(('\r' == *source) || ('\n' == *source))
    {
      //End of line, parsing completed.
      dest->playlist_len = file_nr + 1;
      return;
    }

    if(',' == *source)
    {
      commaCount++;
      decimal = 1;
      if(commaCount > 3)
      {
        file_nr++;
      }
      continue;
    }

    switch(commaCount)
    {
      case 0:
        //Number before 1st comma is irrelevant.
        break;

      case 1:
        //After 1st comma we have volume control source.
        dest->vol_ctrl = (uint8_t)*source - '0';
        break;

      case 2:
        //After 2ne comma we repetition number.
        dest->repeat = (dest->repeat * decimal) + ((uint8_t)*source - '0');
        decimal = 10;
        break;

      default:
        //After 3rd comma and beyond we have file numbers in playlist.
        multi = dest->file_nr[file_nr] * decimal;
        newDec = (uint8_t)*source - '0';
        dest->file_nr[file_nr] = multi + newDec;
        decimal = 10;
        break;
    }
  }
}
