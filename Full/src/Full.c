/***********************************************************************
 *
 *  FILE        : Full.c
 *  DATE        : 2021-07-08
 *  DESCRIPTION : Main Program
 *
 *  NOTE:THIS IS A TYPICAL EXAMPLE.
 *
 ***********************************************************************/

//5F1 board

#include "globals.h"

void main(void);

int g_counter;
int g_readBuffer;
uint32_t g_file_size;
uint32_t g_current_byte;
int g_playing;
int g_stopPlaying;

int16_t (*mode)();

char line[100];
int startOfFileNames;
int startOfPlaylist;

static FATFS fs; /* File system object structure */
const static uint8_t g_msc_file[14] = "0:sample02.wpj";
FIL file;
FRESULT fr;
uint8_t g_file_data[FILE_SIZE];
uint8_t spi_test[FILE_SIZE];

file_meta_data_t g_file_meta_data[255];
playlist_t g_output_music[255];
flash_custom_FAT_t flash_table[255];

uint8_t spi_tx_buff[4];
uint8_t spi_rx_buff[4];

wav_header_t g_wav_file;

volatile sci_iic_return_t ret;
sci_iic_info_t iic_info;
uint8_t i2c_rx[5];
uint8_t i2c_tx[5];
uint8_t i2c_address[1] = {I2C_ADDR};
uint8_t i2c_reg_addr[1];

void main(void)
{
  char is_data_in_flash = 0;
  usb_ctrl_t ctrl;
  usb_cfg_t cfg;
  usb_err_t usb_err;
  uint16_t event;
  st_memdrv_info_t memdrv_info;
  nand_flash_status_t nand_status;

  //SPI CS Pin
  PORTA.PDR.BIT.B4 = 1;  //Set pin as output
  PORTA.PODR.BIT.B4 = 1;  //Set pin HIGH

  // CPU init (cloks, RAM, etc.) and peripheral init is done in
  // resetpgr.c in PowerON_Reset_PC function.

  DIP_Init();
  LED_Init();
  FIFO_Init();
  R_DAC1_Start();
  R_DAC1_Set_ConversionValue(0x800);  //This is to avoid popping sound at the start
  R_TMR23_Start();

  //Init USB Host Mass Storage Device controller
  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS;
  cfg.usb_mode = USB_HOST;
  usb_err = R_USB_Open(&ctrl, &cfg);

  //Init SPI for NAND Flash
  memdrv_info.cnt = 0;
  memdrv_info.p_data = NULL;
  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  R_MEMDRV_Open(0, &memdrv_info);
  NAND_CheckBlock();

  /* With polling, check for 1s if USB is connected or not */
  while(g_counter < 1000)
  {
    event = R_USB_GetEvent(&ctrl);  // Get event code

    switch(event)
    {
      case USB_STS_CONFIGURED:

        R_TMR23_Stop();
        g_counter = 1000;
        //nand_status = NAND_Erase();
        //NAND_CopyToFlash();
        is_data_in_flash = 1;
        break;

      case USB_STS_DETACH:
        break;

      case USB_STS_NOT_SUPPORT:
        break;

      case USB_STS_NONE:
        break;

      default:
        break;
    }
  }
  R_TMR23_Stop();
  g_counter = 0;

  /* Check if there is any data in flash. */
  //is_data_in_flash = check_if_data_in_flash()
  is_data_in_flash = 1;

  /* If data in flash, prepare lookout tables. */
  if(is_data_in_flash)
  {
    NAND_Reset();
    NAND_ReadFromFlash(0, sizeof(flash_table), (uint8_t*) &flash_table[0]);
    FIFO_Init();
    NAND_ReadFromFlash(NAND_PAGE_SIZE * 3, sizeof(g_output_music), (uint8_t*) &g_output_music[0]);
    FIFO_Init();
  }
  else
  {
    while(1);  // No data in flash, loop forever here.
  }

  /* Mode select */
  uint8_t mode_select = DIP_ReadState();

  switch(mode_select)
  {
    case 0:
      mode = normalPlay;
      break;

    case 1:
      mode = lastInputPlay;
      break;

    case 2:
      mode = priorityPlay;
      break;

    case 3:
      mode = inputPlay;
      break;

    case 4:
      break;

    case 5:
      mode = binary128ch;
      break;

    case 6:
      mode = binary255_positive;
      break;

    case 7:
      mode = binary255_negative;
      break;
  }
  //mode = normalPlay;
  //mode = lastInputPlay;
  //mode = priorityPlay;
  mode = inputPlay;

  I2C_Init();
  R_EXT_IRQ_IRQ13_Start();

  /* Wait for interrupt from GPIO pins to start playing */
  while(1)
  {
    if(g_isIRQ)
    {
      int16_t song = mode();
      if(-1 != song)
      {
        g_stopPlaying = 0;
        playFromPlaylist(song);
      }
    }
  }

}

