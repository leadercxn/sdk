#include "hk_sys.h"

//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	// __ASM volatile("wfi");	
	__asm__ __volatile__("wfi");  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	// __ASM volatile("cpsid i");
	__asm__ __volatile__("cpsid i");  
}
//开启所有中断
void INTX_ENABLE(void)
{
	// __ASM volatile("cpsie i");	
	__asm__ __volatile__("cpsie i"); 	  
}
//设置栈顶地址
//addr:栈顶地址
void MSR_MSP(uint32_t addr) 
{
 	asm("MSR MSP, r0 "); 			//set Main Stack value
    asm("BX r14");
}