#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdint.h"

#include "stm32f4xx.h"
#include "ff.h"
#include ".\tjpgd1b\src\tjpgd.h"
#include "./Bsp/lcd/bsp_lcd.h"


/* User defined device identifier */
typedef struct {
    FIL *fp;      /* File pointer for input function */
    BYTE *fbuf;    /* Pointer to the frame buffer for output function */
    UINT wfbuf;    /* Width of the frame buffer [pix] */
} IODEV;


/* tiny jpg解码库接口，in_func和out_func接口， 由用户实现*/

/*从SD卡读取图片数据流，如果要从SRAM里读取数据流，则修改此函数*/
UINT in_func (JDEC* jd, BYTE* buff, UINT nbyte)
{
		UINT rwb;
		FRESULT res;
	
    IODEV *dev = (IODEV*)jd->device;   /* Device identifier for the session (5th argument of jd_prepare function) */

    if (buff) 
    {
      /* Read bytes from input stream */
      res = f_read(dev->fp,buff,nbyte,&rwb);
      if(res == FR_OK)   return rwb;				
    }
    /* Remove bytes from input stream */
		return (f_lseek(dev->fp, f_tell(dev->fp)+nbyte) == FR_OK) ? nbyte:0;
}



/*输出解码后的数据流*/
UINT out_func (JDEC* jd, void* bitmap, JRECT* rect)
{
    //IODEV *dev = (IODEV*)jd->device;
    BYTE *src;

    /* Put progress indicator */
    if (rect->left == 0) {
        printf("\r%lu%%,x->%d,y->%d,xsize->%d,ysize->%d", (rect->top << jd->scale) * 100UL / jd->height,rect->left,rect->top,(rect->right - rect->left)+1,(rect->bottom - rect->top)+1);			
    }
    
		src = (BYTE*)bitmap;	//解码后的图像数据
    LCD_DrawColorRect(rect->left,rect->top,(rect->right - rect->left)+1,(rect->bottom - rect->top)+1,src);    
    return 1;    /* Continue to decompress */
}

char jpgbuffer[1024*4];

/* 显示图片的例子 
 * pic_name:jpg文件的完全路径		
 */
void jpgDisplay(char *pic_name)
{
    JDEC jdec;        /* Decompression object */
    JRESULT res;      /* Result code of TJpgDec API */
    IODEV devid;      /* User defined device identifier */
 	
	
		FIL jpgFile;            // file objects
		FRESULT fres;
			
		/* Open a JPEG file */

		 fres = f_open(&jpgFile,pic_name,FA_READ|FA_OPEN_EXISTING);		 
		 if(fres == FR_OK)
		 {
			 devid.fp = &jpgFile; 
		}
		
    /* Prepare to decompress */
    res = jd_prepare(&jdec, in_func, jpgbuffer, 1024*4, &devid);
    if (res == JDR_OK) {
        /* Ready to dcompress. Image info is available here. */
        printf("Image dimensions: %u by %u. %u bytes used.\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);

        res = jd_decomp(&jdec, out_func, 0);   /* 解码并显示图片 。Start to decompress with 1/1 scaling */
       
				if (res == JDR_OK) {
	
            printf("\rOK  \n");				

        } else {
            printf("Failed to decompress: rc=%d\n", res);
        }

    } else {
        printf("Failed to prepare: rc=%d\n", res);
    }

    f_close(devid.fp);       /* Close the JPEG file */

}

