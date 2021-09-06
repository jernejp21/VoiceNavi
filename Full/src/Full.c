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

/** Function definition */
void main(void);
void playFromPlaylist(uint8_t);
void CNT_USB_CntCallback();
void wavmp3p_put(void*, uint32_t);
void emptyPlayBuffer();
void callBack_read();
void I2C_Send(uint8_t reg_add, uint8_t value);
uint8_t I2C_Receive(uint8_t reg_add);
void I2C_Init();
void I2C_Periodic();

/* Global variables */
int g_counter;
int g_readBuffer;
uint32_t g_file_size;
int g_playing;
int g_stopPlaying;
uint8_t g_i2c_gpio_rx[2];
uint8_t g_i2c_gpio_tx[2];
const uint8_t g_msc_file[15];
uint8_t g_isIRQ;
uint8_t g_isIRQTriggered;

uint16_t g_volume[2] __attribute__((aligned(4)));

uint8_t (*mode)();
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

uint8_t i2c_gpio_address[1] = {I2C_GPIO_ADDR};
uint8_t i2c_potent_address[1] = {I2C_POTENT_ADDR};
uint8_t i2c_reg_addr[1];

/* Used inside this file. Declared here so we don't fill up stack. */
char is_data_in_flash = 0;
usb_ctrl_t ctrl;
usb_cfg_t cfg;
usb_err_t usb_err;
uint16_t event;
st_memdrv_info_t memdrv_info;
nand_flash_status_t nand_status;
uint32_t cmt_channel;
int usb_cnt;
uint8_t interval_time;
uint8_t song[20];
uint8_t g_song_cnt;

uint16_t ringbuf[RINGBUF_SIZE] __attribute__((aligned(4096)));
static int decode_putp = 0;

/* main start */
void main(void)
{

  //SPI CS Pin
  PORTD.PDR.BIT.B4 = 1;  //Set pin as output
  //PORTD.PODR.BIT.B4 = 1;  //Set pin HIGH
  NAND_CS_HIGH;

  // CPU init (cloks, RAM, etc.) and peripheral init is done in
  // resetpgr.c in PowerON_Reset_PC function.

  LED_PowOn();
  R_DAC1_Start();
  R_DAC1_Set_ConversionValue(0x800);  //This is to avoid popping sound at the start
  boardType = PIN_BoardSelection();
  I2C_Init();
  /* Periodic timer ~45 ms. Used for I2C communication */
  R_CMT_CreatePeriodic(22, &I2C_Periodic, &cmt_channel);

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

  //Create 500 ms counter for polling to check if USB is connected.
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

  /* Check if there is any data in flash. */
  is_data_in_flash = NAND_CheckDataInFlash();

  /* If data in flash, prepare lookout tables. */
  if(is_data_in_flash)
  {
    NAND_Reset();
    NAND_ReadFromFlash(NAND_FILE_LIST_PAGE, sizeof(flash_table), (uint8_t*)&flash_table[0]);
    NAND_ReadFromFlash(NAND_PLAYLIST_PAGE, sizeof(g_output_music), (uint8_t*)&g_output_music[0]);
  }
  else
  {
    LED_AlarmOn();
    while(1);  // No data in flash, loop forever here.
  }

  /* Mode select */
  uint8_t _mode_select = DIP_ReadState() & 0x07;  //Switches 1, 2, 3 are mode select
  switch(_mode_select)
  {
    case 0:
      mode = normalPlay;
      break;

    case 1:
      mode = lastInputInterruptPlay;
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

  /* Inverval select */
  uint8_t _interval_select = (DIP_ReadState() & 0x18) >> 3;  //Switches 4, 5 are interval select
  if(_mode_select == 0)
  {
    switch(_interval_select)
    {
      /* Interval are 30, 60 and 120 s. */
      case 0:
        interval_time = 0;
        break;

      case 1:
        interval_time = 30;
        break;

      case 2:
        interval_time = 60;
        break;

      case 3:
        interval_time = 120;
        break;
    }
  }
  else
  {
    interval_time = 0;
  }

  /* Enable audio amp */
  PIN_ShutdownSet();

  R_DMAC0_Start();
  R_ADC0_Start();
  R_DMAC1_Start();
  emptyPlayBuffer();

  /* Wait for interrupt from GPIO pins to start playing */
  memset(song, -1, 20);
  int cur_cnt = 0;

  while(1)
  {
    while(cur_cnt < g_song_cnt)
    {
      if(0xFF != song[cur_cnt])
      {
        playFromPlaylist(song[cur_cnt]);
        cur_cnt++;
      }
      else
      {
        break;
      }

      /* Wait for interval time */
      R_BSP_SoftwareDelay(interval_time, BSP_DELAY_SECS);
    }
    g_song_cnt = mode(song);
    cur_cnt = 0;
  }

}

int g_playing = 0;

void playFromPlaylist(uint8_t playNr)
{
  int _index = 0;
  int _fileToPlay;
  uint32_t _dataSize;
  uint32_t _sizeToRead;
  uint32_t _fileAddress;
  int _trackNr = 0;
  int _repetitions = 0;

  LED_BusyOn();
  while(g_output_music[playNr].repeat > _repetitions)
  {
    while(g_output_music[playNr].playlist_len > _trackNr)
    {
      _fileToPlay = g_output_music[playNr].file_nr[_index];
      _fileAddress = flash_table[_fileToPlay].address;

      NAND_ReadFromFlash(_fileAddress, WAV_HEADER_SIZE, g_file_data);

      WAV_Open(&g_wav_file, g_file_data);
      _dataSize = g_wav_file.data_size;
      _fileAddress += WAV_HEADER_SIZE;

      g_playing = 1;
      R_TPU0_SetFrequency(g_wav_file.sample_rate);

      R_TPU0_Start();
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

        wavmp3p_put(g_file_data, _sizeToRead);
        _fileAddress += sizeof(g_file_data);
        _dataSize -= _sizeToRead;

        mode(song);
        if(!g_playing)
        {
          break;
        }
        song[g_song_cnt] = 0xFF;
      }

      R_TPU0_Stop();
      emptyPlayBuffer();
      g_playing = 0;

      _trackNr++;
      _index++;
    }

    _trackNr = 0;
    _index = 0;
    _repetitions++;
  }

  LED_BusyOff();
}

