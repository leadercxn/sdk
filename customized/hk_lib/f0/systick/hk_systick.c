#include "hk_systick.h"

static uint16_t sysclk = 72;
static uint32_t fac_us = 0;
static uint32_t fac_ms = 0;

static uint32_t m_sys_tick;
static uint32_t reload;

static void systick_start(void)
{
	SysTick->CTRL	|=	SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD	=	reload; 						//每1/TICKS_PER_SEC秒中断一次	
	SysTick->CTRL	|=	SysTick_CTRL_ENABLE_Msk;   		//开启SYSTICK    
}

/*SysTick时钟源由HCLK/8提供，当系统频率为72MHz时最小计数周期为1/9MHz,
计满9次为1us，fac_us以9为单位*/
void hk_systick_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //72MHz/8 = 9MHz
	NVIC_SetPriority(SysTick_IRQn, 3);

	fac_us = sysclk / 8; //72/8 = 9,9次1uS
	fac_ms = (uint16_t)fac_us * 1000;
}

void hk_delay_us(uint32_t us)
{
	//	volatile uint32_t temp;
	SysTick->LOAD = (uint32_t)us * fac_us;
	SysTick->CTRL = 0x00; //停止计数
	SysTick->VAL = 0x00;  //清空计数器
	SysTick->CTRL = 0x01; //开始计数
						  //	temp  = SysTick->VAL;
	//当计数器的值减小到0的时候，CTRL寄存器的位16会置1
	while ((SysTick->CTRL & (0x01 << 16)) == 0);
	SysTick->CTRL = 0x00; //停止计数
	SysTick->VAL = 0x00;  //清空计数器
}

/**
 * @brief 当ms过大，导致溢出，是个BUG
 */
void hk_delay_ms(uint32_t ms)
{
	uint32_t i = 0;
	for(i = 0; i < ms; i++)
	{
		//	volatile uint32_t temp;
		SysTick->LOAD = (uint32_t)1 * fac_ms;
		SysTick->CTRL = 0x00; //停止计数
		SysTick->VAL = 0x00;  //清空计数器
		SysTick->CTRL = 0x01; //开始计数
							//	temp  = SysTick->VAL;
		//当计数器的值减小到0的时候，CTRL寄存器的位16会置1
		while ((SysTick->CTRL & (0x01 << 16)) == 0);
		SysTick->CTRL = 0x00; //停止计数
		SysTick->VAL = 0x00;  //清空计数器
	}
}


void hk_systick_init_rebuild(systick_cfg_t *p_cfg)
{
	SysTick_CLKSourceConfig(p_cfg->clk_div); //72MHz/8 = 9MHz
	NVIC_SetPriority(SysTick_IRQn, 3);

	fac_us = p_cfg->ticks_per_us; //72/8 = 9,9次1uS
	fac_ms = (uint16_t)fac_us * 1000;

	// 3. 使能嘀嗒定时器
	reload	=	SystemCoreClock / 8000000;
	reload	*=	1000000 / p_cfg->ticks_per_sec;
	systick_start();
}

void hk_delay_us_rebuild(systick_cfg_t *p_cfg, uint32_t us)
{
	hk_delay_us(us);
	// 该delay会停止嘀嗒定时器，需要重新打开嘀嗒计数器
	systick_start();
}

void hk_delay_ms_rebuild(systick_cfg_t *p_cfg, uint32_t ms)
{
	hk_delay_ms(ms);
	// 该delay会停止嘀嗒定时器，需要重新打开嘀嗒计数器
	systick_start();
}

void SysTick_Handler(void)
{
	m_sys_tick++;
}

uint32_t hk_systick_get(void)
{
	return m_sys_tick;
}
