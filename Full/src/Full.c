/***********************************************************************
 *
 *  FILE        : Full.c
 *  DATE        : 2021-07-08
 *  DESCRIPTION : Main Program
 *
 *  NOTE:THIS IS A TYPICAL EXAMPLE.
 *
 ***********************************************************************/

//5A2 board
#include "globals.h"

void main(void);
void CNT_USB_CntCallback();

int g_counter;
int g_readBuffer;
uint32_t g_file_size;
uint32_t g_current_byte;
int g_playing;
int g_stopPlaying;
__attribute__((aligned))
       uint16_t g_volume[2];

int16_t (*mode)();
modeSelect_t boardType;

char line[100];
int startOfFileNames;
int startOfPlaylist;

uint8_t g_file_data[FILE_SIZE];
file_meta_data_t g_file_meta_data[255];
playlist_t g_output_music[255];
flash_custom_FAT_t flash_table[255];

uint8_t spi_tx_buff[4];
uint8_t spi_rx_buff[4];

wav_header_t g_wav_file;

volatile sci_iic_return_t ret;
sci_iic_info_t iic_info;
uint8_t g_i2c_gpio_rx[2];
uint8_t g_i2c_gpio_tx[2];
uint8_t i2c_gpio_address[1] = {I2C_GPIO_ADDR};
uint8_t i2c_potent_address[1] = {I2C_POTENT_ADDR};
uint8_t i2c_reg_addr[1];

/* Used inside main. Declared here so we don't fill up stack. */
char is_data_in_flash = 0;
usb_ctrl_t ctrl;
usb_cfg_t cfg;
usb_err_t usb_err;
uint16_t event;
st_memdrv_info_t memdrv_info;
nand_flash_status_t nand_status;
uint32_t cmt_channel;
int usb_cnt;

