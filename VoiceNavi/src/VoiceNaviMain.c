/**
 * Voice Navi Firmware
 *
 * MIT License
 *
 * Copyright (c) 2022 Azurtest
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

/* Firmware version: 3.7 */

#include "globals.h"

/** Function definition */
void main(void);
void CNT_USB_CntCallback();
void CNT_IntervalDelay();
void callBack_read();
void ISR_periodicPolling();

/** Defines */
#define I2C_GPIO_ADDR 0x21
#define I2C_POTENT_ADDR 0x28

#define RINGBUF_SIZE 4096
#define p_inc(p, max) (((p + 1) >= max) ? 0 : (p + 1))

/** Global variables */
uint32_t g_binary_vol_reduction_address = NAND_VOL_PAGE;
system_status_t g_systemStatus;
uint8_t g_binary_vol_reduction;
int g_decode_putp = 0;

/** Song, play mode and output related variables */
uint8_t file_data[FILE_SIZE];
playlist_t output_music[255];
flash_custom_FAT_t flash_table[255];
wav_header_t wav_file;
uint16_t volume[2] __attribute__((aligned(4)));
uint16_t ringbuf[RINGBUF_SIZE] __attribute__((aligned(8192)));
uint8_t mode_select;
modeSelect_t boardType;

/** Communication structures */
sci_iic_info_t iic_info;
usb_ctrl_t ctrl;
usb_cfg_t cfg;
st_memdrv_info_t memdrv_info;

/** CMT counter related variables */
uint32_t cmt_channel;
uint32_t cmt_channel_usb;
uint32_t cmt_channel_i2c;
uint32_t cmt_channel_interval_delay;
int usb_cnt;
uint8_t interval_time;

static int decode_getp(void)
{
  return ((unsigned int) DMAC1.DMSAR - (unsigned int)&ringbuf[0]) / sizeof(uint16_t);
}

static int decode_put(uint16_t audio_data)
{
  int ret = 0;

  unsigned int putp = p_inc(g_decode_putp, RINGBUF_SIZE);

  if(putp != decode_getp())
  {

    ringbuf[g_decode_putp] = audio_data;

    g_decode_putp = putp;
    ret = 1;
  }

  return ret;
}

static void wav_put(void *read_buffer, uint32_t size)
{
  uint8_t *data = read_buffer;
  int16_t audio_data;

  if(wav_file.bps == 8)
  {
    for(int n = 0; n < size; n++)
    {
      audio_data = (int16_t)(*data++);
      audio_data = audio_data << 4;

      while(1 != decode_put((uint16_t)audio_data))
      {
        if(!g_systemStatus.flag_isPlaying)
        {
          return;
        }
      }
    }
  }
  else
  {
    for(int n = 0; n < size; n += 2)
    {
      audio_data = (int16_t)((*(data + 1) << 8) | *data);
      audio_data = (audio_data >> 4) + 2048;

      data += 2;

      while(1 != decode_put((uint16_t)audio_data))
      {
        if(!g_systemStatus.flag_isPlaying)
        {
          return;
        }
      }
    }
  }

}

static void emptyPlayBuffer()
{
  /* Fill buffer with 2048 (half of 12-bit ADC) for smooth transitions between songs. */
  for(int i = 0; i < RINGBUF_SIZE; i++)
  {
    ringbuf[i] = 2048;
  }
  R_DAC1_Set_ConversionValue(2048);
}

static void PIN_BusyOn()
{
  if((boardType == WAV_5A2) && (mode_select == 7))
  {
    PIN_BusySet();
  }
  else
  {
    PIN_BusyReset();
  }
}

static void PIN_BusyOff()
{
  if((boardType == WAV_5A2) && (mode_select == 7))
  {
    PIN_BusyReset();
  }
  else
  {
    PIN_BusySet();
  }
}

