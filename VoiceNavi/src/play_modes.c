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

static uint8_t prev_sw = 255;
static uint8_t irqTriggered;


uint8_t normalPlay(uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[8] = {0};
  uint8_t _ret = 0;

  /* Check for switch status only when triggered */
  if(g_isIRQ)
  {
    _gpioa = g_i2c_gpio_rx[0];
    _gpiob = g_i2c_gpio_rx[1];

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    if(0 != (_gpiob & STOP))
    {
      g_playing = 0;
    }
    else
    {
      /* No interrupts. Play only if a song isn't played yet. */
      if(!g_playing)
      {
        /* Get number of pressed switches and pressed positions. */
        _nr_sw_pressed = switchToPlay(_gpioa, _sw_pressed);

        for(char sw_pos = 0; sw_pos < 8; sw_pos++)
        {
          if(_sw_pressed[sw_pos])
          {
            /* If only one switch is pressed, play that song */
            if(_nr_sw_pressed == 1)
            {
              /* Return pressed switch number -1. */
              *songArray = sw_pos;
              prev_sw = sw_pos;
              _ret = 1;
              break;
            }
            else if(_nr_sw_pressed == 2)
            {
              /* If current SW position is different from previous,
               * return pressed switch. This is for alternating play.
               */
              if(sw_pos != prev_sw)
              {
                *songArray = sw_pos;
                prev_sw = sw_pos;
                _ret = 1;
                break;
              }
            }
          }
        }
      }
    }
  }

  return _ret;
}

uint8_t lastInputInterruptPlay(uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[8] = {0};
  uint8_t _ret = 0;

  /* Check for switch status only when triggered */
  if(g_isIRQ)
  {
    _gpioa = g_i2c_gpio_rx[0];
    _gpiob = g_i2c_gpio_rx[1];

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    /* Get number of pressed switches and pressed positions. */
    _nr_sw_pressed = switchToPlay(_gpioa, _sw_pressed);

    if(0 != (_gpiob & STOP))
    {
      g_playing = 0;
    }
    else
    {
      if(_nr_sw_pressed == 1)
      {
        for(char sw_pos = 0; sw_pos < 8; sw_pos++)
        {
          if(_sw_pressed[sw_pos])
          {

            irqTriggered = 0;
            if(prev_sw == sw_pos)
            {
              *songArray = prev_sw;
              _ret = 1;
              break;
            }
            else
            {
              g_playing = 0;
              *songArray = sw_pos;
              prev_sw = sw_pos;
              gpioa_prev = _gpioa;
              _ret = 1;
              break;
            }
          }
        }
      }
      else if(_nr_sw_pressed == 2)
      {
        if(0 == irqTriggered)
        {
          for(char sw_pos = 0; sw_pos < 8; sw_pos++)
          {
            if(_sw_pressed[sw_pos])
            {

              if(prev_sw != sw_pos)
              {
                g_playing = 0;
                *songArray = sw_pos;
                prev_sw = sw_pos;
                irqTriggered = 1;
                _ret = 1;
                break;
              }
            }
          }
        }
      }
    }
  }

  return _ret;
}

uint8_t priorityPlay(uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _sw_pressed[8] = {0};
  uint8_t _ret = 0;

  /* Check for switch status only when triggered */
  if(g_isIRQ)
  {
    _gpioa = g_i2c_gpio_rx[0];
    _gpiob = g_i2c_gpio_rx[1];

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    /* Get number of pressed switches and pressed positions. */
    switchToPlay(_gpioa, _sw_pressed);

    if(0 != (_gpiob & STOP))
    {
      g_playing = 0;
    }
    else
    {
      if(!g_playing)
      {
        prev_sw = 255;
      }

      for(char sw_pos = 0; sw_pos < 8; sw_pos++)
      {
        if(_sw_pressed[sw_pos])
        {
          if((sw_pos < prev_sw))
          {
            g_playing = 0;
            *songArray = sw_pos;
            prev_sw = sw_pos;
            gpioa_prev = _gpioa;
            _ret = 1;
            break;
          }
          else
          {
            *songArray = prev_sw;
            _ret = 1;
            break;
          }
        }
      }
    }
  }

  return _ret;
}

