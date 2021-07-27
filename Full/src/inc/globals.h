/*
 * This file contains all globals that are used over multiple c files.
 * This is due to interrupt routines are in seperate file than main function.
 *
 * Author: Jernej Pangerc
 * Created on (DD.MM.YYYY): 8.7.2021
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "../wav.h"

#define FILE_SIZE (32000)


extern int g_counter;
extern int g_readBuffer;
extern uint8_t g_file_data[FILE_SIZE];
extern wav_header_t g_wav_file;
extern uint32_t g_file_size;
extern uint32_t g_current_byte;
extern int g_playing;
extern playlist_t g_output_music[255];
extern file_meta_data_t g_file_meta_data[255];

#endif //__GLOBALS_H
