#define DEBUG
#include "drivers.h"
#include "app.h"
#include "api.h"



#define CAPTURE_MODE_CIR		 		0	
#define CAPTURE_MODE_2CACHE 			1 

/*
*捕获模式，我们提供两种捕获列子，
*循环模式:图片通过dma自动读取，循环保存到缓冲区内存，我们可以在帧中断里面把图片读出来
*双缓冲方式:分配两块缓冲区，用于轮流保存图片，这种模式很简单，但是浪费更多内存，并且捕获到一个图片后
*需要暂停捕获以重新配置DMA
*
*/
#define CAPTURE_MODE 			CAPTURE_MODE_CIR


#if CAPTURE_MODE == CAPTURE_MODE_CIR
#define CAMERA_BUFF_SIZE               		(50*1024)	//图像缓冲区大小可根据分辨率自行调整，320240一个图片是几k字节
#else
#define CAMERA_BUFF_SIZE                	(50*1024)
#endif

#define CAMERA_IMG_QUEUE_LEN	2

struct CAMERA_CFG camera_cfg;

void img_send_thread(void *arg);
int img_send_thread_id;

static void DMA_DCMIConfiguration(uint32_t *BufferSRC, uint32_t BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC | DMA_IT_TE | DMA_IT_HT,DISABLE);
	DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_HTIF1);
	
	DMA_Cmd(DMA2_Stream1, DISABLE);
	memset(&DMA_InitStructure, 0, sizeof(DMA_InitTypeDef));

	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_Channel = DMA_Channel_1;
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(DCMI->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferSRC;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = BufferSize/4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
 	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
#if CAPTURE_MODE == CAPTURE_MODE_CIR	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
#else
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
#endif
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);

	DMA_ITConfig(DMA2_Stream1, /*DMA_IT_TC|*/DMA_IT_TE/*|DMA_IT_HT*/, ENABLE);  

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_Stream1_IRQn_Priority;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA2_Stream1, ENABLE);

}


/**
 * @brief  初始化摄像头，初始化mcu的dcmi接口，以及摄像头，但是这里还没开始捕获图像
 * @param  
 * @param  
 */
int open_camera()
{
	DCMI_InitTypeDef DCMI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

//	close_camera();

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2

	GPIO_PinAFConfig(DCMI_PCLK_PORT_GROUP, DCMI_PCLK_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_HS_PORT_GROUP, DCMI_HS_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_VS_PORT_GROUP, DCMI_VS_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D0_PORT_GROUP, DCMI_D0_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D1_PORT_GROUP, DCMI_D1_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D2_PORT_GROUP, DCMI_D2_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D3_PORT_GROUP, DCMI_D3_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D4_PORT_GROUP, DCMI_D4_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D5_PORT_GROUP, DCMI_D5_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D6_PORT_GROUP, DCMI_D6_SOURCE, GPIO_AF_DCMI); 
	GPIO_PinAFConfig(DCMI_D7_PORT_GROUP, DCMI_D7_SOURCE, GPIO_AF_DCMI); 
	
	gpio_cfg((uint32_t)DCMI_PCLK_PORT_GROUP, DCMI_PCLK_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_HS_PORT_GROUP, DCMI_HS_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_VS_PORT_GROUP, DCMI_VS_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D0_PORT_GROUP, DCMI_D0_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D1_PORT_GROUP, DCMI_D1_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D2_PORT_GROUP, DCMI_D2_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D3_PORT_GROUP, DCMI_D3_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D4_PORT_GROUP, DCMI_D4_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D5_PORT_GROUP, DCMI_D5_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D6_PORT_GROUP, DCMI_D6_PIN, GPIO_Mode_AF_IF); 
	gpio_cfg((uint32_t)DCMI_D7_PORT_GROUP, DCMI_D7_PIN, GPIO_Mode_AF_IF); 

	gpio_cfg((uint32_t)MCO_PORT_GROUP, MCO_PIN, GPIO_Mode_AF_PP);
	GPIO_PinAFConfig(MCO_PORT_GROUP, MCO_PIN_SOURCE, GPIO_AF_MCO); 
	RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1); 	//12Mhz
	/*
	* 上面配置摄像头的时钟(摄像头从mcu取时钟)，直接取晶振的时钟
	*
	*/
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
	//RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_5); //
	
	DCMI_DeInit();
	DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous; //连续模式
	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;	
	DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;	
	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_Low;
	
	DCMI_Init(&DCMI_InitStructure); 

	memset(&camera_cfg, 0, sizeof(struct CAMERA_CFG));