/* main start */
void main(void)
{

  //SPI CS Pin
  PORTD.PDR.BIT.B4 = 1;  //Set pin as output
  //PORTD.PODR.BIT.B4 = 1;  //Set pin HIGH
  NAND_CS_HIGH;

  uint8_t test = PORTD.PDR.BIT.B4;

  // CPU init (cloks, RAM, etc.) and peripheral init is done in
  // resetpgr.c in PowerON_Reset_PC function.

  LED_PowOn();
  FIFO_Init();
  R_DAC1_Start();
  R_DAC1_Set_ConversionValue(0x800);  //This is to avoid popping sound at the start
  boardType = PIN_BoardSelection();

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
  NAND_Reset();
  NAND_CheckBlock();

  g_counter = 0;
  R_CMT_CreatePeriodic(2, &CNT_USB_CntCallback, &cmt_channel);
  usb_cnt = 0;

  /* With polling, check for 1s if USB is connected or not */
  while(usb_cnt < 2)
  {
    event = R_USB_GetEvent(&ctrl);  // Get event code

    switch(event)
    {
      case USB_STS_CONFIGURED:

        R_CMT_Stop(cmt_channel);
        usb_cnt = 2;
        LED_USBOn();
        NAND_Reset();
        nand_status = NAND_Erase();
        if(NAND_ERASE_NOK == nand_status)
        {
          ERROR_FlashECS();
        }
        NAND_Reset();
        NAND_CopyToFlash();
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
  R_CMT_Stop(cmt_channel);
  LED_USBOff();
  g_counter = 0;

  /* Check if there is any data in flash. */
  is_data_in_flash = NAND_CheckDataInFlash();

  /* If data in flash, prepare lookout tables. */
  if(is_data_in_flash)
  {
    NAND_Reset();
    //first 4 bytes signalise if data is in flash
    NAND_ReadFromFlash(NAND_PAGE_SIZE, sizeof(flash_table), (uint8_t*) &flash_table[0]);
    //flash_table[1].address = 0x177fe;  //bug fix for test.
    NAND_ReadFromFlash(NAND_PAGE_SIZE * 3, sizeof(g_output_music), (uint8_t*) &g_output_music[0]);
  }
  else
  {
    LED_AlarmOn();
    while(1);  // No data in flash, loop forever here.
  }

  /* Mode select */
  uint8_t _mode_select = DIP_ReadState();
  switch(_mode_select)
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
      mode = binary128ch;
      break;

    case 5:
      break;

    case 6:
      mode = binary255_negative;
      break;

    case 7:
      mode = binary255_positive;
      break;
  }

  I2C_Init();
  R_EXT_IRQ_IRQ13_Start();

  /* Enable audio amp */
  PIN_ShutdownSet();

  /* Periodic timer ~45 ms. Used for I2C communication */
  R_CMT_CreatePeriodic(22, &I2C_Periodic, &cmt_channel);
  R_DMAC0_Start();
  R_ADC0_Start();
  R_DMAC1_Start();
  R_TMR_play_Set_Frequency(1000);

  for(int i = 0; i < sizeof(g_file_data); i++)
  {
    FIFO_buffer[i] = i;
    ringbuf[i] = i;
  }

  //R_TPU0_Start();

  /* Wait for interrupt from GPIO pins to start playing */
  while(1)
  {
    if(g_isIRQ)
    {
      int16_t _song = mode();
      if(-1 != _song)
      {
        g_stopPlaying = 0;
        playFromPlaylist(_song);
      }
    }
  }

}

int g_playing = 0;
int fiffo_test = 1000;

void playFromPlaylist(uint8_t playNr)
{
  int _index = 0;
  int _fileToPlay;
  uint32_t _dataSize;
  uint32_t _sizeToRead;
  uint32_t _fileAddress;
  int _trackNr = 0;
  int _repetitions = 0;
  int _fifo_status = FIFO_OK;

  LED_BusyOn();
  while(g_output_music[playNr].repeat > _repetitions)
  {
    while(g_output_music[playNr].playlist_len > _trackNr)
    {
      //DA.DADR1 = 2048;
      _fileToPlay = g_output_music[playNr].file_nr[_index];
      _fileAddress = flash_table[_fileToPlay].address;

      NAND_ReadFromFlash(_fileAddress, WAV_HEADER_SIZE, g_file_data);
      //FIFO_Init();

      WAV_Open(&g_wav_file, g_file_data);
      _dataSize = g_wav_file.data_size;
      _fileAddress += WAV_HEADER_SIZE;
      g_counter = 0;
      g_current_byte = 0;
      //g_stopPlaying = 0;

      g_playing = 1;
      R_TMR_play_Set_Frequency(g_wav_file.sample_rate);

      //R_TMR_play_Start();
      R_TPU0_Start();
      //DA.DADR1 = 4095;
      //LED_USBOn();
      //while(g_playing)
      while(_dataSize > 0)
      {
        g_readBuffer = 0;
        if(_dataSize >= sizeof(g_file_data))
        {
          _sizeToRead = sizeof(g_file_data);
        }
        else
        {
          _sizeToRead = _dataSize;
        }

        NAND_ReadFromFlash(_fileAddress, _sizeToRead, g_file_data);

        LED_USBOn();
        wavmp3p_put(g_file_data, _sizeToRead);
        _fileAddress += sizeof(g_file_data);
        _dataSize -= _sizeToRead;
        mode();
      }
      //LED_USBOff();
      //DA.DADR1 = 0;

      if(g_stopPlaying)
      {
        LED_BusyOff();
        return;
      }

      R_TPU0_Stop();
      FIFO_Init();
      //DA.DADR1 = 2048;

      _trackNr++;
      _index++;
    }

    _trackNr = 0;
    _index = 0;
    _repetitions++;
  }

  LED_BusyOff();
}

__attribute__((aligned(RINGBUF_SIZE)))
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

  /* GPIO Mux Init */
  /* GPIO Mux Reset pin */
  PORT5.PDR.BIT.B0 = 1;  //Set pin as output
  PORT5.PODR.BIT.B0 = 0;  //Set pin LOW
  R_BSP_SoftwareDelay(5, BSP_DELAY_MILLISECS);
  PORT5.PODR.BIT.B0 = 1;  //Set pin HIGH

  i2c_reg_addr[0] = 0x0A;
  g_i2c_gpio_rx[0] = 255;

  iic_info.callbackfunc = &callBack_read;
  iic_info.ch_no = 11;
  iic_info.cnt1st = 1;
  iic_info.cnt2nd = 2;
  iic_info.dev_sts = SCI_IIC_NO_INIT;
  iic_info.p_data1st = i2c_reg_addr;
  iic_info.p_data2nd = g_i2c_gpio_rx;
  iic_info.p_slv_adr = i2c_potent_address;

  ret = R_SCI_IIC_Open(&iic_info);
  /* Potentiometer init */
  I2C_Send(2, 0x80);

  // Switch from Potentiometer address to GPIO address
  iic_info.p_slv_adr = i2c_gpio_address;

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
  uint8_t _rx;

  iic_info.cnt1st = 1;
  iic_info.cnt2nd = 1;
  iic_info.dev_sts = SCI_IIC_NO_INIT;
  iic_info.p_data1st = &reg_add;
  iic_info.p_data2nd = &_rx;

  ret = R_SCI_IIC_MasterReceive(&iic_info);
  if(SCI_IIC_SUCCESS == ret)
  {
    while(SCI_IIC_FINISH != iic_info.dev_sts);
  }

  return _rx;
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

void CNT_USB_CntCallback()
{
  usb_cnt++;
}

void I2C_Periodic()
{
  R_BSP_InterruptsEnable();
  uint8_t volume;

  volume = (uint8_t) (g_volume[0] >> 1);  //g_volume[0] is 16-bit variable, but contains 8-bit value. Potentiometer can accept only values from 0 to 127.

  // Activated when 0.
  if(0 == PIN_6dBGet())
  {
    volume = volume / 2;
  }
  else if(0 == PIN_14dBGet())
  {
    volume = volume / 5;
  }

  /* Send volume data to potentiometer */
  iic_info.p_slv_adr = i2c_potent_address;
  I2C_Send(0, volume);

  /* Read gpioa, gpiob from GPIO mux */
  iic_info.p_slv_adr = i2c_gpio_address;
  g_i2c_gpio_rx[0] = I2C_Receive(0x09);
  g_i2c_gpio_rx[1] = I2C_Receive(0x19);

  if(g_isIRQTriggered)
  {
    g_isIRQ = 1;
  }
  else
  {
    g_isIRQ = 0;
  }
}