uint8_t inputPlay(uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[8] = {0};
  uint8_t _ret = 0;

  _gpioa = g_i2c_gpio_rx[0];
  _gpiob = g_i2c_gpio_rx[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* Get number of pressed switches and pressed positions. */
  _nr_sw_pressed = switchToPlay(_gpioa, _sw_pressed);

  /* Stop if switch isn't triggered */
  if(g_isIRQ)
  {
    if((_nr_sw_pressed == 1) && (0 != (_gpiob & STOP)))
    {
      R_TPU0_Stop();
      LED_BusyOff();
      do
      {
        _gpioa = g_i2c_gpio_rx[0];
        _gpioa = bitOrder(_gpioa);
      }
      while(_gpioa);

      g_playing = 0;
      *songArray = -1;

    }
    else if(_nr_sw_pressed == 1)
    {
      for(char sw_pos = 0; sw_pos < 8; sw_pos++)
      {
        if(_sw_pressed[sw_pos])
        {
          *songArray = sw_pos;
          prev_sw = sw_pos;
          _ret = 1;
          break;
        }
      }
    }
  }
  else
  {
    g_playing = 0;
    *songArray = -1;
  }

  return _ret;
}

uint8_t binary128ch(uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _ret = 0;

  _gpioa = g_i2c_gpio_rx[0];
  _gpiob = g_i2c_gpio_rx[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_playing = 0;
      _ret = 0;
      g_song_cnt = 0;
    }

    if(0 != (_gpiob & STB))
    {
      _gpioa &= 0x7F;  //Pass through mask in case of 8-bit value (mistake).
      switch(_gpioa)
      {
        //STOP
        case 0x7F:
          *(songArray + g_song_cnt) = 0xFF;
          break;

          //Not in use
        case 0x00:
          break;

        default:
          *(songArray + g_song_cnt) = (_gpioa ^ 0x7F) - 1;  //Convert to positive logic
          g_song_cnt++;
          _ret = g_song_cnt;
          break;
      }
    }
  }

  return _ret;
}

uint8_t binary255_positive(uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _ret = 0;

  _gpioa = g_i2c_gpio_rx[0];
  _gpiob = g_i2c_gpio_rx[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_playing = 0;
      _ret = 0;
      g_song_cnt = 0;
    }

    if(0 != (_gpiob & STB))
    {
      switch(_gpioa)
      {
        //STOP
        case 0xFF:
          *(songArray + g_song_cnt) = 0xFF;
          break;

          //Not in use
        case 0xFE:
        case 0x00:
          break;

          //Full volume
        case 0xFD:
          break;

          //Fifth volume
        case 0xFC:
          break;

          //Half volume
        case 0xFB:
          break;

        default:
          *(songArray + g_song_cnt) = _gpioa - 1;
          g_song_cnt++;
          _ret = g_song_cnt;
          break;
      }
    }
  }

  return _ret;
}

uint8_t binary255_negative(uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _ret = 0;

  _gpioa = g_i2c_gpio_rx[0];
  _gpiob = g_i2c_gpio_rx[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_playing = 0;
      _ret = 0;
      g_song_cnt = 0;
    }

    if(0 != (_gpiob & STB))
    {
      switch(_gpioa)
      {
        //STOP
        case 0xFF:
          *(songArray + g_song_cnt) = 0xFF;
          break;

          //Not in use
        case 0xFE:
        case 0x00:
          break;

          //Full volume
        case 0xFD:
          break;

          //Fifth volume
        case 0xFC:
          break;

          //Half volume
        case 0xFB:
          break;

        default:
          *(songArray + g_song_cnt) = (_gpioa ^ 0xFF) - 1; //Convert to positive logic
          g_song_cnt++;
          _ret = g_song_cnt;
          break;
      }
    }
  }

  return _ret;
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

