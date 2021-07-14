/*
 * Description here
 */

#ifndef __WAV_H
#define __WAV_H

#include <stdint.h>

/* WAVE file header structure */
typedef struct wav_header
{
	char riff[4];  //Marks the file as a riff file.
	uint32_t file_size;  //Size of the overall file -8 bytes, in bytes.
	char wave[4];  //File Type Header. It always equals "WAVE".
	char fmt[4];  //Format chunk marker. Includes trailing null
	uint32_t d_len;  //Length of format data as listed above
	uint16_t wave_type;  //Type of format (1 is PCM).
	uint16_t channel;  //Number of Channels
	uint32_t sample_rate;  //Number of Samples per second.
	uint32_t avg_sample_rate;  //(Sample Rate * BitsPerSample * Channels) / 8
	uint16_t block_align;  //(BitsPerSample * Channels) / 8
	uint16_t bps;  //Bits per sample
	char data[4];  //"data" chunk header. Marks the beginning of the data section.
	uint32_t data_size;  //Size of the data section.
	uint32_t start_address;  //Start address of data section.
}wav_header_t;

/* File name order (position) structure */
#define FILE_NAME_LEN 8
typedef struct file_name_pos
{
  char file_name[FILE_NAME_LEN];
} file_name_pos_t;

/* Playlist structure */
#define MAX_NR_FILES 8
typedef struct playlist
{
  uint8_t group;
  uint8_t repeat;
  uint8_t file_nr[MAX_NR_FILES];
  uint8_t playlist_len;
} playlist_t;

void WAV_Open(wav_header_t*, uint8_t*);
void placeSongsToTable(playlist_t*, char*);
void placeNameToTable(char*, char*);

#endif //__WAV_H