int g_playing = 0;
int fiffo_test = 1000;

void playFromPlaylist(uint8_t playNr)
{
  int index = 0;
  int fileToPlay;
  UINT size;
  uint32_t fileAddress;
  int trackNr = 0;
  int repetitions = 0;
  int fifo_status = FIFO_OK;

  LED_BusyOn();
  while(g_output_music[playNr].repeat > repetitions)
  {
    while(g_output_music[playNr].playlist_len > trackNr)
    {
      DA.DADR1 = 2048;
      fileToPlay = g_output_music[playNr].file_nr[index];
      fileAddress = flash_table[fileToPlay].address;

      NAND_ReadFromFlash(fileAddress, WAV_HEADER_SIZE, g_file_data);
      //FIFO_Init();

      WAV_Open(&g_wav_file, g_file_data);
      fileAddress += WAV_HEADER_SIZE;
      NAND_ReadFromFlash(fileAddress, sizeof(g_file_data), g_file_data);
      g_counter = 0;
      g_current_byte = 0;
      //g_stopPlaying = 0;

      g_playing = 1;
      R_TMR01_Set_Frequency(g_wav_file.sample_rate);

      R_TMR01_Start();
      while(g_playing)
      {
        //if(FIFO_head - fiffo_test == FIFO_tail)
        if(g_readBuffer)
        {
          g_readBuffer = 0;
          fileAddress += sizeof(g_file_data);
          NAND_ReadFromFlash(fileAddress, sizeof(g_file_data), g_file_data);
        }

        mode();
      }

      if(g_stopPlaying)
      {
        LED_BusyOff();
        return;
      }

      FIFO_Init();
      DA.DADR1 = 2048;

      trackNr++;
      index++;
    }

    trackNr = 0;
    index = 0;
    repetitions++;
  }

  LED_BusyOff();
}

uint8_t FIFO_buffer[FIFO_SIZE];
uint16_t FIFO_head, FIFO_tail;

void FIFO_Init(void)
{
  FIFO_head = 0;
  FIFO_tail = 0;
}

int FIFO_Write(uint8_t *new, uint16_t size)
{
  for(int i = 0; i < size; i++)
  {
    if(FIFO_head == ((FIFO_tail - 1 + FIFO_SIZE) % FIFO_SIZE))
    {
      return FIFO_FULL; /* Queue Full*/
    }

    FIFO_buffer[FIFO_head] = *(new + i);

    FIFO_head = (FIFO_head + 1) % FIFO_SIZE;
  }

  return FIFO_OK;  // No errors
}

int FIFO_Read(uint8_t *old, uint16_t size)
{
  for(int i = 0; i < size; i++)
  {
    if(FIFO_head == FIFO_tail)
    {
      return FIFO_EMPTY; /* Queue Empty - nothing to get*/
    }

    *(old + i) = FIFO_buffer[FIFO_tail];

    FIFO_tail = (FIFO_tail + 1) % FIFO_SIZE;
  }

  return FIFO_OK;  // No errors
}

void I2C_Init()
{

  /* GPIO Mux Reset pin */
  PORT5.PDR.BIT.B0 = 1;  //Set pin as output
  PORT5.PODR.BIT.B0 = 0;  //Set pin LOW
  R_BSP_SoftwareDelay(5, BSP_DELAY_MILLISECS);
  PORT5.PODR.BIT.B0 = 1;  //Set pin HIGH

  i2c_reg_addr[0] = 0x0A;
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
  I2C_Send(0x0A, 0xC2);  //INT is active HIGH

  //IODIRA
  I2C_Send(0x00, 0xFF);
  //IOPOLA
  I2C_Send(0x01, 0);
  //GPINTENA
  I2C_Send(0x02, 0xFF);
  //DEFVALA
  I2C_Send(0x03, 0xFF);
  //INTCONA
  I2C_Send(0x04, 0xFF);
  //GPPUA
  I2C_Send(0x06, 0xFF);

  //IODIRB
  I2C_Send(0x10, 0xFF);
  //IOPOLB
  I2C_Send(0x11, 0);
  //GPINTENB
  I2C_Send(0x12, 0xFF);
  //DEFVALB
  I2C_Send(0x13, 0xFF);
  //INTCONB
  I2C_Send(0x14, 0xFF);
  //GPPUB
  I2C_Send(0x16, 0xFF);

  //Clear interrupts
  I2C_Receive(0x09);
  I2C_Receive(0x19);
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
