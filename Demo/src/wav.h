/*
 * Description here
 */

#ifndef __WAV_H
#define __WAV_H

#include <stdint.h>
extern int g_count;

typedef struct wav_header
{
	char riff[4];
	uint32_t file_size;
	char wave[4];
	char fmt[4];
	uint32_t d_len;
	uint16_t wave_type;
	uint16_t channel;
	uint32_t sample_rate;
	uint32_t avg_sample_rate;
	uint16_t block_align;
	uint16_t bps;
	char data[4];
	uint32_t data_size;
	uint32_t start_address;
}wav_header_t;

void WAV_Open(wav_header_t*, uint8_t*);

#endif //__WAV_H
