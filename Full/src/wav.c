/*
 * Description here
 */

#include "wav.h"

void WAV_Open(wav_header_t* header, uint8_t* address)
{
	int i;

	// Marks file as "RIFF" - if not, error
	for(i = 0; i < 4; i++)
	{
		header->riff[i] = (char)(*address);
		address++;
	}

	// File size
	header->file_size = *((uint32_t*)address);
	address += 4;

	// File type header - must be "WAVE", if not error
	for(i = 0; i < 4; i++)
	{
		header->wave[i] = (char)(*address);
		address++;
	}

	// "fmt " Format chunk marker - includes trailing null.
	for(i = 0; i < 4; i++)
	{
		header->fmt[i] = (char)(*address);
		address++;
	}

	// Length of format data
	header->d_len = *((uint32_t*)address);
	address += 4;

	// Type of format (must be 1 - PCM format)
	header->wave_type = *((uint16_t*)address);
	address += 2;

	// Number of channels
	header->channel = *((uint16_t*)address);
	address += 2;

	// Sample rate in Hz
	header->sample_rate = *((uint32_t*)address);
	address += 4;

	// 	(Sample Rate * BitsPerSample * Channels) / 8
	header->avg_sample_rate = *((uint32_t*)address);
	address += 4;

	// (BitsPerSample * Channels) / 8
	header->block_align = *((uint16_t*)address);
	address += 2;

	// Bits per sample
	header->bps = *((uint16_t*)address);
	address += 2;

	// "data" chunk header. Marks beginning of data section
	for(i = 0; i < 4; i++)
	{
		header->data[i] = (char)(*address);
		address++;
	}

	// Size of data section
	header->data_size = *((uint32_t*)address);
	address += 4;

	header->start_address = (uint32_t)address;

}

void placeNameToTable(char *dest, char *source)
{
  uint8_t startCopy = 0;

  for(;; source++)
  {
    if(('\r' == *source) || ('\n' == *source))
    {
      return;
    }

    if(',' == *source)
    {
      startCopy = 1;
      continue;
    }

    if(startCopy)
    {
      *dest = *source;
      dest++;
    }
  }
}

void placeSongsToTable(playlist_t *dest, char *source)
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
        break;

      case 1:
        dest->group = (uint8_t) *source - '0';
        break;

      case 2:
        dest->repeat = (dest->repeat * decimal) + ((uint8_t) *source - '0');
        decimal = 10;
        break;

      default:
        multi = dest->file_nr[file_nr] * decimal;
        newDec = (uint8_t)*source - '0';
        dest->file_nr[file_nr] = (dest->file_nr[file_nr] * decimal) + ((uint8_t) *source - '0');
        decimal = 10;
        break;
    }
  }
}