#if CAPTURE_MODE == CAPTURE_MODE_CIR	
	camera_cfg.dma_buff[0] = (char*)mem_malloc(CAMERA_BUFF_SIZE);
	camera_cfg.dma_index = 0;
#else
	camera_cfg.dma_buff[0] = (char*)mem_malloc(CAMERA_BUFF_SIZE);
	camera_cfg.dma_buff[1] = (char*)mem_malloc(CAMERA_BUFF_SIZE);
   	camera_cfg.dma_index = 0;
#endif
	//memset(camera_cfg.dma_buff, 0, CAMERA_BUFF_SIZE);
	
	DMA_DCMIConfiguration((uint32_t*)camera_cfg.dma_buff[camera_cfg.dma_index], CAMERA_BUFF_SIZE);
#if CAPTURE_MODE == CAPTURE_MODE_2CACHE
	camera_cfg.dma_index = !camera_cfg.dma_index;
#endif
	DCMI_ITConfig(DCMI_IT_FRAME | DCMI_IT_OVF 
		| DCMI_IT_ERR |DCMI_IT_VSYNC | DCMI_IT_LINE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DCMI_IRQn_Priority;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
 	//OV2640_HW_Init();
	//OV2640_JPEGConfig(JPEG_320x240);
	if (ov2640_Init() != 0)
        return  - 1;
	
	camera_cfg.img_event = init_event();

	img_send_thread_id = thread_create(img_send_thread,
                        0,
                        TASK_IMG_SEND_PRIO,
                        0,
                        TASK_IMG_SEND_STACK_SIZE,
                        "img_snd");
												
										
	DCMI_JPEGCmd(ENABLE); //for ov2640											

	DCMI_Cmd(ENABLE);
	
	return 0;
}

/**
 * @brief  关闭摄像头
 * @param  
 * @param  
 */
int close_camera()
{
	DCMI_Cmd(DISABLE);

	DMA_Cmd(DMA2_Stream1, DISABLE);

	thread_exit(img_send_thread_id);

#if CAPTURE_MODE == CAPTURE_MODE_CIR	
	if(camera_cfg.dma_buff[0])
		mem_free(camera_cfg.dma_buff[0]);
	camera_cfg.dma_buff[0] = 0;
#else
	if(camera_cfg.dma_buff[0])
		mem_free(camera_cfg.dma_buff[0]);
	camera_cfg.dma_buff[0] = 0;

	if(camera_cfg.dma_buff[1])
		mem_free(camera_cfg.dma_buff[1]);
	camera_cfg.dma_buff[1] = 0;
#endif	

	DCMI_DeInit();

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, DISABLE);
	
	return 0;
}

/**
 * @brief  开始捕获图像，如果是连续获取模式，图片会源源不断的发往img_send_thread线程，等待发送出去
 * @param  
 * @param  
 */
void start_capture_img()
{
	p_dbg_enter;
//	camera_cfg.pos = 0;
//	DMA_DCMIConfiguration((uint32_t*)camera_cfg.dma_buff[camera_cfg.dma_index], CAMERA_BUFF_SIZE);
	DCMI_CaptureCmd(ENABLE);
}

/**
 * @brief  停止捕获图像
 * @param  
 * @param  
 */
void stop_capture_img()
{
	p_dbg_enter;
//	camera_cfg.pos = 0;
//	DMA_DCMIConfiguration((uint32_t*)camera_cfg.dma_buff[camera_cfg.dma_index], CAMERA_BUFF_SIZE);
	DCMI_CaptureCmd(DISABLE);
}



#define RGB565_MASK_RED 0xF800 
#define RGB565_MASK_GREEN 0x07E0 
#define RGB565_MASK_BLUE 0x001F
extern int remote_socket_fd;

