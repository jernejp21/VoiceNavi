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

#ifndef __PLAY_MODES_H
#define __PLAY_MODES_H

void emptyPlay(uint8_t *i2c_gpio, uint8_t *songArray);
void errorResetPlay(uint8_t *i2c_gpio, uint8_t *songArray);
void normalPlay(uint8_t *i2c_gpio, uint8_t *songArray);
void lastInputInterruptPlay(uint8_t *i2c_gpio, uint8_t *songArray);
void priorityPlay(uint8_t *i2c_gpio, uint8_t *songArray);
void inputPlay(uint8_t *i2c_gpio, uint8_t *songArray);
void binary127ch_negative(uint8_t *i2c_gpio, uint8_t *songArray);
void binary250_positive(uint8_t *i2c_gpio, uint8_t *songArray);
void binary250_negative(uint8_t *i2c_gpio, uint8_t *songArray);
void binary255_positive(uint8_t *i2c_gpio, uint8_t *songArray);
void binary255_negative(uint8_t *i2c_gpio, uint8_t *songArray);
void binary255_5F9IH(uint8_t *i2c_gpio, uint8_t *songArray);
void IRQ_handler();

#endif //__PLAY_MODES_H
