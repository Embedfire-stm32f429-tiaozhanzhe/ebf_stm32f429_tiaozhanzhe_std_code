#define DEBUG
#include "drivers.h"
#include "app.h"
#include "api.h"


static const u8 OV7670_reg[][2]=
{	 
#if 1
	{0x3a, 0x0C}, 
	{0x40, 0xd0}, 
	{0x32, 0x80}, 
	{0x17, 0x16}, 
	{0x18, 0x04}, 
	{0x19, 0x02}, 
	{0x1a, 0x7a},//0x7a, 
	{0x03, 0x05},//0x0a, 	
#if 1  // 原尺寸
	{0x12, 0x14}, //COM7  QVGA
	{0x0c, 0x00}, 
	{0x3e, 0x00},// 0x00
	{0x70, 0}, 
	{0x71, 0}, 
	{0x72, 0x11}, 
	{0x73, 0x00},// 
#else  // 缩小一半
	{0x12, 0x14}, //COM7  QVGA
	{0x0c, 0x0c}, 
	{0x3e, 0x18},// 0x00
	{0x70, 64}, 
	{0x71, 64}, 
	{0x72, 0x11}, 
	{0x73, 0x08},//
#endif
	{0xa2, 0x02}, 
	{0x11, 0x40}, 
	{0x7a, 0x20}, 
	{0x7b, 0x1c}, 
	{0x7c, 0x28}, 
	{0x7d, 0x3c}, 
	{0x7e, 0x55}, 
	{0x7f, 0x68}, 
	{0x80, 0x76}, 
	{0x81, 0x80}, 
	{0x82, 0x88}, 
	{0x83, 0x8f}, 
	{0x84, 0x96}, 
	{0x85, 0xa3}, 
	{0x86, 0xaf}, 
	{0x87, 0xc4}, 
	{0x88, 0xd7}, 
	{0x89, 0xe8}, 
	{0x13, 0xe0}, 
	{0x00, 0x00},//AGC 
	{0x10, 0x00}, 
	{0x0d, 0x00}, 
	{0x14, 0x28},//0x38, limit the max gain 
	{0xa5, 0x05}, 
	{0xab, 0x07}, 
	{0x24, 0x75}, 
	{0x25, 0x63}, 
	{0x26, 0xA5}, 
	{0x9f, 0x78}, 
	{0xa0, 0x68}, 
	{0xa1, 0x03},//0x0b, 
	{0xa6, 0xdf},//0xd8, 
	{0xa7, 0xdf},//0xd8, 
	{0xa8, 0xf0}, 
	{0xa9, 0x90}, 
	{0xaa, 0x94}, 
	{0x13, 0xe5}, 
	{0x0e, 0x61}, 
	{0x0f, 0x4b}, 
	{0x16, 0x02}, 
	{0x1e, 0x07},//0x07, 翻转
	{0x21, 0x02}, 
	{0x22, 0x91}, 
	{0x29, 0x07}, 
	{0x33, 0x0b}, 
	{0x35, 0x0b}, 
	{0x37, 0x1d}, 
	{0x38, 0x71}, 
	{0x39, 0x2a}, 
	{0x3c, 0x78}, 
	{0x4d, 0x40}, 
	{0x4e, 0x20}, 
	{0x69, 0x55}, 
	{0x6b, 0x0A},//PLL 重要参数 
	{0x74, 0x19}, 
	{0x8d, 0x4f}, 
	{0x8e, 0x00}, 
	{0x8f, 0x00}, 
	{0x90, 0x00}, 
	{0x91, 0x00}, 
	{0x92, 0x00},//0x19,//0x66 
	{0x96, 0x00}, 
	{0x9a, 0x80}, 
	{0xb0, 0x84}, 
	{0xb1, 0x0c}, 
	{0xb2, 0x0e}, 
	{0xb3, 0x82}, 
	{0xb8, 0x0a}, 
	{0x43, 0x14}, 
	{0x44, 0xf0}, 
	{0x45, 0x34}, 
	{0x46, 0x58}, 
	{0x47, 0x28}, 
	{0x48, 0x3a}, 
	{0x59, 0x88}, 
	{0x5a, 0x88}, 
	{0x5b, 0x44}, 
	{0x5c, 0x67}, 
	{0x5d, 0x49}, 
	{0x5e, 0x0e}, 
	{0x64, 0x04}, 
	{0x65, 0x20}, 
	{0x66, 0x05}, 
	{0x94, 0x04}, 
	{0x95, 0x08}, 
	{0x6c, 0x0a}, 
	{0x6d, 0x55}, 
	{0x6e, 0x11}, 
	{0x6f, 0x9f},//0x9e for advance AWB 
	{0x6a, 0x40}, 
	{0x01, 0x40}, 
	{0x02, 0x40}, 
	{0x13, 0xe7}, 
	{0x15, 0x00}, //?//重要参数 
	{0x4f, 0x80}, 
	{0x50, 0x80}, 
	{0x51, 0x00}, 
	{0x52, 0x22}, 
	{0x53, 0x5e}, 
	{0x54, 0x80}, 
	{0x55, 0x0A},//亮度 
	{0x56, 0x4f},//对比度 
	{0x58, 0x9e},	
	{0x41, 0x08}, 
	{0x3f, 0x05},//边缘增强调整 
	{0x75, 0x05}, 
	{0x76, 0xe1}, 
	{0x4c, 0x0F},//噪声抑制强度 
	{0x77, 0x0a}, 
	{0x3d, 0xc2},//0xc0, 
	{0x4b, 0x09}, 
	{0xc9, 0x60}, 
	{0x41, 0x38}, 
	{0x34, 0x11}, 
	{0x3b, 0x02},//0x00,//0x02, 
	{0xa4, 0x89},//0x88, 
	{0x96, 0x00}, 
	{0x97, 0x30}, 
	{0x98, 0x20}, 
	{0x99, 0x30}, 
	{0x9a, 0x84}, 
	{0x9b, 0x29}, 
	{0x9c, 0x03}, 
	{0x9d, 0x4c}, 
	{0x9e, 0x3f}, 
	{0x78, 0x04},	
	{0x79, 0x01}, 
	{0xc8, 0xf0}, 
	{0x79, 0x0f}, 
	{0xc8, 0x00}, 
	{0x79, 0x10}, 
	{0xc8, 0x7e}, 
	{0x79, 0x0a}, 
	{0xc8, 0x80}, 
	{0x79, 0x0b}, 
	{0xc8, 0x01}, 
	{0x79, 0x0c}, 
	{0xc8, 0x0f}, 
	{0x79, 0x0d}, 
	{0xc8, 0x20}, 
	{0x79, 0x09}, 
	{0xc8, 0x80}, 
	{0x79, 0x02}, 
	{0xc8, 0xc0}, 
	{0x79, 0x03}, 
	{0xc8, 0x40}, 
	{0x79, 0x05}, 
	{0xc8, 0x30}, 
	{0x79, 0x26}, 
	{0x09, 0x03}, 
	{0x3b, 0x42},//0x82,//0xc0,//0xc2,	//night mode 
#endif
};



