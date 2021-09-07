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

#ifndef __WAV_H
#define __WAV_H

#include <stdint.h>

/* Change parameters if needed */
#define MAX_NR_FILES 8 //Max number of files which can be stacked into single file to play
#define FILE_NAME_LEN 13 //Max length of file name - including ".wav" + 1 (for end of line symbol).

/* Do not change parameters below */
#define WAV_HEADER_SIZE 44  //WAVE file header size is 44 bytes.

/** WAVE file header structure */
typedef struct wav_header
{
  char riff[4];  //Marks the file as a riff file.
  uint32_t file_size;  //Size of the overall file 8 bytes, in bytes.
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
} wav_header_t;

/** File name structure */
typedef struct file_name
{
  char file_name[FILE_NAME_LEN];
} file_name_t;

/** File data structure */
typedef struct file_meta_data
{
  uint32_t file_size;
  uint32_t address;
} file_meta_data_t;

/** Playlist structure */
typedef struct playlist
{
  uint8_t vol_ctrl;
  uint8_t repeat;
  uint8_t file_nr[MAX_NR_FILES];
  uint8_t playlist_len;
} playlist_t;

/** Global functions */
void WAV_Open(wav_header_t*, uint8_t*);
void WAV_PlaceSongsToTable(playlist_t*, char*);
void WAV_PlaceNameToTable(char*, char*);

#endif //__WAV_H
