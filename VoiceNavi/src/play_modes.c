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

#define MAX_BIN_BUFF_SIZE 20

#define STOP 0x01
#define STB 0x02

static uint8_t gpioa_prev;

static uint8_t prev_sw = 255;
static uint8_t irqTriggered;

static uint8_t bitOrder(uint8_t order)
{
  uint8_t new_order = 0;

  order ^= 0xFF;  //Reverse all bits, to show only active gpios.

  new_order = order >> 4;
  new_order |= (order & 0x01) << 7;
  new_order |= (order & 0x02) << 5;
  new_order |= (order & 0x04) << 3;
  new_order |= (order & 0x08) << 1;

  return new_order;
}

static uint8_t switchToPlay(uint16_t bitField, uint8_t *sw_array_p)
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

void emptyPlay(uint8_t *i2c_gpio, uint8_t *empty)
{

}

void normalPlay(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[8] = {0};

  /* Check for switch status only when triggered */
  if(g_systemStatus.flag_isIRQ)
  {
    _gpioa = *(i2c_gpio + 0);
    _gpiob = *(i2c_gpio + 1);

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
    }
    else if(!g_systemStatus.flag_isPlaying)
    {
      /* No interrupts. Play only if a song isn't played yet. */
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
            g_systemStatus.song_cnt = 1;
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
              g_systemStatus.song_cnt = 1;
              break;
            }
          }
        }
      }
    }
  }
}

void lastInputInterruptPlay(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[8] = {0};

  /* Check for switch status only when triggered */
  if(g_systemStatus.flag_isIRQ)
  {
    _gpioa = *(i2c_gpio + 0);
    _gpiob = *(i2c_gpio + 1);

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    /* Get number of pressed switches and pressed positions. */
    _nr_sw_pressed = switchToPlay(_gpioa, _sw_pressed);

    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
    }
    else if(gpioa_prev != _gpioa)
    {
      if((1 == _nr_sw_pressed) || ((2 == _nr_sw_pressed) && (0 == irqTriggered)))
      {
        for(char sw_pos = 0; sw_pos < 8; sw_pos++)
        {
          if(_sw_pressed[sw_pos])
          {
            if(prev_sw != sw_pos)
            {
              irqTriggered = 1;
            }
            else
            {
              irqTriggered = 0;
            }
            g_systemStatus.flag_isPlaying = 0;
            *songArray = sw_pos;
            prev_sw = sw_pos;
            g_systemStatus.song_cnt = 1;
            break;
          }
        }
      }
    }
  }
  gpioa_prev = _gpioa;
}

void priorityPlay(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _sw_pressed[8] = {0};

  /* Check for switch status only when triggered */
  if(g_systemStatus.flag_isIRQ)
  {
    _gpioa = *(i2c_gpio + 0);
    _gpiob = *(i2c_gpio + 1);

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    /* Get number of pressed switches and pressed positions. */
    switchToPlay(_gpioa, _sw_pressed);

    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
    }
    else
    {
      if(!g_systemStatus.flag_isPlaying)
      {
        prev_sw = 255;
      }

      for(char sw_pos = 0; sw_pos < 8; sw_pos++)
      {
        if(_sw_pressed[sw_pos])
        {
          if(sw_pos < prev_sw)
          {
            g_systemStatus.flag_isPlaying = 0;
            *songArray = sw_pos;
            prev_sw = sw_pos;
            g_systemStatus.song_cnt = 1;
            break;
          }
        }
      }
    }
  }
}

void inputPlay(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[8] = {0};

  _gpioa = *(i2c_gpio + 0);
  _gpiob = *(i2c_gpio + 1);

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* Get number of pressed switches and pressed positions. */
  _nr_sw_pressed = switchToPlay(_gpioa, _sw_pressed);

  /* Stop if switch isn't triggered */
  if(g_systemStatus.flag_isIRQ)
  {
    if((_nr_sw_pressed == 1) && (0 != (_gpiob & STOP)))
    {
      irqTriggered = 1;

      g_systemStatus.flag_isPlaying = 0;
      *songArray = -1;

    }
    else if(_nr_sw_pressed == 1)
    {
      if(!irqTriggered)
      {
        for(char sw_pos = 0; sw_pos < 8; sw_pos++)
        {
          if(_sw_pressed[sw_pos])
          {
            *songArray = sw_pos;
            prev_sw = sw_pos;
            g_systemStatus.song_cnt = 1;
            break;
          }
        }
      }
    }
  }
  else
  {
    g_systemStatus.flag_isPlaying = 0;
    *songArray = -1;
    irqTriggered = 0;
  }
}

void binary127ch_negative(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = *(i2c_gpio + 0);
  _gpiob = *(i2c_gpio + 1);

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      g_systemStatus.song_cnt = 0;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      _gpioa &= 0x7F;  //Pass through mask in case of 8-bit value (mistake).
      switch(_gpioa)
      {
        //STOP
        case 0x7F:
          *(songArray + g_systemStatus.song_cnt) = 0xFF;
          break;

          //Not in use
        case 0x00:
          break;

        default:
          if(g_systemStatus.song_cnt < MAX_BIN_BUFF_SIZE)
          {
            *(songArray + g_systemStatus.song_cnt) = _gpioa - 1;  //Convert to positive logic
            g_systemStatus.song_cnt++;
          }
          break;
      }
      prev_sw = _gpiob & STB;
    }
    else if(0 == (_gpiob & STB))
    {
      prev_sw = 0;
    }
  }
  else
  {
    prev_sw = 0;
  }
}