static void playFromPlaylist(uint8_t playNr, uint8_t isInfineteLoop)
{
  int _index = 0;
  int _fileToPlay;
  uint32_t _dataSize;
  uint32_t _sizeToRead;
  uint32_t _fileAddress;
  int _trackNr = 0;
  int _repetitions = 0;
  wav_err_t wav_err;

  if(output_music[playNr].playlist_len)
  {
    g_systemStatus.flag_isSongAvailable = 1;
  }
  else
  {
    g_systemStatus.flag_isSongAvailable = 0;
    return;
  }

  if(!isInfineteLoop)
  {
    g_systemStatus.vol_ctrl_nr = output_music[playNr].vol_ctrl - 1;
  }

  LED_BusyOn();
  PIN_BusyOn();
  /* Enable audio amp */
  PIN_ShutdownSet();
  while(output_music[playNr].repeat > _repetitions)
  {
    while(output_music[playNr].playlist_len > _trackNr)
    {
      _fileToPlay = output_music[playNr].file_nr[_index];
      _fileAddress = flash_table[_fileToPlay].address;
      _dataSize = flash_table[_fileToPlay].file_size;

      NAND_ReadFromFlash(&_fileAddress, WAV_HEADER_SIZE, file_data);

      wav_err = WAV_Open(&wav_file, file_data, &_fileAddress, _dataSize);

      if((wav_file.channel != 1) || (wav_err != WAV_NO_ERR))
      {
        //Bad wave file or,
        // Mono has 1 channel, stereo has 2 channels.
        // Theoretically there can be more than 2 channels, but we only play mono.
        ERROR_WAVEFile();
        LED_BusyOff();
        PIN_BusyOff();
        /* Disable audio amp */
        PIN_ShutdownReset();
        return;
      }

      _dataSize = wav_file.data_cksize;
      _fileAddress = wav_file.data_address;

      g_systemStatus.flag_isPlaying = 1;
      R_TPU0_SetFrequency(wav_file.sample_rate);

      if(_dataSize < sizeof(file_data))
      {
        _sizeToRead = _dataSize;
      }
      else
      {
        _sizeToRead = sizeof(file_data);
      }

      // Fill the buffer before enabling DMA to avoid blank interval.
      NAND_ReadFromFlash(&_fileAddress, _sizeToRead, file_data);
      g_decode_putp = 0;
      wav_put(file_data, _sizeToRead);
      _dataSize -= _sizeToRead;

      ERROR_ClearErrors();
      R_TPU0_Start();
      while(_dataSize > 0)
      {
        if(_dataSize >= sizeof(file_data))
        {
          _sizeToRead = sizeof(file_data);
        }
        else
        {
          _sizeToRead = _dataSize;
        }

        NAND_ReadFromFlash(&_fileAddress, _sizeToRead, file_data);

        wav_put(file_data, _sizeToRead);
        _dataSize -= _sizeToRead;

        if(!g_systemStatus.flag_isPlaying)
        {
          R_TPU0_Stop();
          R_DMAC1_SetAddresses((void*)&ringbuf, (void*)&DA.DADR1);  // Reset DMA address
          emptyPlayBuffer();
          LED_BusyOff();
          PIN_BusyOff();
          /* Disable audio amp */
          PIN_ShutdownReset();
          return;
        }
      }

      /* Wait until all data is sent to output (speakers) */
      uint32_t end_addr = (uint32_t)(ringbuf + g_decode_putp);
      do
      {
        R_WDT_Restart();
      }
      while((uint32_t)DMAC1.DMSAR != end_addr);

      R_TPU0_Stop();
      R_DMAC1_SetAddresses((void*)&ringbuf, (void*)&DA.DADR1);  // Reset DMA address
      emptyPlayBuffer();
      g_systemStatus.flag_isPlaying = 0;

      _trackNr++;
      _index++;
    }

    _trackNr = 0;
    _index = 0;
    _repetitions++;
  }

  LED_BusyOff();
  PIN_BusyOff();
  /* Disable audio amp */
  PIN_ShutdownReset();
}

static uint8_t I2C_Receive(sci_iic_info_t *i2c_info, uint8_t dev_add, uint8_t reg_add)
{
  volatile sci_iic_return_t ret;
  uint8_t _rx;

  i2c_info->cnt1st = 1;
  i2c_info->cnt2nd = 1;
  i2c_info->dev_sts = SCI_IIC_NO_INIT;
  i2c_info->p_data1st = &reg_add;
  i2c_info->p_data2nd = &_rx;
  i2c_info->p_slv_adr = &dev_add;

  ret = R_SCI_IIC_MasterReceive(i2c_info);
  if(SCI_IIC_SUCCESS == ret)
  {
    while(SCI_IIC_FINISH != i2c_info->dev_sts);
  }

  return _rx;
}

