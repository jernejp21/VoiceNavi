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
#define MAX_NR_OF_SWITCHES 12

#define STOP 0x01
#define STB 0x02

static uint16_t gpio_prev;

static uint8_t prev_sw = 255;
static uint8_t irqTriggered;
static uint8_t isDoubleSwitch;

static uint8_t bitOrder(uint8_t order)
{
  uint8_t new_order = 0;

  order ^= 0xFF;  //Reverse all bits, to show only active gpios (unpressed state).

  new_order = order >> 4;
  new_order |= (order & 0x01) << 7;
  new_order |= (order & 0x02) << 5;
  new_order |= (order & 0x04) << 3;
  new_order |= (order & 0x08) << 1;

  return new_order;
}

static uint8_t switchToPlay(uint32_t bitField, uint8_t *sw_array_p)
{
//Return value is number of switches pressed
  uint8_t cnt = 0;

  for(int index = 0; index < g_systemStatus.nr_of_switches; index++)
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

void emptyPlay(uint8_t *i2c_gpio)
{

}

void errorResetPlay(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get positive logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      SYSTEM.PRCR.WORD = 0xA502;
      SYSTEM.SWRR = 0x0000A501;
    }
  }
}

void normalPlay(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint16_t _gpio;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[MAX_NR_OF_SWITCHES] = {0};

  /* Check for switch status only when triggered */
  if(g_systemStatus.flag_isIRQ)
  {

    if(isDoubleSwitch != 1)
    {
      isDoubleSwitch = 1;
      return;
    }

    isDoubleSwitch = 0;
    _gpioa = i2c_gpio[0];
    _gpiob = i2c_gpio[1];

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    //Combine gpioa and gpiob. HW dependent.
    _gpio = ((_gpiob & 0x78) << 5) | _gpioa;

    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      g_systemStatus.flag_waitForInterval = 0;
    }
    else if((0 == g_systemStatus.flag_isPlaying) && (0 == g_systemStatus.flag_waitForInterval))
    {
      /* No interrupts. Play only if a song isn't played yet. */
      /* Get number of pressed switches and pressed positions. */
      _nr_sw_pressed = switchToPlay(_gpio, _sw_pressed);

      for(char sw_pos = 0; sw_pos < g_systemStatus.nr_of_switches; sw_pos++)
      {
        if(_sw_pressed[sw_pos])
        {
          /* If only one switch is pressed, play that song */
          if(_nr_sw_pressed == 1)
          {
            /* Return pressed switch number -1. */
            FIFO_Put(&sw_pos, 1);
            prev_sw = sw_pos;
            break;
          }
          else if(_nr_sw_pressed == 2)
          {
            /* If current SW position is different from previous,
             * return pressed switch. This is for alternating play.
             */
            if(sw_pos != prev_sw)
            {
              FIFO_Put(&sw_pos, 1);
              prev_sw = sw_pos;
              break;
            }
          }
        }
      }
    }
  }
  else
  {
    prev_sw = 255;
    isDoubleSwitch = 0;
  }
}

void lastInputInterruptPlay(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint16_t _gpio;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[MAX_NR_OF_SWITCHES] = {0};

  /* Check for switch status only when triggered */
  if(g_systemStatus.flag_isIRQ)
  {
    if(isDoubleSwitch != 1)
    {
      isDoubleSwitch++;
      return;
    }

    isDoubleSwitch = 0;
    _gpioa = i2c_gpio[0];
    _gpiob = i2c_gpio[1];

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    //Combine gpioa and gpiob. HW dependent.
    _gpio = ((_gpiob & 0x78) << 5) | _gpioa;

    /* Get number of pressed switches and pressed positions. */
    _nr_sw_pressed = switchToPlay(_gpio, _sw_pressed);

    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
    }
    else if(gpio_prev != _gpio)
    {
      if((1 == _nr_sw_pressed) || ((2 == _nr_sw_pressed) && (0 == irqTriggered)))
      {
        for(char sw_pos = 0; sw_pos < g_systemStatus.nr_of_switches; sw_pos++)
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
            FIFO_Put(&sw_pos, 1);
            prev_sw = sw_pos;
            break;
          }
        }
      }
    }
  }
  gpio_prev = _gpio;
  isDoubleSwitch = 0;
}

void priorityPlay(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint16_t _gpio;
  uint8_t _sw_pressed[MAX_NR_OF_SWITCHES] = {0};

  /* Check for switch status only when triggered */
  if(g_systemStatus.flag_isIRQ)
  {
    if(isDoubleSwitch != 1)
    {
      isDoubleSwitch++;
      return;
    }

    isDoubleSwitch = 0;
    _gpioa = i2c_gpio[0];
    _gpiob = i2c_gpio[1];

    /* Correct switches order according to HW design */
    _gpioa = bitOrder(_gpioa);
    _gpiob ^= 0xFF;  //Convert to positive logic.

    //Combine gpioa and gpiob. HW dependent.
    _gpio = ((_gpiob & 0x78) << 5) | _gpioa;

    /* Get number of pressed switches and pressed positions. */
    switchToPlay(_gpio, _sw_pressed);

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

      for(char sw_pos = 0; sw_pos < g_systemStatus.nr_of_switches; sw_pos++)
      {
        if(_sw_pressed[sw_pos])
        {
          if(sw_pos < prev_sw)
          {
            g_systemStatus.flag_isPlaying = 0;
            FIFO_Put(&sw_pos, 1);
            prev_sw = sw_pos;
            break;
          }
        }
      }
    }
  }
}