void binary250_positive(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t vol_reduction[2] = {0, 1};

  _gpioa = *(i2c_gpio + 0);
  _gpiob = *(i2c_gpio + 1);

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get negative logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      g_systemStatus.song_cnt = 0;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        //STOP
        case 0xFF:
          *(songArray + g_systemStatus.song_cnt) = 0xFF;
          break;

          //Not in use
        case 0xFE:
        case 0x00:
          break;

          //Full volume
        case 0xFD:
          //Rewrite existing address with 0, and write to next available address.
          vol_reduction[1] = 1;
          g_binary_vol_reduction = 1;
          NAND_UnlockFlash();
          NAND_WriteToFlash(g_binary_vol_reduction_address, 2, vol_reduction);
          NAND_LockFlash();
          g_binary_vol_reduction_address++;
          break;

          //Fifth volume
        case 0xFC:
          vol_reduction[1] = 5;
          g_binary_vol_reduction = 5;
          NAND_UnlockFlash();
          NAND_WriteToFlash(g_binary_vol_reduction_address, 2, vol_reduction);
          NAND_LockFlash();
          g_binary_vol_reduction_address++;
          break;

          //Half volume
        case 0xFB:
          vol_reduction[1] = 2;
          g_binary_vol_reduction = 2;
          NAND_UnlockFlash();
          NAND_WriteToFlash(g_binary_vol_reduction_address, 2, vol_reduction);
          NAND_LockFlash();
          g_binary_vol_reduction_address++;
          break;

        default:
          if(g_systemStatus.song_cnt < MAX_BIN_BUFF_SIZE)
          {
            *(songArray + g_systemStatus.song_cnt) = _gpioa - 1;
            g_systemStatus.song_cnt++;
          }
          break;
      }
      prev_sw = _gpiob & STB;
    }
    else if(0 == (_gpiob & STB))
    {
      prev_sw = 0;
    }
  }
  else
  {
    prev_sw = 0;
  }
}

void binary250_negative(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint8_t vol_reduction[2] = {0, 1};

  _gpioa = *(i2c_gpio + 0);
  _gpiob = *(i2c_gpio + 1);

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      g_systemStatus.song_cnt = 0;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        //STOP
        case 0xFF:
          *(songArray + g_systemStatus.song_cnt) = 0xFF;
          break;

          //Not in use
        case 0xFE:
        case 0x00:
          break;

          //Full volume
        case 0xFD:
          //Rewrite existing address with 0, and wite to next available address.
          vol_reduction[1] = 1;
          g_binary_vol_reduction = 1;
          NAND_UnlockFlash();
          NAND_WriteToFlash(g_binary_vol_reduction_address, 2, vol_reduction);
          NAND_LockFlash();
          g_binary_vol_reduction_address++;
          break;

          //Fifth volume
        case 0xFC:
          vol_reduction[1] = 5;
          g_binary_vol_reduction = 5;
          NAND_UnlockFlash();
          NAND_WriteToFlash(g_binary_vol_reduction_address, 2, vol_reduction);
          NAND_LockFlash();
          g_binary_vol_reduction_address++;
          break;

          //Half volume
        case 0xFB:
          vol_reduction[1] = 2;
          g_binary_vol_reduction = 2;
          NAND_UnlockFlash();
          NAND_WriteToFlash(g_binary_vol_reduction_address, 2, vol_reduction);
          NAND_LockFlash();
          g_binary_vol_reduction_address++;
          break;

        default:
          if(g_systemStatus.song_cnt < MAX_BIN_BUFF_SIZE)
          {
            *(songArray + g_systemStatus.song_cnt) = _gpioa - 1;
            g_systemStatus.song_cnt++;
          }
          break;
      }
      prev_sw = _gpiob & STB;
    }
    else if(0 == (_gpiob & STB))
    {
      prev_sw = 0;
    }
  }
  else
  {
    prev_sw = 0;
  }
}

void binary255_positive(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = *(i2c_gpio + 0);
  _gpiob = *(i2c_gpio + 1);

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get negative logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      g_systemStatus.song_cnt = 0;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        //STOP
        case 0xFF:
          *(songArray + g_systemStatus.song_cnt) = 0xFF;
          break;

          //Not in use
        case 0x00:
          break;

        default:
          if(g_systemStatus.song_cnt < MAX_BIN_BUFF_SIZE)
          {
            *(songArray + g_systemStatus.song_cnt) = _gpioa - 1;
            g_systemStatus.song_cnt++;
          }
          break;
      }
      prev_sw = _gpiob & STB;
    }
    else if(0 == (_gpiob & STB))
    {
      prev_sw = 0;
    }
  }
  else
  {
    prev_sw = 0;
  }
}

void binary255_negative(uint8_t *i2c_gpio, uint8_t *songArray)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = *(i2c_gpio + 0);
  _gpiob = *(i2c_gpio + 1);

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      g_systemStatus.song_cnt = 0;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        //STOP
        case 0xFF:
          *(songArray + g_systemStatus.song_cnt) = 0xFF;
          break;

          //Not in use
        case 0x00:
          break;

        default:
          if(g_systemStatus.song_cnt < MAX_BIN_BUFF_SIZE)
          {
            *(songArray + g_systemStatus.song_cnt) = _gpioa - 1;
            g_systemStatus.song_cnt++;
          }
          break;
      }
      prev_sw = _gpiob & STB;
    }
    else if(0 == (_gpiob & STB))
    {
      prev_sw = 0;
    }
  }
  else
  {
    prev_sw = 0;
  }
}
