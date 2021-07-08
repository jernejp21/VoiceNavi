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
