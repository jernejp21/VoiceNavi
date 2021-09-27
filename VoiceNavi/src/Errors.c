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

#include "globals.h"

void ERROR_FileSystem()
{
  //500ms period
  R_CMT_CreatePeriodic(2, &led_blink_busy, 0);
  LED_USBOff();

  while(1)
  {
    if(g_systemStatus.flag_isIRQ)
    {
      LED_AlarmOn();
    }
  }
}

void ERROR_WAVEFile()
{
  //500ms period
  R_CMT_CreatePeriodic(2, &led_blink_busy, 0);
  LED_USBOff();

  while(1);
}

void ERROR_FlashECS()
{
  //500ms period
  R_CMT_CreatePeriodic(2, &led_blink_usb, 0);
  LED_AlarmOn();

  while(1);
}

void led_blink_busy()
{
  LED_BusyToggle();
}

void led_blink_usb()
{
  LED_USBToggle();
}
