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

#include "globals.h"

static uint32_t err_cmt_channel = 3;
static uint8_t isErrorPresent;

void ERROR_FileSystem()
{
  if(!isErrorPresent)
  {
    //500ms period
    LED_USBOn();
    LED_AlarmOn();
    LED_BusyOff();
    isErrorPresent = 1;
  }
}

void ERROR_WAVEFile()
{
  if(!isErrorPresent)
  {
    //500ms period
    R_CMT_CreatePeriodicAssignChannelPriority(2, &led_blink_alarm, err_cmt_channel, CMT_PRIORITY_14);
    LED_AlarmOff();
    LED_BusyOff();
    LED_USBOn();
    isErrorPresent = 1;
  }
}

void ERROR_FlashECS()
{
  if(!isErrorPresent)
  {
    //100ms period
    R_CMT_CreatePeriodicAssignChannelPriority(10, &led_blink_alarm, err_cmt_channel, CMT_PRIORITY_14);
    LED_AlarmOff();
    LED_BusyOff();
    LED_USBOff();
    isErrorPresent = 1;
  }
}

void ERROR_FlashEmpty()
{
  if(!isErrorPresent)
  {
    //500ms period
    R_CMT_CreatePeriodicAssignChannelPriority(2, &led_blink_alarm, err_cmt_channel, CMT_PRIORITY_14);
    LED_AlarmOff();
    LED_BusyOff();
    LED_USBOff();
    isErrorPresent = 1;
  }
}

void ERROR_ClearErrors()
{
  if(isErrorPresent)
  {
    R_CMT_Stop(err_cmt_channel);
    LED_AlarmOff();
    LED_USBOff();
    isErrorPresent = 0;
  }
}

void led_blink_busy()
{
  LED_BusyToggle();
}

void led_blink_usb()
{
  LED_USBToggle();
}

void led_blink_alarm()
{
  LED_AlarmToggle();
}
