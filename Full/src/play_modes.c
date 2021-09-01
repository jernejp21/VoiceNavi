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

static uint8_t gpioa_prev;
static uint8_t gpiob_prev;
static uint8_t prev_sw = 255;

int16_t normalPlay()
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  int16_t _ret = -1;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[8] = {0};

  _gpioa = g_i2c_gpio_rx[0];
  _gpiob = g_i2c_gpio_rx[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  if(0 != (_gpiob & STOP))
  {
    R_TPU0_Stop();
    g_playing = 0;
  }
  else
  {
    /* No interrupts. Play only if a song isn't played yet. */
    if(!g_playing)
    {
      /* Get number of pressed switches and pressed positions. */
      _nr_sw_pressed = switchToPlay(_gpioa, _sw_pressed);

      /* If only one switch is pressed, play that song */
      if(_nr_sw_pressed == 1)
      {
        for(char sw_pos = 0; sw_pos < 8; sw_pos++)
        {
          if(_sw_pressed[sw_pos])
          {
            /* Return pressed switch number -1. */
            _ret = sw_pos;
            prev_sw = sw_pos;
            break;
          }
        }
      }
      else if(_nr_sw_pressed == 2)
      {
        for(char sw_pos = 0; sw_pos < 8; sw_pos++)
        {
          if(_sw_pressed[sw_pos])
          {
            /* If current SW position is different from previous,
             * return pressed switch. This is for alternating play.
             */
            if(sw_pos != prev_sw)
            {
              _ret = sw_pos;
              prev_sw = sw_pos;
              break;
            }
          }
        }
      }
    }
  }

  return _ret;
}

int16_t lastInputPlay()
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  int16_t _ret = -1;

  //_gpioa = I2C_Receive(0x07);
  //_gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  _gpioa = (_gpioa >> 4) | (_gpioa << 4);

  //I2C_Receive(0x09);  //Clear INT flags
  //I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {
    if(gpioa_prev == _gpioa)
    {
      //do nothing. correct song is playing
      _ret = gpioa_prev;
    }
    else
    {
      //R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      _ret = _gpioa;
    }

    if(0 != (_gpiob & STOP))
    {
      //R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      _ret = -1;
    }
  }

  gpioa_prev = _gpioa;
  gpiob_prev = _gpiob;

  return _ret;
}

int16_t priorityPlay()
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t prio_order;
  int16_t _ret = -1;

  _gpioa = g_i2c_gpio_rx[0];
  _gpiob = g_i2c_gpio_rx[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  //I2C_Receive(0x09);  //Clear INT flags
  //I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {

    if(gpioa_prev == _gpioa)
    {
      //do nothing. correct song is playing
      _ret = gpioa_prev;
    }
    else
    {
      for(int bit = 0; bit < 8; bit++)
      {
        _ret = _gpioa & (1 << bit);
        if(_ret)
        {
          //R_TMR_play_Stop();
          g_playing = 0;
          g_stopPlaying = 1;
          break;
        }
      }
    }

    /* If STOP, stop immediately */
    if(0 != (_gpiob & STOP))
    {
      //R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      _ret = -1;
    }

    gpioa_prev = _gpioa;
    gpiob_prev = _gpiob;
  }

  return _ret;
}

int16_t inputPlay()
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  int16_t _ret = -1;

  //_gpioa = I2C_Receive(0x07);
  //_gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  _gpioa = (_gpioa >> 4) | (_gpioa << 4);

  //I2C_Receive(0x09);  //Clear INT flags
  //I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {
    /* If STOP, stop immediately */
    if(0 != (_gpiob & STOP))
    {
      //R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      _ret = -1;
      gpiob_prev = 1;
    }
    else
    {
      if(_gpioa != gpioa_prev)
      {
        if((0 == (_gpioa % 2)) || (_gpioa == 1))
        {
          _ret = _gpioa;
          gpioa_prev = _gpioa;

          //R_TMR_play_Stop();
          g_playing = 0;
          g_stopPlaying = 1;
        }
      }
      else
      {
        _ret = gpioa_prev;
      }
    }
  }
  else
  {
    //R_TMR_play_Stop();
    g_playing = 0;
    g_stopPlaying = 1;
    gpiob_prev = 0;
    gpioa_prev = 0;
  }

  if(gpiob_prev)
  {
    _ret = -1;
  }

  return _ret;
}

int16_t binary128ch()
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  int16_t _ret = -1;

  //_gpioa = I2C_Receive(0x07);
  //_gpiob = I2C_Receive(0x17);

  /* Correct switches order according to HW design */
  _gpioa = (_gpioa >> 4) | (_gpioa << 4);

  //I2C_Receive(0x09);  //Clear INT flags
  //I2C_Receive(0x19);  //Clear INT flags

  if(g_isIRQ)
  {
    if(0 != (_gpiob & STB))
    {
      if(_gpioa == 0)
      {
        //stop
        _ret = -1;
      }
      else
      {
        //Put song to fifo.
        _ret = _gpioa;
      }
    }

    if(0 != (_gpiob & STOP))
    {
      //R_TMR_play_Stop();
      g_playing = 0;
      g_stopPlaying = 1;
      _ret = -1;
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

uint8_t bitOrder(uint8_t order)
{
  uint8_t new_order = 0;

  order ^= 0xFF;  //Reverse all bits, to show only active gpios.

  new_order = order >> 4;
  new_order |= (order & 0x01) << 7;
  new_order |= (order & 0x02) << 6;
  new_order |= (order & 0x04) << 5;
  new_order |= (order & 0x08) << 4;

  return new_order;
}

uint8_t switchToPlay(uint16_t bitField, uint8_t *sw_array_p)
{
  //Return value is number of switches pressed
  uint8_t cnt = 0;

  for(int index = 0; index < 8; index++)
  {
    if(bitField & 1)
    {
      *(sw_array_p + index) = 1;
      cnt++;
    }

    bitField = bitField >> 1;
  }

  return cnt;
}