#if CAPTURE_MODE == CAPTURE_MODE_CIR
int handle_dcmi_rcv()
{
	__IO u32_t cur_pos, i, size, ret, cnt;
	char *tmp = 0, *p_img;
	//p_dbg_enter;
	p_img = camera_cfg.dma_buff[0];
	do
	{
	        cur_pos = CAMERA_BUFF_SIZE - DMA2_Stream1->NDTR *4; //鍙栧綋鍓嶇紦鍐插尯鏁版嵁浣嶇疆
	        //p_dbg("[%d]", cur_pos);
//		assert(cur_pos >= 0 && cur_pos < CAMERA_BUFF_SIZE);
	        //濡傛灉娌℃湁鎺ユ敹鍒板垯 cur_pos == camera_cfg.pos
	        if (cur_pos == camera_cfg.pos)
	        {
	            return 0;
	        }
	        if (cur_pos > camera_cfg.pos)
	        {
	            	size = cur_pos - camera_cfg.pos;
			//dump_hex("1", &p_img[camera_cfg.pos], 16);

			if(size > 0 && 
				size < CAMERA_BUFF_SIZE &&
				(p_img[camera_cfg.pos] == 0xff) && 
				(p_img[camera_cfg.pos + 1] == 0xd8))
			{
				tmp = (char*)mem_malloc(size + 4);
				if(tmp)
				{
					int ret;
					memcpy(tmp, (void*)&size, 4);
					memcpy(tmp + 4, &p_img[camera_cfg.pos], size);
					ret = msgsnd(camera_cfg.img_queue, tmp);
					if(ret) //faild
					{
						p_err("msgsnd err:%d", ret);
						mem_free(tmp);
					}
				}
			}else{
				p_err("invalid img");
			}
							
			camera_cfg.pos = cur_pos;
		}
		else  //缂撳啿鍣ㄧ粫鍥炵殑鎯呭喌
		{
			if (cur_pos < camera_cfg.pos)
			{
				size = CAMERA_BUFF_SIZE - camera_cfg.pos;
				//dump_hex("e", &p_img[camera_cfg.pos], 32);
				if((size + cur_pos) > 0 
					&& (size + cur_pos) < CAMERA_BUFF_SIZE &&
					(p_img[0] == 0xff) && 
					(p_img[1] == 0xd8))
				{
					tmp = (char*)mem_malloc(size + cur_pos + 4);
					if(tmp)
					{
						memcpy(tmp, (void*)&size, 4);
						memcpy(tmp + 4, &p_img[camera_cfg.pos], size);
					}
				}

				camera_cfg.pos = 0;
				//size = cur_pos;
				memset(&p_img[camera_cfg.pos], 0, 16);
				if(tmp)
				{
					memcpy(tmp + 4 + size, p_img, cur_pos);
					ret = msgsnd(camera_cfg.img_queue, tmp);
					if(ret) //faild
					{
						p_err("msgsnd err:%d", ret);
						mem_free(tmp);
					}
				}

				camera_cfg.pos = cur_pos;
			}
		}	
	}while (0);
	return 0;
}
#endif

void DCMI_IRQHandler(void)
{
	uint32_t stat = DCMI->MISR;
	enter_interrupt();
	DCMI_ClearITPendingBit(DCMI_IT_FRAME | DCMI_IT_OVF 
		| DCMI_IT_ERR |DCMI_IT_VSYNC | DCMI_IT_LINE);

	if(stat & DCMI_IT_LINE)
	{
		DCMI_ClearITPendingBit(DCMI_IT_LINE);
		stat &= ~DCMI_IT_LINE;
	}

	if(!stat)
		goto end;

	if(stat & DCMI_IT_VSYNC)
	{
		DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
	}

	if(!stat)
		goto end;

	if(stat & DCMI_IT_FRAME)
	{
#if CAPTURE_MODE == CAPTURE_MODE_2CACHE
		int img_size, cur_pos, i;
		char *p_img;
		static int last_pos = 0;

		cur_pos = CAMERA_BUFF_SIZE - DMA2_Stream1->NDTR *4;
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
		DCMI_CaptureCmd(DISABLE);
		DMA_DCMIConfiguration((uint32_t*)camera_cfg.dma_buff[camera_cfg.dma_index], CAMERA_BUFF_SIZE);
		camera_cfg.dma_index = !camera_cfg.dma_index;
		DCMI_CaptureCmd(ENABLE);
	
		p_img = camera_cfg.dma_buff[camera_cfg.dma_index];
		
		if(cur_pos > 0 && 
			cur_pos < CAMERA_BUFF_SIZE && 
			(p_img[0] == 0xff) && 
			(p_img[1] == 0xd8))
		{
			char *tmp;

			for(i = 0; i < cur_pos + 64; i++)
			{
				if(p_img[i] == 0xff && p_img[i + 1] == 0xd9)
				{
					p_dbg("end:%d", i);
					i = 0x55;  //flag
					break;
				}
			}
			if(i == 0x55)
			{
				tmp = (char*)mem_malloc(cur_pos + 4);
				if(tmp)
				{
					int ret;
					memcpy(tmp, (void*)&cur_pos, 4);
					memcpy(tmp + 4, &p_img[0], cur_pos);
					ret = msgsnd(camera_cfg.img_queue, tmp);
					if(ret) //faild
					{
						p_err("msgsnd err:%d", ret);
						mem_free(tmp);
					}
				}else
					p_err("no end");
			}
		}
#endif
#if CAPTURE_MODE == CAPTURE_MODE_CIR
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
		handle_dcmi_rcv();
#endif
	}

	if(stat & DCMI_IT_OVF)
	{
		p_err("2");
		DCMI_ClearITPendingBit(DCMI_IT_OVF);
	}

	if(stat & DCMI_IT_ERR)
	{
		p_err("3");
		DCMI_ClearITPendingBit(DCMI_IT_ERR);
	}
end:	
	exit_interrupt(1);
}


