/**
 ******************************************************************************
 * @file    ov2640.c
 * @author  MCD Application Team
 * @version V1.0.1
 * @date    19-June-2014
 * @brief   This file provides the OV2640 camera driver
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
#define DEBUG
/* Includes ------------------------------------------------------------------*/
#include "drivers.h"
#include "app.h"
#include "api.h"

#include "ov2640_reg.h"

static uint32_t ov2640_ConvertValue(uint32_t feature, uint32_t value);

//返回：1-成功	0-失败
unsigned char wrOV2640Reg(unsigned char regID, unsigned char regDat)
{
    startSCCB();
    if (0 == SCCBwriteByte(0x60))
    {
        stopSCCB();
        return (0);
    }
    delay_us(100);
    if (0 == SCCBwriteByte(regID))
    {
        stopSCCB();
        return (0);
    }
    delay_us(100);
    if (0 == SCCBwriteByte(regDat))
    {
        stopSCCB();
        return (0);
    }
    stopSCCB();

    return (1);
}


//返回：1-成功	0-失败
unsigned char rdOV2640Reg(unsigned char regID, unsigned char *regDat)
{
    //通过写操作设置寄存器地址
    startSCCB();
    if (0 == SCCBwriteByte(0x60))
    {
        stopSCCB();
        return (0);
    }
    delay_us(100);
    if (0 == SCCBwriteByte(regID))
    {
        stopSCCB();
        return (0);
    }
    stopSCCB();

    delay_us(100);

    //设置寄存器地址后，才是读
    startSCCB();
    if (0 == SCCBwriteByte(0x61))
    {
        stopSCCB();
        return (0);
    }
    delay_us(100);
    *regDat = SCCBreadByte();
    noAck();
    stopSCCB();
    return (1);
}

/**
  * @brief  Writes a byte at a specific Camera register
  * @param  Addr: OV2640 register address.
  * @param  Data: Data to be written to the specific register 
  * @retval 0x00 if write operation is OK.
  *       0xFF if timeout condition occured (device not connected or bus error).
  */
uint8_t OV2640_WriteReg(uint16_t Addr, uint8_t Data)
{
  uint32_t timeout = DCMI_TIMEOUT_MAX;
  
  /* Generate the Start Condition */
  I2C_GenerateSTART(CAMERA_I2C, ENABLE);

  /* Test on CAMERA_I2C EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
   
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(CAMERA_I2C, OV2640_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter);
 
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
 
  /* Send CAMERA_I2C location address LSB */
  I2C_SendData(CAMERA_I2C, (uint8_t)(Addr));

  /* Test on CAMERA_I2C EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }
  
  /* Send Data */
  I2C_SendData(CAMERA_I2C, Data);

  /* Test on CAMERA_I2C EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }  
 
  /* Send CAMERA_I2C STOP Condition */
  I2C_GenerateSTOP(CAMERA_I2C, ENABLE);
  
  /* If operation is OK, return 0 */
  return 0;
}

/**
  * @brief  Reads a byte from a specific Camera register
  * @param  Addr: OV2640 register address.
  * @retval data read from the specific register or 0xFF if timeout condition
  *         occured. 
  */
uint8_t OV2640_ReadReg(uint16_t Addr)
{
  uint32_t timeout = DCMI_TIMEOUT_MAX;
  uint8_t Data = 0;

  /* Generate the Start Condition */
  I2C_GenerateSTART(CAMERA_I2C, ENABLE);

  /* Test on CAMERA_I2C EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(CAMERA_I2C, OV2640_DEVICE_READ_ADDRESS, I2C_Direction_Transmitter);
 
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 

  /* Send CAMERA_I2C location address LSB */
  I2C_SendData(CAMERA_I2C, (uint8_t)(Addr));

  /* Test on CAMERA_I2C EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Clear AF flag if arised */
  CAMERA_I2C->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(CAMERA_I2C, ENABLE);
  
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(CAMERA_I2C, OV2640_DEVICE_READ_ADDRESS, I2C_Direction_Receiver);
   
  /* Test on CAMERA_I2C EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }  
 
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(CAMERA_I2C, DISABLE);

  /* Test on CAMERA_I2C EV7 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }   
    
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(CAMERA_I2C, ENABLE);

  /* Receive the Data */
  Data = I2C_ReceiveData(CAMERA_I2C);

  /* return the read data */
  return Data;
}



void CAMERA_IO_Write(uint8_t Reg, uint8_t Value)
{
    OV2640_WriteReg(Reg, Value);
}

/**
 * @brief  Camera reads single data.
 * @param  Addr: I2C address
 * @param  Reg: Register address 
 * @retval Read data
 */
