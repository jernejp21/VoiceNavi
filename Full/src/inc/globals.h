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
#define WAV_HEADER_SIZE 44  //WAVE file header size is 44 bytes.

extern int g_counter;
extern int g_readBuffer;
extern uint8_t g_file_data[FILE_SIZE];
extern wav_header_t g_wav_file;
extern uint32_t g_file_size;
extern uint32_t g_current_byte;
extern int g_playing;

#endif //__GLOBALS_H
