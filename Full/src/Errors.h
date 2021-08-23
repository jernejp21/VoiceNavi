/**
 * Voice Navi Firmware
 *
 * Errors.c module is module for error handlers inside code.
 *
 * Author: Jernej Pangerc (Azur Test)
 * Date: Aug 2021
 *
 * Copyright (C) 2021 Azur Test. All rights reserved
 */

#ifndef __ERRORS_H
#define __ERRORS_H

void ERROR_FileSystem();
void ERROR_WAVEFile();
void ERROR_FlashECS();

#endif //__ERRORS_H