////////////////////////////
//功能：写OV7670寄存器
//返回：1-成功	0-失败
unsigned char wrOV7670Reg(unsigned char regID, unsigned char regDat)
{
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regDat))
	{
		stopSCCB();
		return(0);
	}
  	stopSCCB();
	
  	return(1);
}
////////////////////////////
//功能：读OV7670寄存器
//返回：1-成功	0-失败
unsigned char rdOV7670Reg(unsigned char regID, unsigned char *regDat)
{
	//通过写操作设置寄存器地址
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	stopSCCB();
	
	delay_us(100);
	
	//设置寄存器地址后，才是读
	startSCCB();
	if(0==SCCBwriteByte(0x43))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	*regDat=SCCBreadByte();
  	noAck();
  	stopSCCB();
  	return(1);
}

int set_OV7670reg(void)
{
	int i;
	for(i = 0; i < sizeof(OV7670_reg)/2; i++)
  	{
    	if(wrOV7670Reg(OV7670_reg[i][0],OV7670_reg[i][1]) == 0)
			return -1;
  	}

	return 0;
	
}


uint32_t red_ov7673_id()
{
	uint8_t id[4];
	uint32_t ret;
	rdOV7670Reg(0x0a, &id[0]);
	rdOV7670Reg(0x0b, &id[1]);
	rdOV7670Reg(0x1c, &id[2]);
	rdOV7670Reg(0x1d, &id[3]);
	ret = id[2]*256 + id[3];
	p_dbg("ov7673 id:%x, ver:%x, mid:%x", id[0], id[1], ret);
	memcpy(&ret, id, 4);
	return *((uint32_t*)id);
}

								  
void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height)
{
	unsigned int endx;
	unsigned int endy;// "v*2"必须
	unsigned char temp_reg1, temp_reg2;
	unsigned char temp=0;
	
	endx=(startx+width);
	endy=(starty+height+height);// "v*2"必须
        rdOV7670Reg(0x03, &temp_reg1 );
	temp_reg1 &= 0xf0;
	rdOV7670Reg(0x32, &temp_reg2 );
	temp_reg2 &= 0xc0;
	
	// Horizontal
	temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
	wrOV7670Reg(0x32, temp );
	temp = (startx&0x7F8)>>3;
	wrOV7670Reg(0x17, temp );
	temp = (endx&0x7F8)>>3;
	wrOV7670Reg(0x18, temp );
	
	// Vertical
	temp =temp_reg1|((endy&0x3)<<2)|(starty&0x3);
	wrOV7670Reg(0x03, temp );
	temp = starty>>2;
	wrOV7670Reg(0x19, temp );
	temp = endy>>2;
	wrOV7670Reg(0x1A, temp );
}

