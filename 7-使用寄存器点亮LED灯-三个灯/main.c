
/*
	使用寄存器的方法点亮LED灯
  */
#include "stm32f4xx.h" 


/**
  *   主函数
  */
int main(void)
{	
	/*开启 GPIOH 时钟，使用外设时都要先开启它的时钟*/
	RCC_AHB1ENR |= (1<<7);	
	
	/* LED 端口初始化 */
	
	/*GPIOH MODER10清空*/
	GPIOH_MODER  &= ~( 0x03<< (2*10));	
	/*PH10 MODER10 = 01b 输出模式*/
	GPIOH_MODER |= (1<<2*10);
	
	/*GPIOH OTYPER10清空*/
	GPIOH_OTYPER &= ~(1<<1*10);
	/*PH10 OTYPER10 = 0b 推挽模式*/
	GPIOH_OTYPER |= (0<<1*10);
	
	/*GPIOH OSPEEDR10清空*/
	GPIOH_OSPEEDR &= ~(0x03<<2*10);
	/*PH10 OSPEEDR10 = 0b 速率2MHz*/
	GPIOH_OSPEEDR |= (0<<2*10);
	
	/*GPIOH PUPDR10清空*/
	GPIOH_PUPDR &= ~(0x03<<2*10);
	/*PH10 PUPDR10 = 01b 上拉模式*/
	GPIOH_PUPDR |= (1<<2*10);
	
	/*PH10 BSRR寄存器的 BR10置1，使引脚输出低电平*/
	GPIOH_BSRR |= (1<<16<<10);
	
	/*PH10 BSRR寄存器的 BS10置1，使引脚输出高电平*/
	//GPIOH_BSRR |= (1<<10);

  /*GPIOH MODER11清空*/
	GPIOH_MODER  &= ~( 0x03<< (2*11));	
	/*PH11 MODER11 = 01b 输出模式*/
	GPIOH_MODER |= (1<<2*11);
	
	/*GPIOH OTYPER11清空*/
	GPIOH_OTYPER &= ~(1<<1*11);
	/*PH11 OTYPER11 = 0b 推挽模式*/
	GPIOH_OTYPER |= (0<<1*11);
	
	/*GPIOH OSPEEDR11清空*/
	GPIOH_OSPEEDR &= ~(0x03<<2*11);
	/*PH11 OSPEEDR11 = 0b 速率2MHz*/
	GPIOH_OSPEEDR |= (0<<2*11);
	
	/*GPIOH PUPDR11清空*/
	GPIOH_PUPDR &= ~(0x03<<2*11);
	/*PH11 PUPDR11 = 01b 上拉模式*/
	GPIOH_PUPDR |= (1<<2*11);
	
	/*PH11 BSRR寄存器的 BR11置1，使引脚输出低电平*/
	GPIOH_BSRR |= (1<<16<<11);
	
	
  /*GPIOH MODER12清空*/
	GPIOH_MODER  &= ~( 0x03<< (2*12));	
	/*PH12 MODER12 = 01b 输出模式*/
	GPIOH_MODER |= (1<<2*12);
	
	/*GPIOH OTYPER12清空*/
	GPIOH_OTYPER &= ~(1<<1*12);
	/*PH12 OTYPER12 = 0b 推挽模式*/
	GPIOH_OTYPER |= (0<<1*12);
	
	/*GPIOH OSPEEDR12清空*/
	GPIOH_OSPEEDR &= ~(0x03<<2*12);
	/*PH12 OSPEEDR12 = 0b 速率2MHz*/
	GPIOH_OSPEEDR |= (0<<2*12);
	
	/*GPIOH PUPDR12清空*/
	GPIOH_PUPDR &= ~(0x03<<2*12);
	/*PH12 PUPDR12 = 01b 上拉模式*/
	GPIOH_PUPDR |= (1<<2*12);
	
	/*PH12 BSRR寄存器的 BR12置1，使引脚输出低电平*/
	GPIOH_BSRR |= (1<<16<<12);
	while(1);

}

// 函数为空，目的是为了骗过编译器不报错
void SystemInit(void)
{	
}






/*********************************************END OF FILE**********************/