static void I2C_Send(sci_iic_info_t *i2c_info, uint8_t dev_add, uint8_t reg_add, uint8_t value)
{
  volatile sci_iic_return_t ret;

  i2c_info->cnt1st = 1;
  i2c_info->cnt2nd = 1;
  i2c_info->dev_sts = SCI_IIC_NO_INIT;
  i2c_info->p_data1st = &reg_add;
  i2c_info->p_data2nd = &value;
  i2c_info->p_slv_adr = &dev_add;

  ret = R_SCI_IIC_MasterSend(i2c_info);
  if(SCI_IIC_SUCCESS == ret)
  {
    while(SCI_IIC_FINISH != i2c_info->dev_sts);
  }

}
static void I2C_Init()
{
  /* GPIO Mux Reset pin */
  PIN_RstReset();
  R_BSP_SoftwareDelay(5, BSP_DELAY_MILLISECS);
  PIN_RstSet();

  iic_info.callbackfunc = &callBack_read;
  iic_info.ch_no = 11;
  iic_info.cnt1st = 1;
  iic_info.cnt2nd = 1;
  iic_info.dev_sts = SCI_IIC_NO_INIT;

  R_SCI_IIC_Open(&iic_info);

  /* Potentiometer init - do not write to EEPROM */
  I2C_Send(&iic_info, I2C_POTENT_ADDR, 2, 0x80);

  /* GPIO Mux Init */
  // IOCON
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x0A, 0xC2);  // INT is active HIGH

  // IODIRA
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x00, 0xFF);
  // IOPOLA
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x01, 0);
  // GPINTENA
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x02, 0xFF);
  // DEFVALA
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x03, 0xFF);
  // INTCONA
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x04, 0xFF);
  // GPPUA
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x06, 0xFF);

  // IODIRB
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x10, 0xFF);
  // IOPOLB
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x11, 0);
  // GPINTENB
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x12, 0xFF);
  // DEFVALB
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x13, 0xFF);
  // INTCONB
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x14, 0xFF);
  // GPPUB
  I2C_Send(&iic_info, I2C_GPIO_ADDR, 0x16, 0xFF);

  // Clear interrupts
  I2C_Receive(&iic_info, I2C_GPIO_ADDR, 0x09);
  I2C_Receive(&iic_info, I2C_GPIO_ADDR, 0x19);
}

