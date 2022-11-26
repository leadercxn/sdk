#include "hk_fsmc.h"

static uint32_t m_fsmc_bank_norsram[] = 
{FSMC_Bank1_NORSRAM1, FSMC_Bank1_NORSRAM2, FSMC_Bank1_NORSRAM3, FSMC_Bank1_NORSRAM4};

static uint32_t m_fsmc_bus_width[] = 
{FSMC_MemoryDataWidth_8b, FSMC_MemoryDataWidth_16b};


uint16_t fsmc_conig(uint32_t sdram_no, buswidth_e buswidth)
{
    if (sdram_no > 3)
    {
        return -1;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);

	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
    
	readWriteTiming.FSMC_AddressSetupTime   = 0x07;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime    = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
    readWriteTiming.FSMC_DataSetupTime      = 26;		 // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision        = 0x00;
    readWriteTiming.FSMC_DataLatency        = 0x00;
    readWriteTiming.FSMC_AccessMode         = FSMC_AccessMode_A;	 //模式A 
    
	writeTiming.FSMC_AddressSetupTime       = 0x04;	 //地址建立时间（ADDSET）为1个HCLK  
    writeTiming.FSMC_AddressHoldTime        = 0x00;	 //地址保持时间（A		
    writeTiming.FSMC_DataSetupTime          = 8;		 ////数据保存时间为4个HCLK	
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision            = 0x00;
    writeTiming.FSMC_DataLatency            = 0x00;
    writeTiming.FSMC_AccessMode             = FSMC_AccessMode_A;	 //模式A 

    // BTCR为寄存器BCRx,BTRx, x=(NE1~NE4)
    FSMC_NORSRAMInitStructure.FSMC_Bank             = m_fsmc_bank_norsram[sdram_no];
    // BCRx[1] --> MUXEN地址/数据复用使能位
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux   = FSMC_DataAddressMux_Disable; // 不复用数据地址
    // BCRx[3:2] --> MTYP存储器类型
    // TODO: 给LCD使用仅选择SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryType       =FSMC_MemoryType_SRAM;            //SRAM   
    // BCRx[5:4] --> MWID：存储器数据总线宽度
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth  = m_fsmc_bus_width[buswidth];      //存储器数据宽度为16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode  =FSMC_BurstAccessMode_Disable;  
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity       = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait =FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode         = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation   = FSMC_WriteOperation_Enable;	//存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal       = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode     = FSMC_ExtendedMode_Enable;     //读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst       = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct    = &readWriteTiming;    //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct        = &writeTiming;            //写时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置
 	FSMC_NORSRAMCmd(m_fsmc_bank_norsram[sdram_no], ENABLE);  // 使能BANK1 
}



