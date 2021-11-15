/*********************************************************************************
*      Copyright:  (C) 2018 wangtao
*                  All rights reserved.
*
*       Filename:  SPISet.c
*    Description:  This file
*                 
*        Version:  1.0.0(11/07/2018)
*         Author:  WangTao <TAlicer@163.com>
*      ChangeLog:  1, Release initial version on "11/07/2018 17:15:56 PM"
*                 
********************************************************************************/
 
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi.h"
 
 
static const char   *spiDev0  = "/dev/spidev1.0" ;
static const char   *spiDev1  = "/dev/spidev0.1" ;

static  uint8_t     spiBPW   = 8 ;
static  uint16_t    spiDelay = 0 ;
 
static uint32_t     spiSpeeds [2] ;
static int          spiFds [2] ;
 
/*
* SPIDataRW:
*    Write and Read a block of data over the SPI bus.
*    Note the data ia being read into the transmit buffer, so will
*    overwrite it!
*    This is also a full-duplex operation.
*********************************************************************************
*********************************************************************************/
int SPIDataRW (int channel, unsigned char *tx_data, unsigned char *rx_data, int len)
{
  int i = 0;
  struct spi_ioc_transfer spi ;
   
  channel &= 1 ;
 
  memset (&spi, 0, sizeof (spi)) ;
 
  spi.tx_buf        = (unsigned long)tx_data ;
  spi.rx_buf        = (unsigned long)rx_data ;
  spi.len           = len ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeeds [channel] ;
  spi.bits_per_word = spiBPW ;
 
  return ioctl (spiFds [channel], SPI_IOC_MESSAGE(1), &spi) ; //SPI_IOC_MESSAGE(1)��1��ʾspi_ioc_transfer������
}


/*
* SPISetupMode:
*    Open the SPI device, and set it up, with the mode, etc.
*********************************************************************************
*********************************************************************************/
 
int SPISetupMode (int channel, int speed, int mode)
{
  int fd ;
  uint8_t lsb;

  
  if ((fd = open (channel == 0 ? spiDev0 : spiDev1, O_RDWR)) < 0)
  {
    printf("Unable to open SPI device: %s\n", strerror (errno)) ;
    return -1;
  }
 
  spiSpeeds [channel] = speed ;
  spiFds    [channel] = fd ;
  
/*
* ����spi�Ķ�дģʽ��
*  Mode 0�� CPOL=0, CPHA=0
*  Mode 1�� CPOL=0, CPHA=1
*  Mode 2�� CPOL=1, CPHA=0
*  Mode 3�� CPOL=1, CPHA=1
*  ��������Ĭ������Ϊģʽ0
*********************************************************************************
*/
  if (ioctl (fd, SPI_IOC_WR_MODE, &mode) < 0)                     
  {                                                               
    printf("Can't set spi mode: %s\n", strerror (errno)) ;         
    return -1;                                                    
  }                                                               
 
  if (ioctl (fd, SPI_IOC_RD_MODE, &mode) < 0)                     
  {                                                               
    printf("Can't get spi mode: %s\n", strerror (errno)) ;        
    return -1;                                                 
  }    
  
/*
* spi�Ķ�дbit/word���ÿ�д
*    ��������Ϊ8��λΪһ���ֽ�
*********************************************************************************
*/
  if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)          
  {                                                               
    printf("Can't set bits per word: %s\n", strerror (errno))  ;  
    return -1;                                                    
  }                                                              
  
  if (ioctl (fd, SPI_IOC_RD_BITS_PER_WORD, &spiBPW) < 0)          
  {                                                               
    printf("Can't get bits per word: %s\n", strerror (errno))  ;  
    return -1;                                                   
  }   
  
/*
* ����spi��д����
*********************************************************************************
*/
  if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
  {
    printf("Can't set max speed hz: %s\n", strerror (errno));
    return -1;
  }
  
  if (ioctl (fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
  {
    printf("Can't get max speed hz: %s\n", strerror (errno));
    return -1;
  }

  
  if (ioctl (fd, SPI_IOC_RD_LSB_FIRST, &lsb) < 0)
  {
	  printf("can't get lsb first hz");
  }

  printf("spi mode: %d\n", mode);
  printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);
  printf("lsb: %d\n", lsb);


  return fd ;
}
 
 
/*
* SPISetup:
*    Open the SPI device, and set it up, etc. in the default MODE 0
*********************************************************************************
*********************************************************************************/
 
int SPISetup (int channel, int speed)
{
  int nMode = SPI_MODE_0;
  return SPISetupMode (channel, speed, nMode) ;
}