int decode_getp(void)
{
  return ((unsigned int) DMAC1.DMSAR - (unsigned int)&ringbuf[0]) / sizeof(uint16_t);
}

int decode_put(uint16_t audio_data)
{
  int ret = 0;
  unsigned int putp = p_inc(decode_putp, RINGBUF_SIZE);

  if(putp != decode_getp())
  {

    ringbuf[decode_putp] = audio_data;

    decode_putp = putp;
    ret = 1;
  }

  return ret;
}

void wavmp3p_put(void *read_buffer, uint32_t size)
{
  uint8_t *data = read_buffer;
  int16_t audio_data;

  if(g_wav_file.bps == 8)
  {
    for(int n = 0; n < size; n++)
    {
      audio_data = (int16_t)(*data++);
      audio_data = audio_data << 4;

      while(1 != decode_put((uint16_t)audio_data));
    }
  }
  else
  {
    for(int n = 0; n < size; n += 2)
    {
      audio_data = (int16_t)((*(data + 1) << 8) | *data);
      audio_data = (audio_data >> 4) + 2048;

      data += 2;

      while(1 != decode_put((uint16_t)audio_data));
    }
  }

}

void emptyPlayBuffer()
{
  for(int i = 0; i < RINGBUF_SIZE; i++)
  {
    ringbuf[i] = 2048;
  }
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

  /* Potentiometer init - do not write to EEPROM */
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
}

void CNT_USB_CntCallback()
{
  usb_cnt++;
}

void I2C_Periodic()
{
  R_BSP_InterruptsEnable();
  uint8_t _volume;

  //g_volume[0] is 16-bit variable, but contains 8-bit value. Potentiometer can accept only values from 0 to 127.
  _volume = (uint8_t)(g_volume[0] >> 1);

  // Activated when 0.
  if(0 == PIN_Get6dB())
  {
    // -6dB is half
    _volume = _volume / 2;
  }
  else if(0 == PIN_Get14dB())
  {
    // -14dB is fifth
    _volume = _volume / 5;
  }

  /* Send volume data to potentiometer */
  iic_info.p_slv_adr = i2c_potent_address;
  I2C_Send(0, _volume);

  /* Read gpioa, gpiob from GPIO mux */
  iic_info.p_slv_adr = i2c_gpio_address;
  g_i2c_gpio_rx[0] = I2C_Receive(0x09);
  g_i2c_gpio_rx[1] = I2C_Receive(0x19);

  g_isIRQ = PIN_GetExtIRQ();
}