uint8_t CAMERA_IO_Read(uint8_t Reg)
{
    uint8_t value;
    value = OV2640_ReadReg(Reg);
//		if(value != 0xff)
//			p_err("CAMERA_IO_Read faild");
    return value;
}

void CAMERA_Delay(uint32_t val)
{
   sleep(val);
}

void OV2640_Reset(void)
{
    CAMERA_IO_Write(OV2640_DSP_RA_DLMT, 0x01);
    CAMERA_IO_Write(OV2640_SENSOR_COM7, 0x80);
}

/**
 * @}
 */

/** @defgroup OV2640_Private_Functions
 * @{
 */
void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat)
{
    uint32_t i;

    OV2640_Reset();
    sleep(200);

    /* Initialize OV2640 */
    for (i = 0; i < (sizeof(OV2640_JPEG_INIT) / 2); i++)
    {
        CAMERA_IO_Write(OV2640_JPEG_INIT[i][0], OV2640_JPEG_INIT[i][1]);
    }

    /* Set to output YUV422 */
    for (i = 0; i < (sizeof(OV2640_YUV422) / 2); i++)
    {
        CAMERA_IO_Write(OV2640_YUV422[i][0], OV2640_YUV422[i][1]);
    }

    CAMERA_IO_Write(0xff, 0x01);
    CAMERA_IO_Write(0x15, 0x00);

    /* Set to output JPEG */
    for (i = 0; i < (sizeof(OV2640_JPEG) / 2); i++)
    {
        CAMERA_IO_Write(OV2640_JPEG[i][0], OV2640_JPEG[i][1]);
    }

    sleep(100);

    switch (ImageFormat)
    {
        case JPEG_176x144:
            {
                p_dbg("set ImageFormat: JPEG_176x144");
                for (i = 0; i < (sizeof(OV2640_176x144_JPEG) / 2); i++)
                {
                    CAMERA_IO_Write(OV2640_176x144_JPEG[i][0], OV2640_176x144_JPEG[i][1]);
                }
                break;
            }
        case JPEG_320x240:
            {
                p_dbg("set ImageFormat: JPEG_320x240");
                for (i = 0; i < (sizeof(OV2640_320x240_JPEG) / 2); i++)
                {
                    CAMERA_IO_Write(OV2640_320x240_JPEG[i][0], OV2640_320x240_JPEG[i][1]);
                }
                break;
            }
        case JPEG_352x288:
            {
                p_dbg("set ImageFormat: JPEG_352x288");
                for (i = 0; i < (sizeof(OV2640_352x288_JPEG) / 2); i++)
                {
                    CAMERA_IO_Write(OV2640_352x288_JPEG[i][0], OV2640_352x288_JPEG[i][1]);
                }
                break;
            }
        case JPEG_640x480:
            {
                p_dbg("set ImageFormat: JPEG_640x480");
                for (i = 0; i < (sizeof(OV2640_640x480_JPEG) / 2); i++)
                {
                    CAMERA_IO_Write(OV2640_640x480_JPEG[i][0], OV2640_640x480_JPEG[i][1]);
                }
                break;
            }
        case JPEG_800x600:
            {
                p_dbg("set ImageFormat: JPEG_800x600");
                for (i = 0; i < (sizeof(OV2640_800x600_JPEG) / 2); i++)
                {
                    CAMERA_IO_Write(OV2640_800x600_JPEG[i][0], OV2640_800x600_JPEG[i][1]);
                }
                break;
            }
        case JPEG_1024x768:
            {
                p_dbg("set ImageFormat: JPEG_1024x768");
                for (i = 0; i < (sizeof(OV2640_1024x768_JPEG) / 2); i++)
                {
                    CAMERA_IO_Write(OV2640_1024x768_JPEG[i][0], OV2640_1024x768_JPEG[i][1]);
                }
                break;
            }
        default:
            {
                p_dbg("set ImageFormat: JPEG_640x480");
                for (i = 0; i < (sizeof(OV2640_640x480_JPEG) / 2); i++)
                {
                    CAMERA_IO_Write(OV2640_640x480_JPEG[i][0], OV2640_640x480_JPEG[i][1]);
                }
                break;
            }
    }
}

/**
 * @brief  Initializes the OV2640 CAMERA component.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  resolution: Camera resolution
 * @retval None
 */