void DMA2_Stream1_IRQHandler() 
{
	enter_interrupt();

	if(DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_HTIF1))
	{
		DMA_ClearFlag(DMA2_Stream1,  DMA_FLAG_HTIF1);
		
	}

	if(DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TCIF1))
	{
		DMA_ClearFlag(DMA2_Stream1,  DMA_FLAG_TCIF1);
	}

	if(DMA_GetFlagStatus(DMA2_Stream1, DMA_FLAG_TEIF1))
	{
		p_err("dcmi dma err");
		DMA_ClearFlag(DMA2_Stream1,  DMA_FLAG_TEIF1);
	}

 	exit_interrupt(0);
}


int switch_jpeg_size()
{
    static ImageFormat_TypeDef format = JPEG_160x120;

    OV2640_JPEGConfig(format);

    format += 1;
    if (format == JPEG_FORMAT_NUM)
        format = JPEG_160x120;

    return 0;
}


#define SEND_IMG_BLOCK_SIZE 1024


/*
*发送图片到本地局域网连接，可以选择tcp或者UDP方式
*/
int send_img_local(uint8_t *buff, int size)
{
    int i, ret;
    int blocks = (size + SEND_IMG_BLOCK_SIZE - 1) / SEND_IMG_BLOCK_SIZE;
    mutex_lock(camera_cfg.img_mutex);
    for (i = 0; i < blocks; i++)
    {
       ret = send_data(remote_socket_fd, buff + SEND_IMG_BLOCK_SIZE * i, SEND_IMG_BLOCK_SIZE);	//tcp方式发送
	 //ret = udp_broadcast(4801, buff + SEND_IMG_BLOCK_SIZE * i, SEND_IMG_BLOCK_SIZE); //udp广播方式发送
        if (ret != SEND_IMG_BLOCK_SIZE){
	    mutex_unlock(camera_cfg.img_mutex);
            return  - 1;
        }
    }
    if (size > SEND_IMG_BLOCK_SIZE *blocks)
    {
        size -= SEND_IMG_BLOCK_SIZE * blocks;
        ret = send_data(remote_socket_fd, buff + SEND_IMG_BLOCK_SIZE * blocks, size);
	//ret = udp_broadcast(4801, buff + SEND_IMG_BLOCK_SIZE * blocks, size);
        if (ret != size){
	    mutex_unlock(camera_cfg.img_mutex);
            return  - 1;
        }
    }
    mutex_unlock(camera_cfg.img_mutex);
    return 0;
}


/*
*发送图片，这里判断是否有本地连接，即是否有客户端连接到了开发板
*有就直接发往本地，否则发往远程
*/
void send_img(uint8_t *p_img, int len)
{
	if(remote_socket_fd != -1){
		send_img_local(p_img, len);
	}else
  {
//		mqtt_pub(p_img, len);
  }
}

void img_send_thread(void *arg)
{
	int ret, len;
	uint8_t *p_img;
	struct mqtt_pkg_head *pkg_head;
		
	sleep(2000);//tcp杩炴帴娌￠偅涔堝揩寤虹珛锛岀瓑寰呬竴浼�
	
	msgget(&camera_cfg.img_queue, CAMERA_IMG_QUEUE_LEN);
	camera_cfg.img_mutex = mutex_init("");
	
	while(1)
	{
		ret = msgrcv(camera_cfg.img_queue, (void**)&p_img, 0);
		if(ret != 0)
		{
			p_err_fun;
			continue;
		}
		len = *((int*)p_img);
		p_dbg("[%d]", len);
		
		if(remote_socket_fd != -1){
			send_img_local(p_img + 4, len);
		}else{
			stop_capture_img(); //如果不是本地连接，获取到一个图片就关掉捕获
		}
		if(camera_cfg.remote_capture_img){

			camera_cfg.remote_capture_img = FALSE;
			pkg_head = (struct mqtt_pkg_head *)p_img;
			//p_img前面4BYTE是图片大小，这里刚好作为mqtt_pkg_head
			pkg_head->dir = MSG_DIR_DEV2APP;
			pkg_head->msg_id = MQTT_MSG_IMAGE;
			pkg_head->reserved = 0;
			send_img(p_img, len + 4);
		}
		mem_free(p_img);
	}
}


