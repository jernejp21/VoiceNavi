/**
 * Voice Navi Firmware
 *
 * Different play modes for Voice Navi
 *
 * Author: Jernej Pangerc (Azur Test)
 * Date: Aug 2021
 *
 * Copyright (C) 2021 Azur Test. All rights reserved
 */

#include "globals.h"
#define STOP 0x01
#define STB 0x02

uint8_t gpioa_prev;
uint8_t gpiob_prev;

int16_t normalPlay()
{
  uint8_t gpioa;
  uint8_t gpiob;
  int16_t ret = -1;

  gpioa = I2C_Receive(0x07);
  gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  gpioa = (gpioa >> 4) | (gpioa << 4);

  if(0 != (gpiob & STOP))
  {
    R_TMR_play_Stop();
    g_playing = 0;
    g_stopPlaying = 1;
    ret = -1;
  }

  I2C_Receive(0x09);  //Clear INT flags
  I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {
    if(0 != (gpiob & STOP))
    {
      R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
    }
    else
    {
      if(!g_playing)
      {
        //g_stopPlaying = 0;
        ret = gpioa;
      }
    }

  }

  return ret;
}

int16_t lastInputPlay()
{
  uint8_t gpioa;
  uint8_t gpiob;
  int16_t ret = -1;

  gpioa = I2C_Receive(0x07);
  gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  gpioa = (gpioa >> 4) | (gpioa << 4);

  I2C_Receive(0x09);  //Clear INT flags
  I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {
    if(gpioa_prev == gpioa)
    {
      //do nothing. correct song is playing
      ret = gpioa_prev;
    }
    else
    {
      R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      ret = gpioa;
    }

    if(0 != (gpiob & STOP))
    {
      R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      ret = -1;
    }
  }

  gpioa_prev = gpioa;
  gpiob_prev = gpiob;

  return ret;
}

int16_t priorityPlay()
{
  uint8_t gpioa;
  uint8_t gpiob;
  uint8_t prio_order;
  int16_t ret = -1;

  gpioa = I2C_Receive(0x07);
  gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  gpioa = (gpioa >> 4) | (gpioa << 4);

  I2C_Receive(0x09);  //Clear INT flags
  I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {

    if(gpioa_prev == gpioa)
    {
      //do nothing. correct song is playing
      ret = gpioa_prev;
    }
    else
    {
      for(int bit = 0; bit < 8; bit++)
      {
        ret = gpioa & (1 << bit);
        if(ret)
        {
          R_TMR_play_Stop();
          g_playing = 0;
          g_stopPlaying = 1;
          break;
        }
      }
    }

    /* If STOP, stop immediately */
    if(0 != (gpiob & STOP))
    {
      R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      ret = -1;
    }
  }

  gpioa_prev = gpioa;
  gpiob_prev = gpiob;

  return ret;
}

int16_t inputPlay()
{
  uint8_t gpioa;
  uint8_t gpiob;
  int16_t ret = -1;

  gpioa = I2C_Receive(0x07);
  gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  gpioa = (gpioa >> 4) | (gpioa << 4);

  I2C_Receive(0x09);  //Clear INT flags
  I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {
    /* If STOP, stop immediately */
    if(0 != (gpiob & STOP))
    {
      R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      ret = -1;
      gpiob_prev = 1;
    }
    else
    {
      if(gpioa != gpioa_prev)
      {
        if((0 == (gpioa % 2)) || (gpioa == 1))
        {
          ret = gpioa;
          gpioa_prev = gpioa;

          R_TMR_play_Stop();
          g_playing = 0;
          g_stopPlaying = 1;
        }
      }
      else
      {
        ret = gpioa_prev;
      }
    }
  }
  else
  {
    R_TMR_play_Stop();
    g_playing = 0;
    g_stopPlaying = 1;
    gpiob_prev = 0;
    gpioa_prev = 0;
  }

  if(gpiob_prev)
  {
    ret = -1;
  }

  return ret;
}

int16_t binary128ch()
{
  uint8_t gpioa;
  uint8_t gpiob;
  int16_t ret = -1;

  gpioa = I2C_Receive(0x07);
  gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  gpioa = (gpioa >> 4) | (gpioa << 4);

  I2C_Receive(0x09);  //Clear INT flags
  I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {
    if(0 != (gpiob & STB))
    {
      if(gpioa == 0)
      {
        //stop
        ret = -1;
      }
      else
      {
        //Put song to fifo.
        ret = gpioa;
      }
    }

    if(0 != (gpiob & STOP))
    {
      R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      ret = -1;
    }
  }

  return -1;
}

int16_t binary255_positive()
{
  return -1;
}

int16_t binary255_negative()
{
  return -1;
}
