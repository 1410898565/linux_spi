/*********************************************************************************
*      Copyright:  (C) 2018 wangtao
*                  All rights reserved.
*
*       Filename:  spi_own.c
*    Description:  This file
*                 
*        Version:  1.0.0(11/07/2018)
*         Author:  WangTao <TAlicer@163.com>
*      ChangeLog:  1, Release initial version on "11/07/2018 17:15:56 PM"
*                 
********************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "spi.h"

int initSPI()
{
  int spiFd;

  spiFd=SPISetup(0,281250); //��ʼ��SPIͨ��0�����ڵ�Ƭ�������ٶ�������281250hz
  if(spiFd==-1)
  {
      printf("init spi failed!\n");
	  return -1;
  }
}


int main()
{
  int i=0;
  int nRet = -1;
  char ctmp = 0;
  char SPIReadStartCMD = 0x1f;
  char SPIReadStopCMD = 0x2f;
  char tx_Data[160]={0xff}; //�����д������
  char rx_Data[160]={0};

  initSPI(); //spi�ĳ�ʼ��

  for(i=0;i<sizeof(tx_Data);i++)
  {
	tx_Data[i]=0xff;
  }


  while(1)
  {
    memset(rx_Data,0,sizeof(rx_Data));

    //�����ָ��
    SPIDataRW(0,&SPIReadStartCMD,&ctmp,1);
    usleep(10*1000);
    //��ʼ��ȡ����
    nRet=SPIDataRW(0,tx_Data,rx_Data,sizeof(tx_Data)); 
    usleep(10*1000);
    //������ָֹ��
    SPIDataRW(0,&SPIReadStopCMD,&ctmp,1);


    printf("rxSize=%d\nrx_Data:\n",nRet);

    for(i=0;i<sizeof(rx_Data);i++)
    {
	if(i%16==0) printf("\n");
	printf("[%2x]",rx_Data[i]);
    }

 
    printf("\n");
    usleep(10*1000);
  }
  return 0;
}