int ov2640_Init()
{
	  GPIO_InitTypeDef GPIO_InitStructure;	
	  I2C_InitTypeDef  I2C_InitStruct;
    uint32_t index;

    /* Initialize I2C */
    gpio_cfg((uint32_t)DCMI_RST_PORT_GROUP, DCMI_RST_PIN, GPIO_Mode_Out_PP);
    gpio_cfg((uint32_t)DCMI_PWD_PORT_GROUP, DCMI_PWD_PIN, GPIO_Mode_Out_PP);

    GPIO_SET(DCMI_RST_PORT_GROUP, DCMI_RST_PIN);
    GPIO_CLR(DCMI_PWD_PORT_GROUP, DCMI_PWD_PIN);

//    SCCB_INIT();
 /****** Configures the I2C2 used for OV2640 camera module configuration *****/
	 /* I2C2 clock enable */
	  RCC_APB1PeriphClockCmd(CAMERA_I2C_CLK, ENABLE);

	  /* GPIOB clock enable */
	  RCC_AHB1PeriphClockCmd(CAMERA_I2C_SCL_GPIO_CLK|CAMERA_I2C_SDA_GPIO_CLK, ENABLE);

	  /* Connect I2C2 pins to AF4 */
	  GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);

	  /* Configure I2C2 GPIOs */
	  GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SCL_PIN ;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(CAMERA_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);

	  GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SDA_PIN ;
	  GPIO_Init(CAMERA_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(CAMERA_I2C_SDA_GPIO_PORT, CAMERA_I2C_SDA_SOURCE, CAMERA_I2C_SDA_AF);

	  /* Configure I2C2 */
	  /* I2C DeInit */
	  I2C_DeInit(CAMERA_I2C);

	  /* Enable the I2C peripheral */
	  I2C_Cmd(CAMERA_I2C, ENABLE);

	  /* Set the I2C structure parameters */
	  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	  I2C_InitStruct.I2C_OwnAddress1 = 0xFE;
	  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	  I2C_InitStruct.I2C_ClockSpeed = 30000;

	  /* Initialize the I2C peripheral w/ selected parameters */
	  I2C_Init(CAMERA_I2C, &I2C_InitStruct);
		
		
		
    /* Prepare the camera to be configured */
    CAMERA_IO_Write(OV2640_DSP_RA_DLMT, 0x01);
    CAMERA_IO_Write(OV2640_SENSOR_COM7, 0x80);
    CAMERA_Delay(200);

    index = ov2640_ReadID();

    p_dbg("id:%x", index);
    if (index != OV2640_ID)
        return  - 1;

    OV2640_JPEGConfig(JPEG_320x240);

    return 0;
}

/**
 * @brief  Configures the OV2640 camera feature.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  feature: Camera feature to be configured
 * @param  value: Value to be configured
 * @param  brightness_value: Brightness value to be configured
 * @retval None
 */
void ov2640_Config(uint32_t feature, uint32_t value, uint32_t brightness_value)
{
    uint8_t value1, value2;
    uint32_t value_tmp;
    uint32_t br_value;

    /* Convert the input value into ov2640 parameters */
    value_tmp = ov2640_ConvertValue(feature, value);
    br_value = ov2640_ConvertValue(CAMERA_CONTRAST_BRIGHTNESS, brightness_value);

    switch (feature)
    {
        case CAMERA_BLACK_WHITE:
            {
                CAMERA_IO_Write(0xff, 0x00);
                CAMERA_IO_Write(0x7c, 0x00);
                CAMERA_IO_Write(0x7d, value_tmp);
                CAMERA_IO_Write(0x7c, 0x05);
                CAMERA_IO_Write(0x7d, 0x80);
                CAMERA_IO_Write(0x7d, 0x80);
                break;
            }
        case CAMERA_CONTRAST_BRIGHTNESS:
            {
                value1 = (uint8_t)(value_tmp);
                value2 = (uint8_t)(value_tmp >> 8);
                CAMERA_IO_Write(0xff, 0x00);
                CAMERA_IO_Write(0x7c, 0x00);
                CAMERA_IO_Write(0x7d, 0x04);
                CAMERA_IO_Write(0x7c, 0x07);
                CAMERA_IO_Write(0x7d, br_value);
                CAMERA_IO_Write(0x7d, value1);
                CAMERA_IO_Write(0x7d, value2);
                CAMERA_IO_Write(0x7d, 0x06);
                break;
            }
        case CAMERA_COLOR_EFFECT:
            {
                value1 = (uint8_t)(value_tmp);
                value2 = (uint8_t)(value_tmp >> 8);
                CAMERA_IO_Write(0xff, 0x00);
                CAMERA_IO_Write(0x7c, 0x00);
                CAMERA_IO_Write(0x7d, 0x18);
                CAMERA_IO_Write(0x7c, 0x05);
                CAMERA_IO_Write(0x7d, value1);
                CAMERA_IO_Write(0x7d, value2);
                break;
            }
        default:
            {
                break;
            }
    }
}