void OV7670_HW(u16 hstart,u16 vstart,u16 hstop,u16 vstop)
{
	u8 v;		
	wrOV7670Reg(0x17,(hstart>>3)&0xff);//HSTART
	wrOV7670Reg(0x18,(hstop>>3)&0xff);//HSTOP
	rdOV7670Reg(0x32,&v);
	v=(v&0xc0)|((hstop&0x7)<<3)|(hstart&0x7);
	wrOV7670Reg(0x32,v);//HREF
	
	wrOV7670Reg(0x19,(vstart>>2)&0xff);//VSTART ?aê???8??
	wrOV7670Reg(0x1a,(vstop>>2)&0xff);//VSTOP	?áê???8??
	rdOV7670Reg(0x03,&v);
	v=(v&0xf0)|((vstop&0x3)<<2)|(vstart&0x3);	
	wrOV7670Reg(0x03,v);//VREF																 
	wrOV7670Reg(0x11,0x00);
}		

/* OV7670_init() */
//返回1成功，返回0失败
int OV7670_init()
{
	unsigned char temp, retry = 10;	

	gpio_cfg((uint32_t)DCMI_RST_PORT_GROUP, DCMI_RST_PIN, GPIO_Mode_Out_PP);
	gpio_cfg((uint32_t)DCMI_PWD_PORT_GROUP, DCMI_PWD_PIN, GPIO_Mode_Out_PP);

	GPIO_SET(DCMI_RST_PORT_GROUP, DCMI_RST_PIN);
	GPIO_CLR(DCMI_PWD_PORT_GROUP, DCMI_PWD_PIN);

	SCCB_INIT();

	temp=0x80;
	sleep(100);
	while(--retry)
	{
		if(wrOV7670Reg(0x12, temp)) //Reset SCCB
			break;
	}
	if(!retry){
		p_err("not find ov7670");
		return -1;
	}
	sleep(100);
  	if(set_OV7670reg() != 0)
		p_dbg("ov7670 init faild");

	
	//OV7670_config_window(272,16,160,200);
	//OV7670_config_window(272,16,320,240);
	//OV7670_HW(50, 16, 50 + 160, 16 + 120);
	
	red_ov7673_id();
	
	p_dbg("ov7670 ok");
	return 0; //ok			  
} 
