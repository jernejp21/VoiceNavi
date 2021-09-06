/***********************************************************************
 *
 *  FILE        : Demo.c
 *  DATE        : 2021-07-02
 *  DESCRIPTION : Main Program
 *
 *  NOTE:THIS IS A TYPICAL EXAMPLE.
 *
 ***********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "r_smc_entry.h"
#include "platform.h"
#include "r_sci_iic_rx_if.h"
#include "r_cmt_rx_if.h"
#include "r_sys_time_rx_if.h"

void main();

void main(void)
{

  while(1)
  {
    /*if(isRead)
     {
     isRead = 0;
     i2c_rx[0] = I2C_Receive(i2c_reg_addr[0]);
     }

     if(isWrite)
     {
     isWrite = 0;
     I2C_Send(i2c_reg_addr[0], i2c_tx[0]);
     }*/

    /*for(int i = 0; i < 1000000; i++);
    PORT5.PODR.BIT.B4 ^= 1;
    for(int i = 0; i < 1000000; i++);
    PORT5.PODR.BIT.B1 ^= 1;
    for(int i = 0; i < 1000000; i++);
    PORT5.PODR.BIT.B3 ^= 1;
    for(int i = 0; i < 1000000; i++);
    PORT5.PODR.BIT.B2 ^= 1;*/

  }
}

