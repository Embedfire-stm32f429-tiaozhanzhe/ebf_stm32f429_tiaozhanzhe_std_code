#ifndef	 __SPI_FLASH_H
#define	__SPI_FLASH_H
#include "type.h"
#include "api.h"


#define FLASH_ROOM_SIZE				(1024*1024*16)
#define FIRMWARE_BASE_ADDR			(1290*4096)

//#define ssize_t int

int  w25q128_init(void);
int w25q128_write(u32 to, size_t len,const u_char *buf);
int w25q128_read(u32 from, size_t len, u_char *buf);
int w25q128_erase(u32 addr, int len);
uint32_t w25q128_read_id(void);
int read_firmware(u32 offset, u32 len, u8 * pbuf);
int test_write_firmware_to_spi_flash(void);

#endif

