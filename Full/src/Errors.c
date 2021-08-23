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
  R_TMR_various_Set_Frequency(1);
  g_counter = 0;
  R_TMR_various_Start();
  LED_USBOff();

  while(1)
  {
    if(g_counter == 500)
    {
      g_counter = 0;
      LED_BusyToggle();
    }
    if(g_isIRQ)
    {
      LED_AlarmOn();
    }
  }

}

void ERROR_WAVEFile()
{
  R_TMR_various_Set_Frequency(1);
  g_counter = 0;
  R_TMR_various_Start();
  LED_USBOff();

  while(1)
  {
    if(g_counter == 500)
    {
      g_counter = 0;
      LED_BusyToggle();
    }
  }
}

void ERROR_FlashECS()
{
  R_TMR_various_Set_Frequency(1);
  g_counter = 0;
  R_TMR_various_Start();
  LED_USBOff();

  /*while(1)
  {
    if(g_counter == 500)
    {
      g_counter = 0;
      LED_USBToggle();
    }
  }*/
}
