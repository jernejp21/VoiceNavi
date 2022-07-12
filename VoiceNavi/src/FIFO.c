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
#include "smc_gen/r_bsp/mcu/all/sbrk.h"

uint8_t *FIFO_buffer;
uint8_t FIFO_head, FIFO_tail, FIFO_size;
//uint8_t

void FIFO_Init(uint8_t size)
{
  // Create FIFO 1 element bigger than the size of FIFO queue. Easier to get it going.
  // malloc not working, so se are using sbrk
  FIFO_buffer = (uint8_t*)sbrk(size + 1);
  FIFO_size = size + 1;
}

void FIFO_Reset(void)
{
  FIFO_head = 0;
  FIFO_tail = 0;
}

int FIFO_Put(uint8_t *data, uint8_t size)
{
  for(int i = 0; i < size; i++)
  {
    if(FIFO_head == ((FIFO_tail - 1 + FIFO_size) % FIFO_size))
    {
      return FIFO_FULL; /* Queue Full*/
    }

    FIFO_buffer[FIFO_head] = *(data + i);

    FIFO_head = (FIFO_head + 1) % FIFO_size;
  }

  return FIFO_OK;  // No errors
}

int FIFO_Get(uint8_t *data, uint8_t size)
{
  for(int i = 0; i < size; i++)
  {
    if(FIFO_head == FIFO_tail)
    {
      return FIFO_EMPTY; /* Queue Empty - nothing to get*/
    }

    *(data + i) = FIFO_buffer[FIFO_tail];

    FIFO_tail = (FIFO_tail + 1) % FIFO_size;
  }

  return FIFO_OK;  // No errors
}
