/**
 * Voice Navi Firmware
 *
 * Errors.h module is module for error handlers inside code.
 *
 * Author: Jernej Pangerc (Azur Test)
 * Date: Aug 2021
 *
 * Copyright (C) 2021 Azur Test. All rights reserved
 */

#include "globals.h"

void ERROR_FileSystem()
{
  //500ms period
  R_CMT_CreatePeriodic(2, &led_blink_busy, 0);
  LED_USBOff();

  while(1)
  {
    if(g_isIRQ)
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