void inputPlay(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  uint16_t _gpio;
  uint8_t _nr_sw_pressed;
  uint8_t _sw_pressed[MAX_NR_OF_SWITCHES] = {0};

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpiob ^= 0xFF;  //Convert to positive logic.

  //Combine gpioa and gpiob. HW dependent.
  _gpio = ((_gpiob & 0x78) << 5) | _gpioa;

  /* Get number of pressed switches and pressed positions. */
  _nr_sw_pressed = switchToPlay(_gpio, _sw_pressed);

  /* Stop if switch isn't triggered */
  if(g_systemStatus.flag_isIRQ)
  {
    if((_nr_sw_pressed == 1) && (0 != (_gpiob & STOP)))
    {
      irqTriggered = 1;

      g_systemStatus.flag_isPlaying = 0;

    }
    else if(_nr_sw_pressed == 1)
    {
      if(!irqTriggered && !isDoubleSwitch)
      {
        for(char sw_pos = 0; sw_pos < g_systemStatus.nr_of_switches; sw_pos++)
        {
          if(_sw_pressed[sw_pos])
          {
            FIFO_Put(&sw_pos, 1);
            prev_sw = sw_pos;
            break;
          }
        }
      }
      else
      {
        isDoubleSwitch = 0;
        g_systemStatus.flag_isPlaying = 0;
      }
    }
    else
    {
      isDoubleSwitch = 1;
    }
  }
  else
  {
    g_systemStatus.flag_isPlaying = 0;
    irqTriggered = 0;
  }
}

void binary127ch_negative(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get positive logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      return;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      if(_gpioa > 0x7F)
      {
        //Ignore if 7th bit is on.
        return;
      }
      switch(_gpioa)
      {
        case 0x7F:
          //STOP
          g_systemStatus.flag_isPlaying = 0;
          break;

        default:
          //Subtract positive logic from 0x7E to get negative logic
          _gpioa = 0x7E - _gpioa;
          FIFO_Put(&_gpioa, 1);
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

void binary250_positive(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  //uint8_t vol_reduction[2] = {0, 1};

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get positive logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has positive logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      return;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        case 0x00:
          break;
        case 0xFF:
          //STOP
          g_systemStatus.flag_isPlaying = 0;
          break;

          //Not in use
        case 0xFE:
        case 0xFD:
        case 0xFC:
        case 0xFB:
          break;

          /* NOT IN USE
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
           */

        default:
          _gpioa = _gpioa - 1;
          FIFO_Put(&_gpioa, 1);
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

void binary250_negative(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;
  //uint8_t vol_reduction[2] = {0, 1};

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get positive logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      return;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        case 0x00:
          break;
        case 0xFF:
          //STOP
          g_systemStatus.flag_isPlaying = 0;
          break;

          //Not in use
        case 0xFE:
        case 0xFD:
        case 0xFC:
        case 0xFB:
          break;

          /* NOT IN USE
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
           */

        default:
          //Subtract positive logic from 0xFA to get negative logic
          _gpioa = 0xFA - _gpioa;
          FIFO_Put(&_gpioa, 1);
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

void binary255_positive(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get positive logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has positive logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      return;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        case 0x00:
          //STOP
          g_systemStatus.flag_isPlaying = 0;
          break;

        default:
          _gpioa = _gpioa - 1;
          FIFO_Put(&_gpioa, 1);
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

void binary255_negative(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get positive logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      return;
    }

    if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
    {
      switch(_gpioa)
      {
        case 0xFF:
          //STOP
          g_systemStatus.flag_isPlaying = 0;
          break;

        default:
          //Subtract positive logic from 0xFE to get negative logic
          _gpioa = 0xFE - _gpioa;
          FIFO_Put(&_gpioa, 1);
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

void binary255_5F9IH(uint8_t *i2c_gpio)
{
  uint8_t _gpioa;
  uint8_t _gpiob;

  _gpioa = i2c_gpio[0];
  _gpiob = i2c_gpio[1];

  /* Correct switches order according to HW design */
  _gpioa = bitOrder(_gpioa);
  _gpioa ^= 0xFF;  //Convert again, to get positive logic
  _gpiob ^= 0xFF;  //Convert to positive logic.

  /* This mode has negative logic */
  if(g_systemStatus.flag_isIRQ)
  {
    if(0 != (_gpiob & STOP))
    {
      g_systemStatus.flag_isPlaying = 0;
      return;
    }

    if(g_systemStatus.flag_isPlaying == 0)
    {
      if((0 != (_gpiob & STB)) && (prev_sw != (_gpiob & STB)))
      {
        switch(_gpioa)
        {
          //No STOP command
          case 0xFF:
            break;

          default:
            //Subtract positive logic from 0xFE to get negative logic
            _gpioa = 0xFE - _gpioa;
            FIFO_Put(&_gpioa, 1);
            break;
        }
        prev_sw = _gpiob & STB;
      }
      else if(0 == (_gpiob & STB))
      {
        prev_sw = 0;
      }
    }
  }
  else
  {
    prev_sw = 0;
  }
}
