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
void checkGPIO();
void callBack_read();
void callBack_write();
void I2C_Send(uint8_t reg_add, uint8_t value);
uint8_t I2C_Receive(uint8_t reg_add);

#define I2C_ADDR 0x21
#define I2C_READ ((I2C_ADDR << 1) | 0x01)
#define I2C_WRITE ((I2C_ADDR << 1) | 0x00)

volatile sci_iic_return_t ret;
sci_iic_info_t iic_info;
__attribute__((aligned))
uint16_t g_buff[2];
uint8_t i2c_rx[5];
uint8_t i2c_tx[5];
uint8_t i2c_address[1] = {I2C_ADDR};
uint8_t i2c_reg_addr[1];

int isRead = 1;
int isWrite = 0;
int g_irqFlag = 0;

void timerCallback();
void timeCallback();

uint32_t tim_channel;
bool ret_val;

/* start of main func */
void main(void)
{
  printf("Entered main()\n");

  PORT5.PDR.BIT.B1 = 1;
  PORT5.PDR.BIT.B2 = 1;
  PORT5.PDR.BIT.B3 = 1;
  PORT5.PDR.BIT.B4 = 1;

  PORT5.PODR.BIT.B1 = 1;
  PORT5.PODR.BIT.B2 = 1;
  PORT5.PODR.BIT.B3 = 1;
  PORT5.PODR.BIT.B4 = 1;

  /*i2c_reg_addr[0] = 0x0A;
   i2c_rx[0] = 255;

   iic_info.callbackfunc = &callBack_read;
   iic_info.ch_no = 11;
   iic_info.cnt1st = 1;
   iic_info.cnt2nd = 2;
   iic_info.dev_sts = SCI_IIC_NO_INIT;
   iic_info.p_data1st = i2c_reg_addr;
   iic_info.p_data2nd = i2c_rx;
   iic_info.p_slv_adr = i2c_address;

   ret = R_SCI_IIC_Open(&iic_info);

   //IOCON
   I2C_Send(0x0A, 0xC2);

   //IODIRA
   I2C_Send(0x00, 0x00);
   //IOPOLA
   I2C_Send(0x01, 0);
   //GPINTENA
   //I2C_Send(0x02, 0xFF);
   //DEFVALA
   //I2C_Send(0x03, 0xFF);
   //INTCONA
   //I2C_Send(0x04, 0xFF);
   //GPPUA
   //I2C_Send(0x06, 0xFF);

   //IODIRB
   I2C_Send(0x10, 0x00);
   //IOPOLB
   //I2C_Send(0x11, 0);
   //GPINTENB
   //I2C_Send(0x12, 0xFF);
   //DEFVALB
   //I2C_Send(0x13, 0xFF);
   //INTCONB
   //I2C_Send(0x14, 0xFF);
   //GPPUB
   //I2C_Send(0x16, 0xFF);

   //Clear interrupts
   I2C_Receive(0x09);
   I2C_Receive(0x19);

   R_Config_ICU_IRQ13_Start();*/

  //R_BSP_InterruptsEnable();
  R_Config_DMAC0_Start();
  R_Config_S12AD0_Start();

  //R_SYS_TIME_Open();
  ret_val = R_CMT_CreatePeriodic(4, &timerCallback, &tim_channel);
  ret_val = R_CMT_CreatePeriodic(2, &timeCallback, &tim_channel);

  SYS_TIME time;
  time.min = 37;
  time.hour = 13;
  time.day = 25;
  time.month = 8;
  time.year = 2021;
  time.time_zone[0] = 1;
  time.time_zone[1] = 9;

  //R_SYS_TIME_SetCurrentTime(&time);



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

void timerCallback()
{
  R_BSP_InterruptsEnable();
  PORT5.PODR.BIT.B2 ^= 1;
}

void timeCallback()
{
  R_BSP_InterruptsEnable();
  //SYS_TIME time;
  //R_SYS_TIME_GetCurrentTime(&time);
  PORT5.PODR.BIT.B3 ^= 1;
}

/*void r_Config_ICU_irq13_interrupt(void)
 {
 R_BSP_InterruptsEnable();
 R_Config_ICU_IRQ13_Stop();
 checkGPIO();
 R_Config_ICU_IRQ13_Start();
 }*/

void checkGPIO()
{
  //Read interrupt flag register
  i2c_rx[0] = I2C_Receive(0x07);
  i2c_rx[1] = I2C_Receive(0x17);

  switch(i2c_rx[0])
  {
    case 0x01:
      printf("GPIOA 0\n");
      break;

    case 0x02:
      printf("GPIOA 1\n");
      break;

    case 0x04:
      printf("GPIOA 2\n");
      break;

    case 0x08:
      printf("GPIOA 3\n");
      break;

    case 0x10:
      printf("GPIOA 4\n");
      break;

    case 0x20:
      printf("GPIOA 5\n");
      break;

    case 0x40:
      printf("GPIOA 6\n");
      break;

    case 0x80:
      printf("GPIOA 7\n");
      break;

    default:
      break;
  }

  switch(i2c_rx[1])
  {
    case 0x01:
      printf("GPIOB 0\n");
      break;

    case 0x02:
      printf("GPIOB 1\n");
      break;

    case 0x04:
      printf("GPIOB 2\n");
      break;

    case 0x08:
      printf("GPIOB 3\n");
      break;

    case 0x10:
      printf("GPIOB 4\n");
      break;

    case 0x20:
      printf("GPIOB 5\n");
      break;

    case 0x40:
      printf("GPIOB 6\n");
      break;

    case 0x80:
      printf("GPIOB 7\n");
      break;

    default:
      break;
  }

  //Clear interrupts; wait until there is no signal on pin.
  do
  {
    i2c_rx[0] = I2C_Receive(0x09);
    i2c_rx[0] = I2C_Receive(0x19);
    i2c_rx[1] = I2C_Receive(0x07);
  }
  while((i2c_rx[0] ^ 0xff) | (i2c_rx[1] ^ 0xff));

}

uint8_t I2C_Receive(uint8_t reg_add)
{
  uint8_t rx;

  iic_info.cnt1st = 1;
  iic_info.cnt2nd = 1;
  iic_info.dev_sts = SCI_IIC_NO_INIT;
  iic_info.p_data1st = &reg_add;
  iic_info.p_data2nd = &rx;

  ret = R_SCI_IIC_MasterReceive(&iic_info);
  if(SCI_IIC_SUCCESS == ret)
  {
    while(SCI_IIC_FINISH != iic_info.dev_sts);
  }

  return rx;
}

void I2C_Send(uint8_t reg_add, uint8_t value)
{
  iic_info.cnt1st = 1;
  iic_info.cnt2nd = 1;
  iic_info.dev_sts = SCI_IIC_NO_INIT;
  iic_info.p_data1st = &reg_add;
  iic_info.p_data2nd = &value;

  ret = R_SCI_IIC_MasterSend(&iic_info);
  if(SCI_IIC_SUCCESS == ret)
  {
    while(SCI_IIC_FINISH != iic_info.dev_sts);
  }

}

void callBack_read()
{
  volatile sci_iic_return_t ret;
  sci_iic_mcu_status_t iic_status;
  sci_iic_info_t iic_info_ch;
  iic_info_ch.ch_no = 11;
  ret = R_SCI_IIC_GetStatus(&iic_info_ch, &iic_status);
  if(SCI_IIC_SUCCESS != ret)
  {
    /* Call error processing for the R_SCI_IIC_GetStatus()function*/
  }
  else
  {
    if(1 == iic_status.BIT.NACK)
    {
      /* Processing when a NACK is detected
       by verifying the iic_status flag. */

    }
  }
}

void callBack_write()
{
  volatile sci_iic_return_t ret;
  sci_iic_mcu_status_t iic_status;
  sci_iic_info_t iic_info_ch;
  iic_info_ch.ch_no = 11;
  ret = R_SCI_IIC_GetStatus(&iic_info_ch, &iic_status);
  if(SCI_IIC_SUCCESS != ret)
  {
    /* Call error processing for the R_SCI_IIC_GetStatus()function*/
  }
  else
  {
    if(1 == iic_status.BIT.NACK)
    {
      /* Processing when a NACK is detected
       by verifying the iic_status flag. */

    }
  }
}

/* End of function my_rspi_callback. */