static int getDataFromFlash()
{
  uint8_t is_data_in_flash;
  uint32_t flash_address = 0;

  /* Check if there is any data in flash. */
  is_data_in_flash = NAND_CheckDataInFlash(&flash_address);

  /* If data in flash, prepare lookout tables. */
  if(is_data_in_flash)
  {
    NAND_Reset();
    flash_address = (flash_address & 0xFFFF0000) + NAND_PAGE_SIZE;
    NAND_ReadFromFlash(&flash_address, sizeof(flash_table), (uint8_t*)&flash_table[0]);
    flash_address = (flash_address & 0xFFFF0000) + 2 * NAND_PAGE_SIZE;
    NAND_ReadFromFlash(&flash_address, sizeof(output_music), (uint8_t*)&output_music[0]);

  }
  else
  {
    return 0;
  }

  return 1;
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

static void sys_init()
{
  // SPI CS Pin
  PORTD.PDR.BIT.B4 = 1;  // Set pin as output
  NAND_CS_HIGH;

  // CPU init (cloks, RAM, etc.) and peripheral init is done in
  // resetpgr.c in PowerON_Reset_PC function.

  playMode = emptyPlay;
  R_DAC1_Start();
  R_DAC1_Set_ConversionValue(0x800);  // This is to avoid popping sound at the start
  I2C_Init();

  // Init USB Host Mass Storage Device controller
  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS;
  cfg.usb_mode = USB_HOST;
  R_USB_Open(&ctrl, &cfg);

  // Init SPI for NAND Flash
  memdrv_info.cnt = 0;
  memdrv_info.p_data = NULL;
  memdrv_info.io_mode = MEMDRV_MODE_SINGLE;

  R_MEMDRV_Open(0, &memdrv_info);
  NAND_Reset();
  NAND_CheckBlock();

  R_DMAC0_SetAddresses((void*)&S12AD.ADDR6, (void*)&volume);
  R_DMAC0_Start();
  R_ADC0_Start();
  R_DMAC1_SetAddresses((void*)&ringbuf, (void*)&DA.DADR1);
  R_DMAC1_Start();
  emptyPlayBuffer();
}

static uint8_t board_selection()
{
  if(PIN_GetDebugMode() == 1)
  {
    // Normal mode
    return PIN_BoardSelection();  // Solder jumpers MS1, MS2
  }
  else
  {
    // Debug mode
    return (((DIP_ReadState() ^ 0xFF) & 0xC0) >> 6);  // Pins 7, 8
  }
}

/*static uint8_t usb_copy_enable()
 {
 if(PIN_GetDebugMode() == 1)
 {
 //Normal mode
 return ((DIP_ReadState() & 0x40) >> 6);  //Pin 7
 }
 else
 {
 //Debug mode
 return ((DIP_ReadState() & 0x20) >> 5);  //Pin 6
 }
 }*/

void CNT_USB_LedCallback()
{
  LED_USBToggle();
}

void CNT_USB_CntCallback()
{
  usb_cnt++;
}

void CNT_IntervalDelay()
{
  static int del_cnt;

  // Because period is 100 ms, we have to multiply by 10 to get delay in seconds.
  if((del_cnt == (interval_time * 10)) || (g_systemStatus.flag_waitForInterval == 0))
  {
    R_CMT_Stop(cmt_channel_interval_delay);
    g_systemStatus.flag_waitForInterval = 0;
    del_cnt = 0;
  }

  del_cnt++;
}

/* Periodic ISR for polling status on GPIO MUX */
void ISR_periodicPolling()
{
  R_BSP_InterruptsEnable();
  uint8_t _volume;
  uint8_t gpio_rx[2];

  if(boardType == WAV_5F9IH)
  {
    // volume[0] is 16-bit variable, but contains 8-bit value. Potentiometer can accept only values from 0 to 127.
    _volume = (uint8_t)(volume[g_systemStatus.vol_ctrl_nr] >> 1);
  }
  else
  {
    _volume = 127;
  }

  // Activated when 0.
  if(0 == PIN_Get6dB() || 2 == g_binary_vol_reduction)
  {
    // -6dB is half
    _volume = _volume / 2;
  }
  else if(0 == PIN_Get14dB() || 5 == g_binary_vol_reduction)
  {
    // -14dB is fifth
    _volume = _volume / 5;
  }

  /* Send volume data to potentiometer */
  I2C_Send(&iic_info, I2C_POTENT_ADDR, 0, _volume);

  /* Read gpioa, gpiob from GPIO mux */
  gpio_rx[0] = I2C_Receive(&iic_info, I2C_GPIO_ADDR, 0x09);
  gpio_rx[1] = I2C_Receive(&iic_info, I2C_GPIO_ADDR, 0x19);

  g_systemStatus.flag_isIRQ = PIN_GetExtIRQ();

  playMode(gpio_rx);

  g_systemStatus.flag_semaphoreLock = 0;
}

/* main start */
void main(void)
{
  usb_status_t event;
  int isDataInFlash;
  nand_flash_status_t flash_status;
  uint8_t fifo_value;

  sys_init();

  /* Periodic timer for GPIO polling, period is  20 ms. */
  R_CMT_CreatePeriodic(50, &ISR_periodicPolling, &cmt_channel_i2c);

  /* Check if USB is inserted */
  // Create 100 ms counter for polling to check if USB is connected.
  R_CMT_CreatePeriodic(10, &CNT_USB_CntCallback, &cmt_channel_usb);
  while(usb_cnt < 10)
  {
    event = R_USB_GetEvent(&ctrl);  // Get event code
    R_WDT_Restart();

    switch(event)
    {
      case USB_STS_CONFIGURED:
        R_CMT_Stop(cmt_channel_i2c);  // Stop GPIO polling

        LED_USBOn();
        NAND_Reset();
        flash_status = NAND_CopyToFlash();
        if(flash_status != NAND_WRITE_OK)
        {
          isDataInFlash = 0;
          goto WHILE1;
        }

        // Create 500 ms counter for flashing USB LED.
        R_CMT_CreatePeriodic(2, &CNT_USB_LedCallback, &cmt_channel);
        break;

      case USB_STS_DETACH:
        if(flash_status == NAND_WRITE_OK)
        {
          R_CMT_Stop(cmt_channel);
          LED_USBOff();
        }
        break;

      case USB_STS_NOT_SUPPORT:
        break;

      case USB_STS_NONE:
        break;

      default:
        break;
    }
  }
  R_CMT_Stop(cmt_channel_usb);

  isDataInFlash = getDataFromFlash();
  if(!isDataInFlash)
  {
    ERROR_FlashEmpty();
    R_CMT_Stop(cmt_channel_i2c);  // Stop GPIO polling
  }

  /* Determine board type */
  boardType = board_selection();

  /* Number of switches */
  if(boardType == WAV_5F1)
  {
    g_systemStatus.nr_of_switches = 4;
  }
  else if(boardType == WAV_5F9IH)
  {
    g_systemStatus.nr_of_switches = 12;
  }
  else
  {
    g_systemStatus.nr_of_switches = 8;
  }

  /* Mode select */
  mode_select = DIP_ReadState() & 0x07;  // Switches 1, 2, 3 are mode select
  switch(mode_select)
  {
    case 0:
      if(boardType == WAV_5F9IH)
      {
        playMode = lastInputInterruptPlay;
        R_CMT_Stop(cmt_channel_i2c);
        R_CMT_CreatePeriodic(286, &ISR_periodicPolling, &cmt_channel_i2c);  // 3,5 ms
      }
      else
      {
        playMode = normalPlay;
      }
      FIFO_Init(1);
      break;

    case 1:
      if(boardType == WAV_5F9IH)
      {
        playMode = binary255_5F9IH;
      }
      else
      {
        playMode = lastInputInterruptPlay;
      }
      FIFO_Init(1);
      break;

    case 2:
      if(boardType != WAV_5F9IH)
      {
        playMode = priorityPlay;
        FIFO_Init(1);
      }
      break;

    case 3:
      if(boardType != WAV_5F9IH)
      {
        playMode = inputPlay;
        FIFO_Init(1);
      }
      break;

    case 4:
      if(boardType == WAV_5A2)
      {
        playMode = binary127ch_negative;
        FIFO_Init(MAX_BIN_BUFF_SIZE);
      }
      break;

    case 5:
      if(boardType == WAV_5F2)
      {
        playMode = binary127ch_negative;
        FIFO_Init(MAX_BIN_BUFF_SIZE);
      }
      break;

    case 6:
      if(boardType == WAV_5A2)
      {
        playMode = binary250_negative;
      }
      if(boardType == WAV_5F2)
      {
        playMode = binary255_positive;
      }
      FIFO_Init(MAX_BIN_BUFF_SIZE);
      break;

    case 7:
      if(boardType == WAV_5A2)
      {
        playMode = binary250_positive;
        PIN_BusyReset();
      }
      if(boardType == WAV_5F2)
      {
        playMode = binary255_negative;
      }
      FIFO_Init(MAX_BIN_BUFF_SIZE);
      break;
  }

  /* Inverval select */
  uint8_t _interval_select = (DIP_ReadState() & 0x18) >> 3;  // Switches 4, 5 are interval select
  if((mode_select == 0) && (boardType != WAV_5F9IH))
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

  /* Turn LED on after init is compleate and system is ready to run */
  if(isDataInFlash)
  {
    LED_PowOn();
  }

  /* Wait for interrupt from GPIO pins to start playing */
WHILE1: while(1)
  {
    R_WDT_Restart();
    /* Go to main part of program, only if data is available in flash. Otherwise loop and check if USB is attached. */
    if(isDataInFlash)
    {
      if(PIN_GetSW2() == 2)
      {
        // Position 1 (chime), continuous play of song nr. 1, volume adjustment on VR3
        g_systemStatus.vol_ctrl_nr = 0;
        playFromPlaylist(0, 1);
      }
      else if(PIN_GetSW2() == 1)
      {
        // Position 3 (voice), continuous play of song nr. 1, volume adjustment on VR4
        g_systemStatus.vol_ctrl_nr = 1;
        playFromPlaylist(0, 1);
      }
      else
      {
        /* Play song if anything is in queue */
        if(!g_systemStatus.flag_semaphoreLock)
        {
          while(FIFO_Get(&fifo_value, 1) != FIFO_EMPTY)
          {
            if(0xFF != fifo_value)
            {
              playFromPlaylist(fifo_value, 0);
            }

            /* Wait for interval time */
            if(interval_time && g_systemStatus.flag_isSongAvailable)
            {
              LED_BusyOn();
              PIN_BusyReset();
              g_systemStatus.flag_waitForInterval = 1;
              // 100 ms period
              R_CMT_CreatePeriodic(10, &CNT_IntervalDelay, &cmt_channel_interval_delay);
              while(g_systemStatus.flag_waitForInterval)
              {
                R_WDT_Restart();
              }
              LED_BusyOff();
              PIN_BusySet();
            }
            g_systemStatus.flag_semaphoreLock = 1;
          }
        }
      }
    }
  }
}
