/**
  ******************************************************************************
  * @file    aux_data.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用于从SD卡烧录FLASH出厂数据
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */  


#include "aux_data.h"
#include "ff.h"
#include "./fatfs/drivers/fatfs_flash_spi.h"
#include "./Bsp/led/bsp_led.h" 


/***************************************************************************************************************/
/*
【 ！】外部Flash使用情况说明（W25Q128）	

本程序正常运行后，会给FLASH芯片刷成如下内容。
//Tab = 4个空格

|-------------------------------------------------------------------------------------------------------------------|												
|序号	|文件名/工程					|功能										|起始地址		|长度				|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|1		|外部flash读写例程			|预留给裸机Flash测试							|0				|4096 (BYTE)		|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|2		|预留						|预留										|1*4096			|59*4096 (BYTE)		|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|3		|app.c						|XBF字库文件（emWin使用,新宋体25.xbf）		|60*4096		|649*4096(1.23MB)	|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|4		|app.c						|XBF字库文件(emWin使用,新宋体19.xbf)			|710*4096		|529*4096(172KB)	|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|4		|firecc936.c				|文件系统中文支持字库(emWin使用,UNIGBK.BIN)	|1240*4096		|43*4096(172KB)		|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|5		|EMW1062模块				|WIFI模块固件(BCM43362-5.90.230.12.bin)		|1284*4096		|62*4096(248KB)		|
|5.1	|EMW1062模块				|WIFI模块参数1(预留，不需要写文件)		    |1347*4096		|1*4096(4KB)		|
|5.2	|EMW1062模块				|WIFI模块参数2(预留，不需要写文件)		    |1348*4096		|1*4096(4KB)		|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|6		|裸机中文显示例程			|裸机中文字库（GB2312_H2424.FON）			|1360*4096		|144*4096(576KB)	|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|7		|diskio.c					|FATFS文件系统（emWin使用）					|1536*4096		|2560*4096(10MB)	|
|-------------------------------------------------------------------------------------------------------------------|
*/
/*******************************************************************************************************************/



Aux_Data_Typedef  burn_data[] =  
{
  [AUX_XBF_XINSONGTI_25] =
  {
    .filename           =   "0:新宋体25.xbf",
    .description        =   "XBF字库文件（emWin使用,新宋体25.xbf）",
    .start_addr         =   60*4096 ,
    .length             =   649*4096, 
    .burn_option      =  UPDATE,
  },
  
  [AUX_XBF_XINSONGTI_19] =
  {
    .filename           =   "0:新宋体19.xbf",
    .description        =   "XBF字库文件(emWin使用,新宋体19.xbf)",
    .start_addr         =   710*4096 ,
    .length             =   529*4096, 
    .burn_option      =  UPDATE,
  },
  
  [AUX_UNIGBK] =
  {
    .filename           =   "0:UNIGBK.BIN",
    .description        =   "文件系统中文支持字库(emWin使用,UNIGBK.BIN)",
    .start_addr         =   1240*4096 ,
    .length             =   43*4096,
    .burn_option      =  UPDATE,      
  },
  
  [AUX_WIFI_FW] =
  {
    .filename           =   "0:BCM43362-5.90.230.12.bin",
    .description        =   "WIFI模块固件",
    .start_addr         =   1284*4096 ,
    .length             =   62*4096, 
    .burn_option      =  UPDATE,
  },
  
  [AUX_WIFI_PARA1] =
  {
    .filename           =   "",
    .description        =   "WIFI模块参数1",
    .start_addr         =   1347*4096 ,
    .length             =   1*4096, 
    .burn_option      =  DO_NOT_UPDATE,
  },
  
  [AUX_WIFI_PARA2] =
  {
    .filename           =   "",
    .description        =   "WIFI模块参数2",
    .start_addr         =   1348*4096 ,
    .length             =   1*4096, 
    .burn_option      =  DO_NOT_UPDATE,
  },
  
  [AUX_GB2312_H2424] =
  {
    .filename           =   "0:GB2312_H2424.FON",
    .description        =   "裸机中文字库",
    .start_addr         =   1360*4096 ,
    .length             =   144*4096,
    .burn_option      =  UPDATE,      
  },
  
  [AUX_FILE_SYSTEM] =
  {
    .filename           =   "",
    .description        =   "FATFS文件系统",
    .start_addr         =   1536*4096 ,
    .length             =   2560*4096, 
    .burn_option      =  DO_NOT_UPDATE,
  }

};

FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT result; 
UINT  bw;            					    /* File R/W count */
 
/**
  * @brief  向FLASH写入文件
  * @param  dat：要写入的文件数据的信息
  * @param  file_num：要写入的文件个数
  * @retval 正常返回FR_OK
  */
int burn_file_sd2flash(Aux_Data_Typedef *dat,uint8_t file_num) 
{
    uint8_t i;

    uint32_t write_addr=0,j=0;
    uint8_t tempbuf[256],flash_buf[256];
    
    result = f_mount(&fs,"0:",1);
    
    //如果文件系统挂载失败就退出
    if(result != FR_OK)
    {
      BURN_ERROR("f_mount ERROR!");
      LED_RED;
      return result;
    }
    
    for(i=0;i<file_num;i++)
    {
       if (dat[i].burn_option == DO_NOT_UPDATE)
          continue;
       
       BURN_INFO("-------------------------------------"); 
       BURN_INFO("准备烧录内容：%s",dat[i].description);
       LED_BLUE;
       
       result = f_open(&fnew,dat[i].filename,FA_OPEN_EXISTING | FA_READ);
        if(result != FR_OK)
        {
            BURN_ERROR("打开文件失败！");
            LED_RED;
            return result;
        }
        
      BURN_INFO("正在擦除要使用的FLASH空间...");
  
      write_addr = dat[i].start_addr;
        
      for(j=0;j < dat[i].length/4096 ;j++)//擦除扇区，起始位置710*4096共2116KB
      {
        SPI_FLASH_SectorErase(write_addr+j*4096);
      }
      
      BURN_INFO("正在向FLASH写入内容...");
      
      write_addr = dat[i].start_addr;
      while(result == FR_OK) 
      {
        result = f_read( &fnew, tempbuf, 256, &bw);//读取数据	 
        if(result!=FR_OK)			 //执行错误
        {
          BURN_ERROR("读取文件失败！");
          LED_RED;
          return result;
        }      
        SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //拷贝数据到外部flash上    
        write_addr+=256;				
        if(bw !=256)break;
      }

        BURN_INFO("内容写入完毕,开始校验数据...");

        //校验数据
      write_addr = dat[i].start_addr;
     
      f_lseek(&fnew,0);
      
      while(result == FR_OK) 
      {
        result = f_read( &fnew, tempbuf, 256, &bw);//读取数据	 
        if(result!=FR_OK)			 //执行错误
        {
          BURN_ERROR("读取文件失败！");
          LED_RED;
          return result;
        }      
        SPI_FLASH_BufferRead(flash_buf,write_addr,bw);  //从FLASH中读取数据
        write_addr+=bw;		
        
        for(j=0;j<bw;j++)
        {
          if(tempbuf[i] != flash_buf[i])
          {
            BURN_ERROR("数据校验失败！");
            LED_RED;
            return -1;
          }
         }  
     
        if(bw !=256)break;//到了文件尾
      }
      

      BURN_INFO("数据校验成功！");
      BURN_INFO("文件：%s 写入完成",dat[i].filename);
      BURN_INFO("-------------------------------------");
      LED_GREEN;

      f_close(&fnew); 
    }
    
    
    BURN_INFO("************************************");
    BURN_INFO("所有文件均已烧录成功！");
    return FR_OK;


}

