/**
 * @brief  Read the OV2640 Camera identity.
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval the OV2640 ID
 */
uint16_t ov2640_ReadID()
{
    /* Initialize I2C */
    //CAMERA_IO_Init();

    /* Prepare the sensor to read the Camera ID */
    CAMERA_IO_Write(OV2640_DSP_RA_DLMT, 0x01);

    /* Get the camera ID */
    return (CAMERA_IO_Read(OV2640_SENSOR_PIDH));
}

/******************************************************************************
Static Functions
 *******************************************************************************/
/**
 * @brief  Convert input values into ov2640 parameters.
 * @param  feature: Camera feature to be configured
 * @param  value: Value to be configured
 * @retval The converted value
 */
static uint32_t ov2640_ConvertValue(uint32_t feature, uint32_t value)
{
    uint32_t ret = 0;

    switch (feature)
    {
        case CAMERA_BLACK_WHITE:
            {
                switch (value)
                {
                case CAMERA_BLACK_WHITE_BW:
                    {
                        ret = OV2640_BLACK_WHITE_BW;
                        break;
                    }
                case CAMERA_BLACK_WHITE_NEGATIVE:
                    {
                        ret = OV2640_BLACK_WHITE_NEGATIVE;
                        break;
                    }
                case CAMERA_BLACK_WHITE_BW_NEGATIVE:
                    {
                        ret = OV2640_BLACK_WHITE_BW_NEGATIVE;
                        break;
                    }
                case CAMERA_BLACK_WHITE_NORMAL:
                    {
                        ret = OV2640_BLACK_WHITE_NORMAL;
                        break;
                    }
                default:
                    {
                        ret = OV2640_BLACK_WHITE_NORMAL;
                        break;
                    }
                }
                break;
            }
        case CAMERA_CONTRAST_BRIGHTNESS:
            {
                switch (value)
                {
                case CAMERA_BRIGHTNESS_LEVEL0:
                    {
                        ret = OV2640_BRIGHTNESS_LEVEL0;
                        break;
                    }
                case CAMERA_BRIGHTNESS_LEVEL1:
                    {
                        ret = OV2640_BRIGHTNESS_LEVEL1;
                        break;
                    }
                case CAMERA_BRIGHTNESS_LEVEL2:
                    {
                        ret = OV2640_BRIGHTNESS_LEVEL2;
                        break;
                    }
                case CAMERA_BRIGHTNESS_LEVEL3:
                    {
                        ret = OV2640_BRIGHTNESS_LEVEL3;
                        break;
                    }
                case CAMERA_BRIGHTNESS_LEVEL4:
                    {
                        ret = OV2640_BRIGHTNESS_LEVEL4;
                        break;
                    }
                case CAMERA_CONTRAST_LEVEL0:
                    {
                        ret = OV2640_CONTRAST_LEVEL0;
                        break;
                    }
                case CAMERA_CONTRAST_LEVEL1:
                    {
                        ret = OV2640_CONTRAST_LEVEL1;
                        break;
                    }
                case CAMERA_CONTRAST_LEVEL2:
                    {
                        ret = OV2640_CONTRAST_LEVEL2;
                        break;
                    }
                case CAMERA_CONTRAST_LEVEL3:
                    {
                        ret = OV2640_CONTRAST_LEVEL3;
                        break;
                    }
                case CAMERA_CONTRAST_LEVEL4:
                    {
                        ret = OV2640_CONTRAST_LEVEL4;
                        break;
                    }
                default:
                    {
                        ret = OV2640_CONTRAST_LEVEL0;
                        break;
                    }
                }
                break;
            }
        case CAMERA_COLOR_EFFECT:
            {
                switch (value)
                {
                case CAMERA_COLOR_EFFECT_ANTIQUE:
                    {
                        ret = OV2640_COLOR_EFFECT_ANTIQUE;
                        break;
                    }
                case CAMERA_COLOR_EFFECT_BLUE:
                    {
                        ret = OV2640_COLOR_EFFECT_BLUE;
                        break;
                    }
                case CAMERA_COLOR_EFFECT_GREEN:
                    {
                        ret = OV2640_COLOR_EFFECT_GREEN;
                        break;
                    }
                case CAMERA_COLOR_EFFECT_RED:
                    {
                        ret = OV2640_COLOR_EFFECT_RED;
                        break;
                    }
                default:
                    {
                        ret = OV2640_COLOR_EFFECT_RED;
                        break;
                    }
                }
                break;
            default:
                {
                    ret = 0;
                    break;
                }
            }
    }

    return ret;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

 /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
