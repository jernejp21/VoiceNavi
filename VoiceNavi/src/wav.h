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

#ifndef __WAV_H
#define __WAV_H

#include <stdint.h>

/* Change parameters if needed */
#define MAX_NR_FILES 8 //Max number of files which can be stacked into single file to play
#define FILE_NAME_LEN 13 //Max length of file name - including ".wav" + 1 (for end of line symbol).

/* Do not change parameters below */
#define WAV_HEADER_SIZE 100  //WAVE file header size is 44 bytes.

/** WAVE file header structure */
typedef struct wav_header
{
  uint32_t wave_cksize;  //Size of wave chunk - the overall file -8 bytes, in bytes.
  uint32_t fmt_cksize;  //Size of format chunk. It can be 16, 18 or 40 bytes.
  uint16_t wave_type;  //Type of format (1 is PCM).
  uint16_t channel;  //Number of Channels
  uint32_t sample_rate;  //Number of Samples per second.
  uint16_t bps;  //Bits per sample
  uint32_t data_cksize;  //Size of the data chunk section.
  uint32_t data_address;  //Start of data chunk section.
} wav_header_t;

/** WAV error enum */
typedef enum wav_err
{
  WAV_NO_ERR = 0,
  WAV_RIFF_ERR = 1,
  WAV_FILESIZE_ERR = 2,
  WAV_WAVE_ERR = 3,
  WAV_FMT_ERR = 4,
  WAV_DATASIZE_ERR = 5,

} wav_err_t;

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
wav_err_t WAV_Open(wav_header_t *header, uint8_t *wav_address, uint32_t *flash_address, uint32_t file_size);
void WAV_PlaceSongsToTable(playlist_t*, char*);
void WAV_PlaceNameToTable(char*, char*);

#endif //__WAV_H
