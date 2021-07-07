/*
 * Description here
 */

#include "wav.h"

void WAV_Open(wav_header_t* header, uint8_t* address)
{
	int i;

	for(i = 0; i < 4; i++)
	{
		header->riff[i] = (char)(*address);
		address++;
	}

	header->file_size = *((uint32_t*)address);
	address += 4;

	for(i = 0; i < 4; i++)
	{
		header->wave[i] = (char)(*address);
		address++;
	}

	for(i = 0; i < 4; i++)
	{
		header->fmt[i] = (char)(*address);
		address++;
	}

	header->d_len = *((uint32_t*)address);
	address += 4;

	header->wave_type = *((uint16_t*)address);
	address += 2;

	header->channel = *((uint16_t*)address);
	address += 2;

	header->sample_rate = *((uint32_t*)address);
	address += 4;

	header->avg_sample_rate = *((uint32_t*)address);
	address += 4;

	header->block_align = *((uint16_t*)address);
	address += 2;

	header->bps = *((uint16_t*)address);
	address += 2;

	for(i = 0; i < 4; i++)
	{
		header->data[i] = (char)(*address);
		address++;
	}

	header->data_size = *((uint32_t*)address);

}
