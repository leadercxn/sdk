#include "hk_sdio.h"
#include "hk_sys.h"
#include <string.h>
#include "trace.h"

// 用于SDIO初始化
SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;   

static uint32_t CSD_Tab[4], CID_Tab[4], RCA = 0;		// SD卡CSD,CID以及相对地址(RCA)数据

extern sdio_obj_t g_sdio_obj;

// SD_ReadDisk/SD_WriteDisk函数专用buf,当这两个函数的数据缓存区地址不是4字节对齐的时候,
// 需要用到该数组,确保数据缓存区地址是4字节对齐的.
uint8_t SDIO_DATA_BUFFER[512] __attribute__ ((aligned(4)));	

SD_Error CmdError(void);  
SD_Error CmdResp7Error(void);
SD_Error CmdResp1Error(uint8_t cmd);
SD_Error CmdResp3Error(void);
SD_Error CmdResp2Error(void);
SD_Error CmdResp6Error(uint8_t cmd,uint16_t*prca);  
SD_Error SDEnWideBus(uint8_t enx);	  
SD_Error IsCardProgramming(sdio_cfg_t *p_cfg, uint8_t *pstatus);
SD_Error FindSCR(uint16_t rca,uint32_t *pscr);
uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes); 

//检查CMD0的执行状态
//返回值:sd卡错误码
SD_Error CmdError(void)
{
	SD_Error errorstatus = SD_OK;
	uint32_t timeout = SDIO_CMD0TIMEOUT;	
    
	while (timeout--)
	{
        if (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)
        {
            break;	//命令已发送(无需响应)	 
        }
	}	    
	if (timeout == 0)
    {
        return SD_CMD_RSP_TIMEOUT; 
    } 

	SDIO_ClearFlag(SDIO_STATIC_FLAGS);		//清除所有标记
	return errorstatus;
}	

//检查R7响应的错误状态
//返回值:sd卡错误码
SD_Error CmdResp7Error(void)
{
	SD_Error errorstatus = SD_OK;
	uint32_t status;
	uint32_t timeout = SDIO_CMD0TIMEOUT;

    while (timeout--)
	{
		status = SDIO->STA;
		if (status & ((1 << 0) | (1 << 2) | (1 << 6)))    //CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
        {
            break;
        }
	}

 	if ((timeout == 0) || (status & (1 << 2)))	        //响应超时
	{																				    
		errorstatus = SD_CMD_RSP_TIMEOUT;	            //当前卡不是2.0兼容卡,或者不支持设定的电压范围
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 			//清除命令响应超时标志
		return errorstatus;
	}	
    
	if (status & (1 << 6))						//成功接收到响应
	{								   
		errorstatus = SD_OK;
		SDIO_ClearFlag(SDIO_FLAG_CMDREND); 				//清除响应标志
    }

	return errorstatus;
}

SD_Error CmdResp1Error(uint8_t cmd)
{
    uint32_t status; 

	while (1)
	{
		status = SDIO->STA;
		if (status & ((1<<0) | (1<<2) | (1<<6)))    //CRC错误/命令响应超时/已经收到响应(CRC校验成功)
        {
            break;                  
        }
	} 

	if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)    //响应超时
	{																				    
 		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 				//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	

 	if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)    //CRC错误
	{																				    
 		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL); 				//清除标志
		return SD_CMD_CRC_FAIL;
	}	
    
	if (SDIO->RESPCMD != cmd)
    {
        return SD_ILLEGAL_CMD;                              //命令不匹配 
    }

  	SDIO->ICR = 0X5FF;	 				//清除标记
	return (SD_Error)(SDIO->RESP1 & SD_OCR_ERRORBITS);//返回卡响应
}

SD_Error CmdResp3Error(void)
{
	uint32_t status;						 
    while (1)
	{
		status = SDIO->STA;
		if (status & ((1<<0)|(1<<2)|(1<<6)))        //CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
        {
            break;
        }
	}

 	if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)    //响应超时
	{											 
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			        //清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 

    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记
    return SD_OK;
}

SD_Error CmdResp2Error(void)
{
	SD_Error errorstatus = SD_OK;
	uint32_t status;
	uint32_t timeout = SDIO_CMD0TIMEOUT;
    while (timeout--)
	{
		status = SDIO->STA;
		if (status & ((1<<0) | (1<<2) | (1<<6)))        //CRC错误/命令响应超时/已经收到响应(CRC校验成功)
        {
            break;
        }	
	}

  	if ((timeout == 0) || (status & (1 << 2)))	        //响应超时
	{																				    
		errorstatus = SD_CMD_RSP_TIMEOUT; 
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 		    //清除命令响应超时标志
		return errorstatus;
	}	
    
	if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)    //CRC错误
	{								   
		errorstatus = SD_CMD_CRC_FAIL;
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);		        //清除响应标志
    }
    
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记
    return errorstatus;								    		 
}

SD_Error CmdResp6Error(uint8_t cmd,uint16_t*prca)
{
	SD_Error errorstatus=SD_OK;
	uint32_t status;					    
	uint32_t rspr1;
    while (1)
	{
		status = SDIO->STA;
		if (status & ((1<<0) | (1<<2) | (1<<6)))  //CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
        {
            break;
        }
	}

	if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)    //响应超时
	{																				    
 		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			        //清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 	 

	if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)    //CRC错误
	{								   
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);					//清除响应标志
        return SD_CMD_CRC_FAIL;
	}

	if (SDIO->RESPCMD != cmd)                               //判断是否响应cmd命令
	{
        return SD_ILLEGAL_CMD; 		
	}	    
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);                      //清除所有标记
	
	rspr1 = SDIO->RESP1;					                //得到响应 	 
	if (SD_ALLZERO == 
        (rspr1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
	{
		*prca = (uint16_t)(rspr1 >> 16);			//右移16位得到,rca
		return errorstatus;
	}

    if (rspr1 & SD_R6_GENERAL_UNKNOWN_ERROR)
    {
        return SD_GENERAL_UNKNOWN_ERROR;
    }

    if (rspr1 & SD_R6_ILLEGAL_CMD)
    {
        return SD_ILLEGAL_CMD;
    }

    if (rspr1 & SD_R6_COM_CRC_FAILED)
    {
        return SD_COM_CRC_FAILED;
    }
	return errorstatus;
}

//SDIO使能宽总线模式
//enx:0,不使能;1,使能;
//返回值:错误状态
SD_Error SDEnWideBus(uint8_t enx)
{
	SD_Error errorstatus = SD_OK;
	uint32_t scr[2] = {0, 0};
	uint8_t arg = 0X00;

	if (enx)
	{
		arg = 0X02;
	}
	else 
	{
		arg = 0X00;
	}

	if (SDIO->RESP1 & SD_CARD_LOCKED)
	{
		return SD_LOCK_UNLOCK_FAILED;					//SD卡处于LOCKED状态
	}

	errorstatus = FindSCR(g_sdio_obj.sdio_cfg.cardinfo->rca, scr);		//得到SCR寄存器数据
	if (errorstatus != SD_OK)
	{
		return errorstatus;
	}

	if ((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)		//支持宽总线
	{
		//发送CMD55+RCA,短响应	
		SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)g_sdio_obj.sdio_cfg.cardinfo->rca << 16;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);

		errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

		if (errorstatus != SD_OK)
		{
			return errorstatus; 
		}

		//发送ACMD6,短响应,参数:10,4位;00,1位.
		SDIO_CmdInitStructure.SDIO_Argument = arg;			
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);

		errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}
	else 
	{
		return SD_REQUEST_NOT_APPLICABLE;				//不支持宽总线设置 	 
	}
}

SD_Error FindSCR(uint16_t rca, uint32_t *pscr)
{
	uint32_t index = 0; 
	SD_Error errorstatus = SD_OK;
	uint32_t tempscr[2] = {0, 0};  
	
	//发送CMD16,短响应,设置Block Size为8字节
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)8;	 	
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN; 	// cmd16
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  // r1
	SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);

	errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

	if (errorstatus != SD_OK)
	{
		return errorstatus;
	}   

	//发送CMD55,短响应 
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) rca << 16; 
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;					
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);
	
	errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
	
	if (errorstatus != SD_OK)
	{
		return errorstatus;
	}

	SDIO_DataInitStructure.SDIO_DataTimeOut 	= SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DataLength 		= 8;  					//8个字节长度,block为8字节,SD卡到SDIO.
	SDIO_DataInitStructure.SDIO_DataBlockSize 	= SDIO_DataBlockSize_8b;  //块大小8byte 
	SDIO_DataInitStructure.SDIO_TransferDir 	= SDIO_TransferDir_ToSDIO;
	SDIO_DataInitStructure.SDIO_TransferMode 	= SDIO_TransferMode_Block;
	SDIO_DataInitStructure.SDIO_DPSM 			= SDIO_DPSM_Enable;
	SDIO_DataConfig(&SDIO_DataInitStructure);		

	//发送ACMD51,短响应,参数为0	
	SDIO_CmdInitStructure.SDIO_Argument = 0x0;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;	
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  	//r1
	SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);
	
	errorstatus = CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);
	
	if (errorstatus != SD_OK)
	{
		return errorstatus;	
	}		
	
	while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL 
			| SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
	{ 
		if (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	//接收FIFO数据可用
		{
			*(tempscr + index) = SDIO_ReadData();				//读取FIFO内容
			index++;
			if (index >= 2)
			{
				break;
			}
		}
	}

	if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//数据超时错误
	{										   
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 					//清错误标志
		return SD_DATA_TIMEOUT;
	}
	else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//数据块CRC错误
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  					//清错误标志
		return SD_DATA_CRC_FAIL;		   
	}
	else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//接收fifo上溢错误
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);						//清错误标志
		return SD_RX_OVERRUN;		 
	}
	else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//接收起始位错误
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);						//清错误标志
		return SD_START_BIT_ERR;		 
	}  
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记

	//把数据顺序按8位为单位倒过来.   	
	*(pscr+1) = ((tempscr[0]&SD_0TO7BITS)<<24) | ((tempscr[0]&SD_8TO15BITS)<<8) 
				| ((tempscr[0]&SD_16TO23BITS)>>8) | ((tempscr[0]&SD_24TO31BITS)>>24);
	*(pscr) = ((tempscr[1]&SD_0TO7BITS)<<24) | ((tempscr[1]&SD_8TO15BITS)<<8) 
				| ((tempscr[1]&SD_16TO23BITS)>>8) | ((tempscr[1]&SD_24TO31BITS)>>24);
	return errorstatus;	
}

//检查卡是否正在执行写操作
//pstatus:当前状态.
//返回值:错误代码
SD_Error IsCardProgramming(sdio_cfg_t *p_cfg, uint8_t *pstatus)
{
	uint32_t respR1 = 0, status = 0; 


	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) p_cfg->cardinfo->rca << 16; 	//卡相对地址参数
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;		//发送CMD13 	
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	

	status = SDIO->STA;
	
	//等待操作完成
	while (!(status & ((1<<0)|(1<<6)|(1<<2))))
	{
		status = SDIO->STA;					
	}
	
 	if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)			//CRC检测失败
	{  
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);	
		return SD_CMD_CRC_FAIL;
	}

	if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)			//命令超时 
	{
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);		
		return SD_CMD_RSP_TIMEOUT;
	}

	if (SDIO->RESPCMD != SD_CMD_SEND_STATUS)
	{
		return SD_ILLEGAL_CMD;
	}
	
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	
	respR1 = SDIO->RESP1;
	
	*pstatus= (uint8_t)((respR1>>9)&0x0000000F);
	return SD_OK;
}

//得到NumberOfBytes以2为底的指数.
//NumberOfBytes:字节数.
//返回值:以2为底的指数值
uint8_t convert_from_bytes_to_power_of_two(uint16_t bytenum)
{
	uint8_t count = 0;

	while (bytenum != 1)
	{
		bytenum >>= 1;
		count++;
	}

	return count;
} 

//卡上电
//查询所有SDIO接口上的卡设备,并查询其电压和配置时钟
//返回值:错误代码;(0,无错误)
SD_Error hk_sd_pwr_on(sdio_cfg_t *p_cfg)
{
    uint8_t i = 0;
	SD_Error errorstatus = SD_OK;
	uint32_t response = 0, count = 0, validvoltage = 0;
	uint32_t SDType = SD_STD_CAPACITY;

	/*初始化时的时钟不能大于400KHz*/ 
    /* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
    SDIO_InitStructure.SDIO_ClockDiv        = SDIO_INIT_CLK_DIV;	
    SDIO_InitStructure.SDIO_ClockEdge       = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass     = SDIO_ClockBypass_Disable;  	//不使用bypass模式，直接用HCLK进行分频得到SDIO_CK
    SDIO_InitStructure.SDIO_ClockPowerSave  = SDIO_ClockPowerSave_Disable;	// 空闲时不关闭时钟电源
    SDIO_InitStructure.SDIO_BusWide         = SDIO_BusWide_1b;	 				//1位数据线
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;//硬件流控关闭
    SDIO_Init(&SDIO_InitStructure);
    
    SDIO_SetPowerState(SDIO_PowerState_ON);	//上电状态,开启卡时钟
    
    SDIO_ClockCmd(ENABLE);//SDIOCK使能 

    for (i = 0; i < 74; i++)
	{
		//	发送CMD0进入IDLE STAGE模式命令.
		SDIO_CmdInitStructure.SDIO_Argument = 0x0;          
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE; //cmd0
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;     //无响应
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;     //则CPSM在开始发送命令之前等待数据传输结束。 
        SDIO_SendCommand(&SDIO_CmdInitStructure);	  		        //写命令进命令寄存器
		
		errorstatus = CmdError();
		if (errorstatus == SD_OK)
        {
            break;
        }
    }

    if (errorstatus)
    {
        return errorstatus;         //返回错误状态
    }

	//发送CMD8,短响应,检查SD卡接口特性
	SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;	        
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;	    //cmd8
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;	    //r7
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;			        //关闭等待中断
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
	
  	errorstatus = CmdResp7Error();						//等待R7响应
    
 	if(errorstatus == SD_OK) 							//R7响应正常
	{
		p_cfg->cardinfo->cardtype = SDIO_STD_CAPACITY_SD_CARD_V2_0;	//SD 2.0卡
		SDType   = SD_HIGH_CAPACITY;			   		//高容量卡
	}

	//发送CMD55,短响应	
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;         
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);		    //发送CMD55,短响应	 
	
	errorstatus = CmdResp1Error(SD_CMD_APP_CMD); 		 	//等待R1响应
    
	if (errorstatus == SD_OK)                              //SD2.0/SD 1.1,否则为MMC卡
	{																  
		//SD卡,发送ACMD41 SD_APP_OP_COND,参数为:0x80100000 
		while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
		{	   			
			//发送CMD55,短响应							   
            SDIO_CmdInitStructure.SDIO_Argument = 0x00;                 
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;	    
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);			       
			
			errorstatus = CmdResp1Error(SD_CMD_APP_CMD); 	 	            //等待R1响应   
            if (errorstatus != SD_OK)
            {
                return errorstatus;   	            //响应错误
            }
			
            //acmd41，命令参数由支持的电压范围及HCS位组成，HCS位置一来区分卡是SDSc还是sdhc
			//发送ACMD41,短响应	
            SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;              //r3
            SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);
			
			errorstatus = CmdResp3Error(); 					//等待R3响应
			
            if (errorstatus != SD_OK)
            {
                return errorstatus;   	//响应错误  
            }

			response = SDIO->RESP1;			   				//得到响应
			validvoltage = (((response>>31)==1)?1:0);			//判断SD卡上电是否完成
			count++;
		}

		if (count >= SD_MAX_VOLT_TRIAL)
		{
			errorstatus = SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 
		if (response &= SD_HIGH_CAPACITY)
		{
			p_cfg->cardinfo->cardtype = SDIO_HIGH_CAPACITY_SD_CARD;	
		}
    }
    else        //MMC卡
	{
		//MMC卡,发送CMD1 SDIO_SEND_OP_COND,参数为:0x80FF8000 
		while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
		{	   		
			//发送CMD1,短响应								   				   
			SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_MMC;	   
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_OP_COND;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r3
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);
			
			errorstatus = CmdResp3Error(); 					//等待R3响应   
            if (errorstatus != SD_OK)
            {
                return errorstatus;   	//响应错误  
            }

			response = SDIO->RESP1;				   				//得到响应
			validvoltage = (((response >> 31) ==1 ) ? 1 : 0);
			count++;
		}

		if (count >= SD_MAX_VOLT_TRIAL)
		{
			errorstatus = SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 			    
		p_cfg->cardinfo->cardtype = SDIO_MULTIMEDIA_CARD;
    }
    
    return errorstatus;		
}


//SD卡 Power OFF
//返回值:错误代码;(0,无错误)
SD_Error hk_sd_pwr_off(void)
{
	SDIO_SetPowerState(SDIO_PowerState_OFF);//SDIO电源关闭,时钟停止	
	return SD_OK;		  
} 

//初始化所有的卡,并让卡进入就绪状态
//返回值:错误代码
SD_Error hk_sd_initial_cards(sdio_cfg_t *p_cfg)
{
    SD_Error errorstatus = SD_OK;
    uint16_t rca = 0x01;
    if (SDIO_GetPowerState() == 0)
    {
        return SD_REQUEST_NOT_APPLICABLE;               //检查电源状态,确保为上电状态
    }

 	if (SDIO_SECURE_DIGITAL_IO_CARD != p_cfg->cardinfo->cardtype)        //非SECURE_DIGITAL_IO_CARD
	{
		//发送CMD2,取得CID,长响应
		SDIO_CmdInitStructure.SDIO_Argument = 0x0;      
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);       
		
		errorstatus = CmdResp2Error(); 					//等待R2响应   
		if (errorstatus != SD_OK)
        {
            return errorstatus;   	                    //响应错误	
        }
        
        CID_Tab[0] = SDIO->RESP1;
		CID_Tab[1] = SDIO->RESP2;
		CID_Tab[2] = SDIO->RESP3;
		CID_Tab[3] = SDIO->RESP4;
	}

    //判断卡类型
	if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == p_cfg->cardinfo->cardtype) ||
        (SDIO_STD_CAPACITY_SD_CARD_V2_0 == p_cfg->cardinfo->cardtype) ||
        (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == p_cfg->cardinfo->cardtype) ||
        (SDIO_HIGH_CAPACITY_SD_CARD == p_cfg->cardinfo->cardtype))
	{
		//发送CMD3,短响应 
		SDIO_CmdInitStructure.SDIO_Argument = 0x00;               
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r6
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);	               
		
		errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR, &rca);        //等待R6响应 
		if (errorstatus != SD_OK)                                    //响应错误
        {
            return errorstatus;   	           
        }		    
	}   

    if (SDIO_MULTIMEDIA_CARD == p_cfg->cardinfo->cardtype)
    {
		//发送CMD3,短响应 
        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca<<16);       
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r6
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);	            	
			
		errorstatus = CmdResp2Error(); 					            //等待R2响应   
		if (errorstatus != SD_OK)                                   //响应错误	 
        {
            return errorstatus;   
        }	            
    }

	if (SDIO_SECURE_DIGITAL_IO_CARD != p_cfg->cardinfo->cardtype)			            //非SECURE_DIGITAL_IO_CARD
	{
		 //发送CMD9+卡RCA,取得CSD,长响应 
		g_sdio_obj.sdio_cfg.cardinfo->rca = rca;
        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);   
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);
		
		errorstatus = CmdResp2Error(); 					//等待R2响应   
		if (errorstatus != SD_OK)                       //响应错误	
        {
            return errorstatus;   		
        }    

        CSD_Tab[0]=SDIO->RESP1;
		CSD_Tab[1]=SDIO->RESP2;
		CSD_Tab[2]=SDIO->RESP3;						
		CSD_Tab[3]=SDIO->RESP4;		
	}
	return SD_OK;//卡初始化成功
} 


SD_Error hk_sd_get_card_info(sdio_cfg_t *p_cfg, sd_card_info_t *cardinfo)
{
    SD_Error errorstatus = SD_OK;
	uint8_t tmp=0;	   

	cardinfo->cardtype	= (uint8_t)p_cfg->cardinfo->cardtype; 		// 卡类型
	cardinfo->rca 		= (uint16_t)p_cfg->cardinfo->rca;			// 卡RCA值

	tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
	cardinfo->sd_csd.CSDStruct = (tmp & 0xC0) >> 6;			//CSD结构
	cardinfo->sd_csd.SysSpecVersion = (tmp & 0x3C) >> 2;	//2.0协议还没定义这部分(为保留),应该是后续协议定义的
	cardinfo->sd_csd.Reserved1 = tmp & 0x03;				//2个保留位  

	tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);		//第1个字节
	cardinfo->sd_csd.TAAC = tmp;				   			//数据读时间1

	tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);		//第2个字节
	cardinfo->sd_csd.NSAC = tmp;		  					//数据读时间2

	tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);				//第3个字节
	cardinfo->sd_csd.MaxBusClkFrec = tmp;		  			//传输速度	 
	
	tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);		//第4个字节
	cardinfo->sd_csd.CardComdClasses = tmp<<4;    			//卡指令类高四位

	tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);		//第5个字节
	cardinfo->sd_csd.CardComdClasses |= (tmp & 0xF0) >> 4;	//卡指令类低四位
	cardinfo->sd_csd.RdBlockLen = tmp & 0x0F;	    		//最大读取数据长度

	tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);			//第6个字节
	cardinfo->sd_csd.PartBlockRead      = (tmp & 0x80) >> 7;	//允许分块读
	cardinfo->sd_csd.WrBlockMisalign    = (tmp & 0x40) >> 6;	//写块错位
	cardinfo->sd_csd.RdBlockMisalign    = (tmp & 0x20) >> 5;	//读块错位
	cardinfo->sd_csd.DSRImpl            = (tmp & 0x10) >> 4;
	cardinfo->sd_csd.Reserved2          = 0; 					//保留

    //标准1.1/2.0卡/MMC卡
    if ((p_cfg->cardinfo->cardtype == SDIO_STD_CAPACITY_SD_CARD_V1_1)
        || (p_cfg->cardinfo->cardtype == SDIO_STD_CAPACITY_SD_CARD_V2_0)
        || (p_cfg->cardinfo->cardtype == SDIO_MULTIMEDIA_CARD))
	{
		cardinfo->sd_csd.DeviceSize = (tmp & 0x03) << 10;	//C_SIZE(12位)

	 	tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF); 			//第7个字节	
		cardinfo->sd_csd.DeviceSize |= (tmp) << 2;

 		tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);	//第8个字节	
		cardinfo->sd_csd.DeviceSize |= (tmp & 0xC0) >> 6;
        cardinfo->sd_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
		cardinfo->sd_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

 		tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);		//第9个字节	
		cardinfo->sd_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
		cardinfo->sd_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
		cardinfo->sd_csd.DeviceSizeMul = (tmp & 0x03) << 1;		//C_SIZE_MULT

 		tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);	  	//第10个字节	
		cardinfo->sd_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
        cardinfo->card_capacity = (cardinfo->sd_csd.DeviceSize + 1);	//计算卡容量
		cardinfo->card_capacity *= (1 << (cardinfo->sd_csd.DeviceSizeMul + 2));
		cardinfo->card_blocksize = 1 << (cardinfo->sd_csd.RdBlockLen);	//块大小
		cardinfo->card_capacity *= cardinfo->card_blocksize;
	}
    else if (p_cfg->cardinfo->cardtype == SDIO_HIGH_CAPACITY_SD_CARD)	//高容量卡
	{
 		tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF); 			//第7个字节	
		cardinfo->sd_csd.DeviceSize = (tmp & 0x3F) << 16;	//C_SIZE

 		tmp =(uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24); 	//第8个字节	
        cardinfo->sd_csd.DeviceSize |= (tmp << 8);

        tmp =(uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);	//第9个字节	
        cardinfo->sd_csd.DeviceSize |= (tmp);

        tmp =(uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8); 	//第10个字节	
        cardinfo->card_capacity  = (long long)(cardinfo->sd_csd.DeviceSize+1) * 512 * 1024;//计算卡容量
		cardinfo->card_blocksize = 512; 			//块大小固定为512字节
	}	  

	cardinfo->sd_csd.EraseGrSize = (tmp & 0x40) >> 6;
	cardinfo->sd_csd.EraseGrMul = (tmp & 0x3F) << 1;	
	
	tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF);			//第11个字节	
	cardinfo->sd_csd.EraseGrMul |= (tmp & 0x80) >> 7;
	cardinfo->sd_csd.WrProtectGrSize = (tmp & 0x7F);

 	tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);	//第12个字节	
	cardinfo->sd_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
	cardinfo->sd_csd.ManDeflECC = (tmp & 0x60) >> 5;
	cardinfo->sd_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
	cardinfo->sd_csd.MaxWrBlockLen = (tmp & 0x03) << 2;	 

	tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);		//第13个字节
	cardinfo->sd_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
	cardinfo->sd_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
	cardinfo->sd_csd.Reserved3 = 0;
	cardinfo->sd_csd.ContentProtectAppli = (tmp & 0x01);  

	tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);		//第14个字节
	cardinfo->sd_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
	cardinfo->sd_csd.CopyFlag = (tmp & 0x40) >> 6;
	cardinfo->sd_csd.PermWrProtect = (tmp & 0x20) >> 5;
	cardinfo->sd_csd.TempWrProtect = (tmp & 0x10) >> 4;
	cardinfo->sd_csd.FileFormat = (tmp & 0x0C) >> 2;
	cardinfo->sd_csd.ECC = (tmp & 0x03);  

	tmp = (uint8_t)(CSD_Tab[3] & 0x000000FF);			//第15个字节
	cardinfo->sd_csd.CSD_CRC = (tmp & 0xFE)>>1;
	cardinfo->sd_csd.Reserved4 = 1;		 

	tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >>24 );	//第0个字节
	cardinfo->sd_cid.ManufacturerID = tmp;		
	
	tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);	//第1个字节
	cardinfo->sd_cid.OEM_AppliID = tmp << 8;	  
	tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);	//第2个字节
	cardinfo->sd_cid.OEM_AppliID |= tmp;	   
	
	tmp = (uint8_t)(CID_Tab[0] & 0x000000FF);			//第3个字节	
	cardinfo->sd_cid.ProdName1 = tmp << 24;	
	tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24); 	//第4个字节
	cardinfo->sd_cid.ProdName1 |= tmp << 16;	  
	tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);	//第5个字节
	cardinfo->sd_cid.ProdName1 |= tmp << 8;		 
	tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);	//第6个字节
	cardinfo->sd_cid.ProdName1 |= tmp;		   
	tmp = (uint8_t)(CID_Tab[1] & 0x000000FF);	  		//第7个字节
	cardinfo->sd_cid.ProdName2 = tmp;			 
	
	tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24); 	//第8个字节
	cardinfo->sd_cid.ProdRev = tmp;		 

	tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);	//第9个字节
	cardinfo->sd_cid.ProdSN = tmp << 24;	   
	tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8); 	//第10个字节
	cardinfo->sd_cid.ProdSN |= tmp << 16;	   
	tmp = (uint8_t)(CID_Tab[2] & 0x000000FF);   		//第11个字节
	cardinfo->sd_cid.ProdSN |= tmp << 8;		   
	tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24); 	//第12个字节
	cardinfo->sd_cid.ProdSN |= tmp;			
	
	tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);	//第13个字节
	cardinfo->sd_cid.Reserved1 |= (tmp & 0xF0) >> 4;
	cardinfo->sd_cid.ManufactDate = (tmp & 0x0F) << 8;    
	tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);	//第14个字节
	cardinfo->sd_cid.ManufactDate |= tmp;
	
	tmp = (uint8_t)(CID_Tab[3] & 0x000000FF);			//第15个字节
	cardinfo->sd_cid.CID_CRC = (tmp & 0xFE) >> 1;
	cardinfo->sd_cid.Reserved2 = 1;	 
	return errorstatus;
}


SD_Error hk_sd_select_deselect(uint32_t addr)
{
	//发送CMD7,选择卡,短响应	
	SDIO_CmdInitStructure.SDIO_Argument =	addr;						
	SDIO_CmdInitStructure.SDIO_CmdIndex =	SD_CMD_SEL_DESEL_CARD;
	SDIO_CmdInitStructure.SDIO_Response =	SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait 	=	SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM 	=	SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);							
	
	return CmdResp1Error(SD_CMD_SEL_DESEL_CARD);
}

SD_Error hk_sd_enable_widebus_operation(sdio_cfg_t *p_cfg, uint32_t wmode)
{
	SD_Error errorstatus = SD_OK;
	
	if (SDIO_MULTIMEDIA_CARD == p_cfg->cardinfo->cardtype)
	{
		return SD_UNSUPPORTED_FEATURE;				//MMC卡不支持
	}
	else if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == p_cfg->cardinfo->cardtype)
			|| (SDIO_STD_CAPACITY_SD_CARD_V2_0 == p_cfg->cardinfo->cardtype)
			|| (SDIO_HIGH_CAPACITY_SD_CARD == p_cfg->cardinfo->cardtype))
	{
		if (wmode >= 2)
		{
			return SD_UNSUPPORTED_FEATURE;			//不支持8位模式
		}
		else   
		{
			errorstatus = SDEnWideBus(wmode);
			if (SD_OK == errorstatus)
			{
				SDIO->CLKCR &= ~(3<<11);				//清除之前的位宽设置    
				SDIO->CLKCR |= (uint16_t)wmode << 11;	//1位/4位总线宽度 
				SDIO->CLKCR |= 0 << 14;					//不开启硬件流控制
			}
		}  
	}
	return errorstatus; 
}

void hk_sdio_clk_set(uint8_t clkdiv)
{
	uint32_t tmpreg = SDIO->CLKCR; 
	tmpreg &= 0XFFFFFF00; 
	tmpreg |= clkdiv;   
	SDIO->CLKCR = tmpreg;
}

SD_Error hk_sd_set_device_mode(sdio_cfg_t *p_cfg, uint32_t mode)
{
	SD_Error errorstatus = SD_OK;
	if ((mode == SD_DMA_MODE) || (mode == SD_POLLING_MODE))
	{
		p_cfg->devicemode = mode;
	}
	else 
	{
		errorstatus = SD_INVALID_PARAMETER;
	}

	return errorstatus;	   
}

//读取当前卡状态
//pcardstatus:卡状态
//返回值:错误代码
SD_Error hk_sd_send_status(uint32_t *pcardstatus)
{
	SD_Error errorstatus = SD_OK;
	if (pcardstatus == NULL)
	{
		errorstatus = SD_INVALID_PARAMETER;
		return errorstatus;
	}

	//发送CMD13,短响应	
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;	 
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	

	errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);	//查询响应状态 
	if (errorstatus != SD_OK)
	{
		return errorstatus;
	}
	*pcardstatus = SDIO->RESP1;//读取响应值
	return errorstatus;
} 

//返回SD卡的状态
//返回值:SD卡状态
SDCardState hk_sd_get_status(void)
{
	uint32_t resp1=0;
	if (hk_sd_send_status(&resp1) != SD_OK)
	{
		return SD_CARD_ERROR;
	}
	else 
	{
		return (SDCardState)((resp1>>9) & 0x0F);
	}
}

SD_Error hk_sd_init(sdio_cfg_t *p_cfg)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	uint8_t clkdiv=0;
	SD_Error errorstatus = SD_OK;	 

	hk_sdio_hw_cfg_t *p_hw_cfg = (hk_sdio_hw_cfg_t *)p_cfg->p_hw_cfg;

	//SDIO IO口初始化
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO|RCC_AHBPeriph_DMA2,ENABLE);//使能SDIO,DMA2时钟	 

    p_hw_cfg->sd_gpio1.gpio_ops.gpio_init(&p_hw_cfg->sd_gpio1.gpio_cfg);
    p_hw_cfg->sd_gpio2.gpio_ops.gpio_init(&p_hw_cfg->sd_gpio2.gpio_cfg);

 	//SDIO外设寄存器设置为默认值 			   	   
	SDIO_DeInit();

	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;			//SDIO中断配置
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 	= 0;					//子优先级0 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    errorstatus = hk_sd_pwr_on(p_cfg);			//SD卡上电

    if (errorstatus == SD_OK)
    {
		errorstatus = hk_sd_initial_cards(p_cfg);			//初始化SD卡	
    }
	
    if (errorstatus == SD_OK)
    {
        errorstatus = hk_sd_get_card_info(p_cfg, p_cfg->cardinfo);	//获取卡信息
    }
	
    if (errorstatus == SD_OK)
    {
		errorstatus = hk_sd_select_deselect((uint32_t)((p_cfg->cardinfo->rca)<<16));//选中SD卡
    }

    if (errorstatus == SD_OK)
    {
        errorstatus = hk_sd_enable_widebus_operation(p_cfg, 1);	//4位宽度,如果是MMC卡,则不能用4位模式 
    }

    if ((errorstatus==SD_OK) || (SDIO_MULTIMEDIA_CARD== p_cfg->cardinfo->cardtype))
	{  		    
        //V1.1/V2.0卡，设置最高72/12=6Mhz
		if (p_cfg->cardinfo->cardtype == SDIO_STD_CAPACITY_SD_CARD_V1_1
            || p_cfg->cardinfo->cardtype == SDIO_STD_CAPACITY_SD_CARD_V2_0)
		{
			clkdiv = SDIO_TRANSFER_CLK_DIV+6;	    
		}
        else
        {
            clkdiv = SDIO_TRANSFER_CLK_DIV;	    //SDHC等其他卡，设置最高72/6=12Mhz
        } 

        //设置时钟频率,SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz 
		hk_sdio_clk_set(clkdiv);				
		//errorstatus = hk_sd_set_device_mode(p_cfg, SD_DMA_MODE);	//设置为DMA模式

		errorstatus = hk_sd_set_device_mode(p_cfg, SD_POLLING_MODE);	//设置为查询模式
    }

	return errorstatus;	
}

//SDIO中断处理函数
//处理SDIO传输过程中的各种中断事务
//返回值:错误代码
SD_Error hk_sd_process_IRQSrc(void)
{
	if (SDIO->STA & (1 << 8))//接收完成中断
	{	 
		if (g_sdio_obj.sdio_cfg.stopcondition == 1)
		{
			//发送CMD12+结束传输 	
			SDIO_CmdInitStructure.SDIO_Argument = 0;		  
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);	
			
			g_sdio_obj.sdio_cfg.transfererror = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
		}
		else
		{
			g_sdio_obj.sdio_cfg.transfererror = SD_OK;
		} 	

		SDIO->ICR |= 1<<8;		//清除完成中断标记
		SDIO->MASK &= ~((1<<1) | (1<<3) | (1<<8) | (1<<14) | (1<<15) | (1<<4) | (1<<5) | (1<<9));//关闭相关中断
		g_sdio_obj.sdio_cfg.transferend = 1;
		return (g_sdio_obj.sdio_cfg.transfererror);
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//数据CRC错误
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//清错误标志
		SDIO->MASK &= ~((1<<1) | (1<<3) | (1<<8) | (1<<14) | (1<<15) | (1<<4) | (1<<5) | (1<<9));//关闭相关中断
		g_sdio_obj.sdio_cfg.transfererror = SD_DATA_CRC_FAIL;
		return SD_DATA_CRC_FAIL;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)//数据超时错误
	{
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);  			//清中断标志
		SDIO->MASK &= ~((1<<1) | (1<<3) | (1<<8) | (1<<14) | (1<<15) | (1<<4) | (1<<5) | (1<<9));//关闭相关中断
		g_sdio_obj.sdio_cfg.transfererror = SD_DATA_TIMEOUT;
		return SD_DATA_TIMEOUT;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)//FIFO上溢错误
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);  			//清中断标志
		SDIO->MASK &= ~((1<<1) | (1<<3) | (1<<8) | (1<<14) | (1<<15) | (1<<4) | (1<<5) | (1<<9));//关闭相关中断
		g_sdio_obj.sdio_cfg.transfererror = SD_RX_OVERRUN;
		return SD_RX_OVERRUN;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)//FIFO下溢错误
	{
		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);  			//清中断标志
		SDIO->MASK &= ~((1<<1) | (1<<3) | (1<<8) | (1<<14) | (1<<15) | (1<<4) | (1<<5) | (1<<9));//关闭相关中断
		g_sdio_obj.sdio_cfg.transfererror = SD_TX_UNDERRUN;
		return SD_TX_UNDERRUN;
	}
	if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)//起始位错误
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);  		//清中断标志
		SDIO->MASK &= ~((1<<1) | (1<<3) | (1<<8) | (1<<14) | (1<<15) | (1<<4) | (1<<5) | (1<<9));//关闭相关中断
		g_sdio_obj.sdio_cfg.transfererror = SD_START_BIT_ERR;
		return SD_START_BIT_ERR;
	}
	return SD_OK;
}

//SDIO中断服务函数		  
void SDIO_IRQHandler(void) 
{											
	hk_sd_process_IRQSrc();			//处理所有SDIO相关中断
}	

//配置SDIO DMA  
//mbuf:存储器地址
//bufsize:传输数据量
//DMA_DIR:方向; @ref DMA_data_transfer_direction      
//				DMA_DIR_PeripheralDST ,存储器-->SDIO(写数据);
// 				DMA_DIR_PeripheralSRC,SDIO-->存储器(读数据);
void hk_sd_dma_cfg(uint32_t*mbuf, uint32_t bufsize, uint32_t dir)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//使能DMA2时钟

	DMA_DeInit(DMA2_Channel4);   		//将DMA2的通道4寄存器重设为缺省值
	DMA_Cmd(DMA2_Channel4, DISABLE ); 	//关闭DMA2 通道4

	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)&SDIO->FIFO;  	//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr 		= (uint32_t)mbuf;  			//DMA内存基地址
	DMA_InitStructure.DMA_DIR 					= dir;  				//数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize 			= bufsize / 4;  			//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;			//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Word;  //数据宽度为32位
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Word; 		//数据宽度为32位
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;  			//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_High; 			//DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M 					= DMA_M2M_Disable;  			//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA2_Channel4, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器

	DMA_Cmd(DMA2_Channel4, DISABLE ); //开启DMA2 通道4
}


//SD卡读取一个块 
//buf:读数据缓存区(必须4字节对齐!!)
//addr:读取地址
//blksize:块大小
SD_Error hk_sd_read_block(sdio_cfg_t *p_cfg, uint8_t *buf, long long addr, uint16_t blksize)
{
	SD_Error errorstatus = SD_OK;
	uint8_t power;
   	uint32_t count = 0, *tempbuff = (uint32_t *)buf;		//转换为uint32_t指针 
	uint32_t timeout = SDIO_DATATIMEOUT;   

	if (NULL == buf)
	{
		return SD_INVALID_PARAMETER; 
	}

   	SDIO->DCTRL = 0x0;									//数据控制寄存器清零(关DMA)   
	if (p_cfg->cardinfo->cardtype == SDIO_HIGH_CAPACITY_SD_CARD)			//大容量卡
	{
		blksize = 512;
		addr >>= 9;
	}   

	SDIO_DataInitStructure.SDIO_DataBlockSize	= SDIO_DataBlockSize_1b;		//清除DPSM状态机配置
	SDIO_DataInitStructure.SDIO_DataLength		= 0;
	SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if (SDIO->RESP1 & SD_CARD_LOCKED)
	{
		return SD_LOCK_UNLOCK_FAILED;		//卡锁了
	}
	
	if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize-1)) == 0))
	{
		power = convert_from_bytes_to_power_of_two(blksize);	    

		//发送CMD16+设置数据长度为blksize,短响应
		SDIO_CmdInitStructure.SDIO_Argument = blksize;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);
		
		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);			//等待R1响应  
		
		if (errorstatus != SD_OK)
		{
			return errorstatus;   	//响应错误	
		} 
	}
	else
	{
		return SD_INVALID_PARAMETER;
	} 				
	
	//清除DPSM状态机配置
	SDIO_DataInitStructure.SDIO_DataBlockSize	= power<<4 ;
	SDIO_DataInitStructure.SDIO_DataLength		= blksize ;
	SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToSDIO;
	SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	//发送CMD17+从addr地址出读取数据,短响应 
	SDIO_CmdInitStructure.SDIO_Argument = addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
	
	errorstatus = CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);	//等待R1响应   
	if (errorstatus != SD_OK)
	{
		return errorstatus;   			//响应错误	
	}
	
	if (p_cfg->devicemode == SD_POLLING_MODE)
	{
		// INTX_DISABLE();						//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		__disable_irq();

		//无上溢/CRC/超时/完成(标志)/起始位错误
		while (!(SDIO->STA & ((1<<5) | (1<<1) | (1<<3) | (1<<10) | (1<<9))))
		{
			if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)		//接收区半满,表示至少存了8个字
			{
				for (count = 0; count < 8; count++)			//循环读取数据
				{
					*(tempbuff + count) = SDIO->FIFO;
				}
				tempbuff += 8;	 
				timeout = 0X7FFFFF; 	//读数据溢出时间
			}
			else 	//处理超时
			{
				if (timeout == 0)
				{
					return SD_DATA_TIMEOUT;
				}
				timeout--;
			}
		} 
		if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//数据超时错误
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 		//清错误标志
			return SD_DATA_TIMEOUT;
		}
		else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//数据块CRC错误
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}
		else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//接收fifo上溢错误
		{
	 		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);			//清错误标志
			return SD_RX_OVERRUN;		 
		}
		else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//接收起始位错误
		{
			SDIO_ClearFlag(SDIO_FLAG_STBITERR);			//清错误标志
			return SD_START_BIT_ERR;		 
		}  
		
		while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	//FIFO里面,还存在可用数据
		{
			*tempbuff = SDIO_ReadData();	//循环读取数据
			tempbuff++;
		}

		// INTX_ENABLE();//开启总中断
		__enable_irq();
		SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记
	}
	else if (p_cfg->devicemode == SD_DMA_MODE)
	{
		hk_sd_dma_cfg((uint32_t*)buf, blksize, DMA_DIR_PeripheralSRC); 
		g_sdio_obj.sdio_cfg.transfererror	= SD_OK;
		g_sdio_obj.sdio_cfg.stopcondition	= 0;				//单块读,不需要发送停止传输指令
		g_sdio_obj.sdio_cfg.transferend	= 0;				//传输结束标置位，在中断服务置1
		SDIO->MASK |= (1<<1) | (1<<3) | (1<<8) | (1<<5) | (1<<9);	//配置需要的中断 
		SDIO_DMACmd(ENABLE);

		//等待传输完成 
		while (((DMA2->ISR & 0X2000) == RESET) 
				&& (g_sdio_obj.sdio_cfg.transferend == 0) 
				&& (g_sdio_obj.sdio_cfg.transfererror == SD_OK) 
				&& timeout)
		{
			timeout--;
		}

		if (timeout == 0)
		{
			return SD_DATA_TIMEOUT;
		}

		if (g_sdio_obj.sdio_cfg.transfererror != SD_OK)
		{
			errorstatus = g_sdio_obj.sdio_cfg.transfererror;  
		}
    }   

	return errorstatus; 
}

uint32_t *tempbuff __attribute__ ((aligned(4)));
SD_Error hk_sd_read_multi_blocks(sdio_cfg_t *p_cfg, uint8_t *buf, long long addr, uint16_t blksize, uint32_t nblks)
{
	SD_Error errorstatus = SD_OK;
	uint8_t power;
	uint32_t count = 0;
	uint32_t timeout = SDIO_DATATIMEOUT;  

	tempbuff=(uint32_t*)buf;					//转换为uint32_t指针
	SDIO->DCTRL=0x0;							//数据控制寄存器清零(关DMA)  
	
	if (p_cfg->cardinfo->cardtype == SDIO_HIGH_CAPACITY_SD_CARD)	//大容量卡
	{
		blksize = 512;
		addr >>= 9;
	}  

	//清除DPSM状态机配置
	SDIO_DataInitStructure.SDIO_DataBlockSize	= 0;
	SDIO_DataInitStructure.SDIO_DataLength		= 0;
	SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if (SDIO->RESP1 & SD_CARD_LOCKED)
	{
		return SD_LOCK_UNLOCK_FAILED;		//卡锁了
	}

	if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize-1)) == 0))
	{
		power = convert_from_bytes_to_power_of_two(blksize);	

		//发送CMD16+设置数据长度为blksize,短响应 
		SDIO_CmdInitStructure.SDIO_Argument = blksize;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);

		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if (errorstatus != SD_OK)							//响应错误
		{
			return errorstatus;   	
		}	 
	}
	else 
	{
		return SD_INVALID_PARAMETER;	
	}
	
	if (nblks > 1)											//多块读  
	{									    
		if (nblks * blksize > SD_MAX_DATA_LENGTH)			//判断是否超过最大接收长度
		{
			return SD_INVALID_PARAMETER;
		}

		//nblks*blksize,512块大小,卡到控制器
		SDIO_DataInitStructure.SDIO_DataBlockSize	= power<<4;
		SDIO_DataInitStructure.SDIO_DataLength		= nblks*blksize;
		SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT;
		SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
		SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToSDIO;
		SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
		SDIO_DataConfig(&SDIO_DataInitStructure);

		//发送CMD18+从addr地址出读取数据,短响应 
		SDIO_CmdInitStructure.SDIO_Argument =  addr;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus = CmdResp1Error(SD_CMD_READ_MULT_BLOCK);	//等待R1响应   
		if (errorstatus != SD_OK)								//响应错误
		{
			return errorstatus;   
		}
		
		if (p_cfg->devicemode == SD_POLLING_MODE)
		{
			// INTX_DISABLE();				//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			__disable_irq();

			//无上溢/CRC/超时/完成(标志)/起始位错误
			while (!(SDIO->STA & ((1<<5) | (1<<1) | (1<<3) | (1<<8) | (1<<9))))
			{
				if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)		//接收区半满,表示至少存了8个字			
				{
					for (count = 0; count < 8; count++)			//循环读取数据
					{
						*(tempbuff+count) = SDIO->FIFO;
					}

					tempbuff += 8;	 
					timeout = 0X7FFFFF; 	//读数据溢出时间
				}
				else 	//处理超时
				{
					if (timeout == 0)
					{
						return SD_DATA_TIMEOUT;
					}
					timeout--;
				}
			}
			
			if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//数据超时错误
			{										   
				SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//清错误标志
				return SD_DATA_TIMEOUT;
			}
			else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//数据块CRC错误
			{
				SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}
			else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//接收fifo上溢错误
			{
				SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		//清错误标志
				return SD_RX_OVERRUN;		 
			}
			else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//接收起始位错误
			{
				SDIO_ClearFlag(SDIO_FLAG_STBITERR);//清错误标志
				return SD_START_BIT_ERR;		 
			}   

			while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	//FIFO里面,还存在可用数据
			{
				*tempbuff = SDIO_ReadData();	//循环读取数据
				tempbuff++;
			}

	 		if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		//接收结束
			{
				if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == p_cfg->cardinfo->cardtype)
					|| (SDIO_STD_CAPACITY_SD_CARD_V2_0 == p_cfg->cardinfo->cardtype)
					|| (SDIO_HIGH_CAPACITY_SD_CARD == p_cfg->cardinfo->cardtype))
				{
					//发送CMD12+结束传输
					SDIO_CmdInitStructure.SDIO_Argument = 0;
					SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);	
					
					errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);	//等待R1响应   
					if (errorstatus != SD_OK)
					{
						return errorstatus;	 
					}
				}
			}
			// INTX_ENABLE();						//开启总中断
			__enable_irq();
			SDIO_ClearFlag(SDIO_STATIC_FLAGS);	//清除所有标记
		}
		else if (p_cfg->devicemode == SD_DMA_MODE)
		{
			hk_sd_dma_cfg((uint32_t*)buf, nblks * blksize, DMA_DIR_PeripheralSRC); 
			g_sdio_obj.sdio_cfg.transfererror 	= SD_OK;
			g_sdio_obj.sdio_cfg.stopcondition 	= 1;				//多块读,需要发送停止传输指令 
			g_sdio_obj.sdio_cfg.transferend	= 0;				//传输结束标置位，在中断服务置1
			SDIO->MASK	|= (1<<1) | (1<<3) | (1<<8) | (1<<5) | (1<<9);	//配置需要的中断 
		 	SDIO->DCTRL	|= 1<<3;		 								//SDIO DMA使能 
			while (((DMA2->ISR & 0X2000) == RESET) && timeout)			//等待传输完成 
			{
				timeout--;
			}
			if (timeout == 0)												//超时
			{
				return SD_DATA_TIMEOUT;
			}

			while ((g_sdio_obj.sdio_cfg.transferend == 0) 
					&& (g_sdio_obj.sdio_cfg.transfererror == SD_OK)); 

			if (g_sdio_obj.sdio_cfg.transfererror != SD_OK)
			{
				errorstatus = g_sdio_obj.sdio_cfg.transfererror;  	 
			}
		}		 
	}
	return errorstatus;

}  

SD_Error hk_sd_write_block(sdio_cfg_t *p_cfg, uint8_t *buf, long long addr, uint16_t blksize)
{
	SD_Error errorstatus = SD_OK;
	uint8_t  power = 0, cardstate = 0;
	uint32_t timeout = 0, bytestransferred = 0;
	uint32_t cardstatus = 0, count = 0, restwords = 0;
	uint32_t tlen = blksize;						//总长度(字节)
	uint32_t *tempbuff = (uint32_t*)buf;	

	if (buf == NULL)
	{
		return SD_INVALID_PARAMETER;//参数错误  
	} 

	//数据控制寄存器清零(关DMA)   
	SDIO->DCTRL=0x0;							

	//清除DPSM状态机配置
	SDIO_DataInitStructure.SDIO_DataBlockSize	= 0;
	SDIO_DataInitStructure.SDIO_DataLength		= 0;
	SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if (SDIO->RESP1 & SD_CARD_LOCKED)
	{
		return SD_LOCK_UNLOCK_FAILED;			//卡锁了
	}

 	if (p_cfg->cardinfo->cardtype == SDIO_HIGH_CAPACITY_SD_CARD)	//大容量卡
	{
		blksize = 512;
		addr >>= 9;
	}    

	if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize-1)) == 0))
	{
		power = convert_from_bytes_to_power_of_two(blksize);	

		//发送CMD16+设置数据长度为blksize,短响应 	
		SDIO_CmdInitStructure.SDIO_Argument = blksize;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		//等待R1响应 
		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);	
		
		if (errorstatus != SD_OK)
		{
			return errorstatus;   	//响应错误	 
		}
		
	}
	else 
	{
		return SD_INVALID_PARAMETER;	
	} 

	//发送CMD13,查询卡的状态,短响应 
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)p_cfg->cardinfo->rca << 16;	
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	

	//等待R1响应 
	// TODO: no card status here
	errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);		
	
	if (errorstatus != SD_OK)
	{
		return errorstatus;
	}

	cardstatus = SDIO->RESP1;													  
	timeout = SD_DATATIMEOUT;
   	while (((cardstatus & 0x00000100) == 0) && (timeout > 0)) 	//检查READY_FOR_DATA位是否置位
	{
		timeout--;

		//发送CMD13,查询卡的状态,短响应
		SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)p_cfg->cardinfo->rca << 16;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);	//等待R1响应 

		if (errorstatus != SD_OK)
		{
			return errorstatus;	
		}			    
		cardstatus = SDIO->RESP1;													  
	}
	if (timeout == 0)
	{
		return SD_ERROR;
	}
	
	//发送CMD24,写单块指令,短响应 	
	SDIO_CmdInitStructure.SDIO_Argument = addr;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	
	
	errorstatus = CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);	//等待R1响应   		   
	if (errorstatus != SD_OK)
	{
		return errorstatus; 
	}  	  
	g_sdio_obj.sdio_cfg.stopcondition = 0;						//单块写,不需要发送停止传输指令 

	SDIO_DataInitStructure.SDIO_DataBlockSize	= power << 4;	//blksize, 控制器到卡	
	SDIO_DataInitStructure.SDIO_DataLength		= blksize;
	SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStructure);
	
	timeout = SDIO_DATATIMEOUT;
	if (p_cfg->devicemode == SD_POLLING_MODE)
	{
		// INTX_DISABLE();				//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		__disable_irq();

		// 数据块发送成功/下溢/CRC/超时/起始位错误
		while (!(SDIO->STA & ((1<<10) | (1<<4) | (1<<1) | (1<<3) | (1<<9))))
		{
			if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)		//发送区半空,表示至少存了8个字
			{
				if ((tlen-bytestransferred) < SD_HALFFIFOBYTES)			//不够32字节了
				{
					restwords = ((tlen - bytestransferred) % 4 == 0)?
								((tlen - bytestransferred) / 4) : ((tlen - bytestransferred) / 4 + 1);
					
					for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
					{
						SDIO_WriteData(*tempbuff);
					}
				}
				else
				{
					for (count = 0; count < 8; count++)
					{
						SDIO_WriteData(*(tempbuff + count));
					}
					tempbuff += 8;
					bytestransferred += 32;
				}
				timeout = 0X3FFFFFFF;	//写数据溢出时间
			}
			else
			{
				if (timeout == 0)
				{
					return SD_DATA_TIMEOUT;
				}
				timeout--;
			}
		}
		
		if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//数据超时错误
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//清错误标志
			return SD_DATA_TIMEOUT;
		}
		else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//数据块CRC错误
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  	//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}
		else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	//接收fifo下溢错误
		{
	 		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		//清错误标志
			return SD_TX_UNDERRUN;		 
		}
		else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//接收起始位错误
		{
			SDIO_ClearFlag(SDIO_FLAG_STBITERR);		//清错误标志
			return SD_START_BIT_ERR;		 
		}   
		
		// INTX_ENABLE();				//开启总中断
		__enable_irq();
		SDIO->ICR = 0X5FF;	 		//清除所有标记	  
	}
	else if (p_cfg->devicemode == SD_DMA_MODE)
	{
		hk_sd_dma_cfg((uint32_t*)buf, blksize, DMA_DIR_PeripheralDST);		//SDIO DMA配置
		g_sdio_obj.sdio_cfg.transfererror 	= SD_OK;
		g_sdio_obj.sdio_cfg.stopcondition 	= 0;					//单块写,不需要发送停止传输指令 
		g_sdio_obj.sdio_cfg.transferend	= 0;					//传输结束标置位，在中断服务置1
		SDIO->MASK |= (1<<1) | (1<<3) | (1<<8) | (1<<4) | (1<<9);	//配置产生数据接收完成中断
		SDIO->DCTRL|=1<<3;								//SDIO DMA使能.  

		while (((DMA2->ISR & 0X2000) == RESET) && timeout)
		{
			timeout--;						//等待传输完成 
		}

		if (timeout == 0)
		{
  			hk_sd_init(p_cfg);	 					//重新初始化SD卡,可以解决写入死机的问题
			return SD_DATA_TIMEOUT;			//超时	 
		
		}

		timeout = SDIO_DATATIMEOUT;
		while ((g_sdio_obj.sdio_cfg.transferend == 0) 
				&& (g_sdio_obj.sdio_cfg.transfererror == SD_OK) 
				&& timeout)
		{
			timeout--;
		}

		if (timeout == 0)
		{
			return SD_DATA_TIMEOUT;			//超时	 
		}

		if (g_sdio_obj.sdio_cfg.transfererror !=SD_OK)
		{
			return g_sdio_obj.sdio_cfg.transfererror;
		}
	}  
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);					//清除所有标记

	errorstatus = IsCardProgramming(p_cfg, &cardstate);
	while ((errorstatus==SD_OK) && ((cardstate == SD_CARD_PROGRAMMING)
			|| (cardstate== SD_CARD_RECEIVING)))
	{
		errorstatus = IsCardProgramming(p_cfg, &cardstate);
	}   

	return errorstatus;
}

//SD卡写多个块 
//buf:数据缓存区
//addr:写地址
//blksize:块大小
//nblks:要写入的块数
//返回值:错误状态	
SD_Error hk_sd_write_multi_blocks(sdio_cfg_t *p_cfg, uint8_t *buf, long long addr, uint16_t blksize, uint32_t nblks)
{
	SD_Error errorstatus = SD_OK;
	uint8_t  power = 0, cardstate = 0;
	uint32_t timeout = 0, bytestransferred = 0;
	uint32_t count = 0, restwords = 0;
	uint32_t tlen = nblks*blksize;				//总长度(字节)
	uint32_t *tempbuff = (uint32_t*)buf;  

	if(buf==NULL)
	{
		return SD_INVALID_PARAMETER; //参数错误  
	}

	SDIO->DCTRL=0x0;							//数据控制寄存器清零(关DMA)   
	
	// 清除DPSM状态机配置
	SDIO_DataInitStructure.SDIO_DataBlockSize	= 0;		
	SDIO_DataInitStructure.SDIO_DataLength		= 0;
	SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT;
	SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if (SDIO->RESP1 & SD_CARD_LOCKED)		//卡锁了
	{
		return SD_LOCK_UNLOCK_FAILED;
	}

	if (p_cfg->cardinfo->cardtype == SDIO_HIGH_CAPACITY_SD_CARD)		//大容量卡
	{
		blksize = 512;
		addr >>= 9;
	}    

	if ((blksize  >0) && (blksize <= 2048) && ((blksize & (blksize-1)) == 0))
	{
		power = convert_from_bytes_to_power_of_two(blksize);
		
		// 发送CMD16+设置数据长度为blksize,短响应
		SDIO_CmdInitStructure.SDIO_Argument = blksize;	
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应  
		
		if (errorstatus != SD_OK)		//响应错误
		{
			return errorstatus;   	
		}	
	}
	else 
	{
		return SD_INVALID_PARAMETER;	
	}
	
	if (nblks > 1)
	{					  
		if (nblks * blksize > SD_MAX_DATA_LENGTH)
		{
			return SD_INVALID_PARAMETER;   
		}

		if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == p_cfg->cardinfo->cardtype)
			|| (SDIO_STD_CAPACITY_SD_CARD_V2_0 == p_cfg->cardinfo->cardtype)
			|| (SDIO_HIGH_CAPACITY_SD_CARD == p_cfg->cardinfo->cardtype))
		{
			//提高性能，发送ACMD55,短响应
			SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)p_cfg->cardinfo->rca<<16;		
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);	
			
			errorstatus = CmdResp1Error(SD_CMD_APP_CMD);		//等待R1响应 
			
			if (errorstatus != SD_OK)
			{
				return errorstatus;	
			}			 
			
			//发送CMD23,设置块数量,短响应 
			SDIO_CmdInitStructure.SDIO_Argument = nblks;			 
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);
			
			errorstatus = CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);	//等待R1响应 
				
			if (errorstatus != SD_OK)
			{
				return errorstatus;
			}	
		} 

		//发送CMD25,多块写指令,短响应 
		SDIO_CmdInitStructure.SDIO_Argument = addr;		  
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	

 		errorstatus = CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);	//等待R1响应   		   
	
		if (errorstatus != SD_OK)
		{
			return errorstatus;
		}

		//blksize, 控制器到卡	
        SDIO_DataInitStructure.SDIO_DataBlockSize	= power << 4;	
		SDIO_DataInitStructure.SDIO_DataLength		= nblks * blksize;
		SDIO_DataInitStructure.SDIO_DataTimeOut		= SD_DATATIMEOUT;
		SDIO_DataInitStructure.SDIO_DPSM			= SDIO_DPSM_Enable;
		SDIO_DataInitStructure.SDIO_TransferDir		= SDIO_TransferDir_ToCard;
		SDIO_DataInitStructure.SDIO_TransferMode	= SDIO_TransferMode_Block;
		SDIO_DataConfig(&SDIO_DataInitStructure);
		
		if (p_cfg->devicemode == SD_POLLING_MODE)
		{
			timeout = SDIO_DATATIMEOUT;
			// INTX_DISABLE();							//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			__disable_irq();
			
			//下溢/CRC/数据结束/超时/起始位错误
			while (!(SDIO->STA & ((1<<4) | (1<<1) | (1<<8) | (1<<3) | (1<<9))))
			{
				//发送区半空,表示至少存了8字(32字节)
				if (SDIO->STA & (1<<14))							
				{	  
					if ((tlen - bytestransferred) < SD_HALFFIFOBYTES)		//不够32字节了
					{
						restwords = ((tlen - bytestransferred) % 4 == 0) 
									? ((tlen - bytestransferred) / 4):((tlen - bytestransferred) / 4 + 1);
						
						for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
						{
							SDIO_WriteData(*tempbuff);
						}
					}
					else 		//发送区半空,可以发送至少8字(32字节)数据
					{
						for (count = 0; count < SD_HALFFIFO; count++)
						{
							SDIO_WriteData(*(tempbuff+count));
						}
						tempbuff += SD_HALFFIFO;
						bytestransferred += SD_HALFFIFOBYTES;
					}
					timeout=0X3FFFFFFF;				//写数据溢出时间
				}
				else
				{
					if (timeout == 0)
					{
						return SD_DATA_TIMEOUT; 
					}
					timeout--;
				}
			} 

			if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//数据超时错误
			{										   
				SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//清错误标志
				return SD_DATA_TIMEOUT;
			}
			else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//数据块CRC错误
			{
				SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}
			else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	//接收fifo下溢错误
			{
				SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		//清错误标志
				return SD_TX_UNDERRUN;		 
			}
			else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//接收起始位错误
			{
				SDIO_ClearFlag(SDIO_FLAG_STBITERR);//清错误标志
				return SD_START_BIT_ERR;		 
			}  

			if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		//发送结束
			{															 
				if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == p_cfg->cardinfo->cardtype) 
					|| (SDIO_STD_CAPACITY_SD_CARD_V2_0 == p_cfg->cardinfo->cardtype)
					|| (SDIO_HIGH_CAPACITY_SD_CARD == p_cfg->cardinfo->cardtype))
				{   
					//发送CMD12+结束传输 	  
					SDIO_CmdInitStructure.SDIO_Argument = 0;
					SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait 	= SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM 	= SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);	
					
					errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);		//等待R1响应   
					if (errorstatus != SD_OK)
					{
						return errorstatus;	
					} 
				}
			}

			// INTX_ENABLE();						//开启总中断
			__enable_irq();
			SDIO_ClearFlag(SDIO_STATIC_FLAGS);	//清除所有标记
		}
		else if (p_cfg->devicemode == SD_DMA_MODE)
		{
			hk_sd_dma_cfg((uint32_t*)buf, nblks*blksize, DMA_DIR_PeripheralDST);	//SDIO DMA配置
			g_sdio_obj.sdio_cfg.transfererror 	= SD_OK;
			g_sdio_obj.sdio_cfg.stopcondition 	= 1;					//多块写,需要发送停止传输指令 
			g_sdio_obj.sdio_cfg.transferend	= 0;					//传输结束标置位，在中断服务置1
			SDIO->MASK |= (1<<1) | (1<<3) | (1<<8) | (1<<4) | (1<<9);	//配置产生数据接收完成中断
	 	 	SDIO->DCTRL |= 1<<3;										//SDIO DMA使能. 
			timeout = SDIO_DATATIMEOUT;

			while (((DMA2->ISR & 0X2000) == RESET) && timeout)
			{
				timeout--;			//等待传输完成 
			}

			if (timeout == 0)	 								//超时
			{									  
  				hk_sd_init(p_cfg);	 					//重新初始化SD卡,可以解决写入死机的问题
	 			return SD_DATA_TIMEOUT;			//超时	 
			}

			timeout = SDIO_DATATIMEOUT;
			while ((g_sdio_obj.sdio_cfg.transferend == 0) 
					&& (g_sdio_obj.sdio_cfg.transfererror == SD_OK) 
					&& timeout)
			{
				timeout--;
			}

			if (timeout == 0)
			{
				return SD_DATA_TIMEOUT;			//超时	
			} 

			if (g_sdio_obj.sdio_cfg.transfererror != SD_OK)
			{
				return g_sdio_obj.sdio_cfg.transfererror;
			}	 
		}
	}

	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除所有标记

	errorstatus = IsCardProgramming(p_cfg, &cardstate);
	while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING)
			|| (cardstate == SD_CARD_RECEIVING)))
	{
		errorstatus = IsCardProgramming(p_cfg, &cardstate);
	}   
	return errorstatus;	   
}


//读SD卡
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;				  				 
uint8_t hk_sd_read_disk(sdio_cfg_t *p_cfg, uint8_t *buf, uint32_t sector, uint8_t cnt)
{
	uint8_t sta = SD_OK;
	long long lsector = sector;
	uint8_t n;

	lsector <<= 9;
	if ((uint32_t)buf % 4 != 0)
	{
		for (n = 0; n < cnt; n++)
		{
			sta = hk_sd_read_block(p_cfg, SDIO_DATA_BUFFER, lsector + 512 * n, 512);	//单个sector的读操作
			memcpy(buf, SDIO_DATA_BUFFER, 512);
			buf += 512;
		} 
	}
	else
	{
		if (cnt == 1)
		{
			sta = hk_sd_read_block(p_cfg, buf, lsector, 512);    	//单个sector的读操作
		}
		else 
		{
			sta = hk_sd_read_multi_blocks(p_cfg, buf, lsector, 512, cnt);	//多个sector  
		}
	}

	return sta;
}

//写SD卡
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;	
uint8_t hk_sd_write_disk(sdio_cfg_t *p_cfg, uint8_t*buf, uint32_t sector, uint8_t cnt)
{
	uint8_t sta = SD_OK;
	uint8_t n;
	long long lsector = sector;

	lsector <<= 9;
	if ((uint32_t)buf % 4 != 0)
	{
		for (n = 0; n < cnt; n++)
		{
			memcpy(SDIO_DATA_BUFFER, buf, 512);
			sta = hk_sd_write_block(p_cfg, SDIO_DATA_BUFFER, lsector + 512 * n, 512);	//单个sector的写操作
			buf += 512;
		} 
	}
	else
	{
		if (cnt == 1)
		{
			sta = hk_sd_write_block(p_cfg, buf, lsector, 512);    		//单个sector的写操作
		}
		else 
		{
			sta = hk_sd_write_multi_blocks(p_cfg, buf, lsector, 512, cnt);	//多个sector  
		}
	}
	return sta;
}

void hk_sdio_send_cmd(uint8_t cmdindex, uint8_t waitrsp, uint32_t arg);
void hk_sdio_send_data_cfg(uint32_t datatimeout, uint32_t datalen, uint8_t blksize, uint8_t dir);

void hk_sdio_show_cardinfo(sdio_cfg_t *p_cfg)
{
	switch (p_cfg->cardinfo->cardtype)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:
			trace_info("Card Type:SDSC V1.1\r\n");
			break;

		case SDIO_STD_CAPACITY_SD_CARD_V2_0:
			trace_info("Card Type:SDSC V2.0\r\n");
			break;

		case SDIO_HIGH_CAPACITY_SD_CARD:
			trace_info("Card Type:SDHC V2.0\r\n");
			break;

		case SDIO_MULTIMEDIA_CARD:
			trace_info("Card Type:MMC Card\r\n");
			break;
	}

	trace_info("card manufacturer ID: %d\r\n", p_cfg->cardinfo->sd_cid.ManufacturerID);
	trace_info("card RCA: %d\r\n", p_cfg->cardinfo->rca);
	trace_info("card capacity: %d\r\n", (uint32_t)(p_cfg->cardinfo->card_capacity >> 20));
	trace_info("card block size: %d\r\n\r\n", p_cfg->cardinfo->card_blocksize);
}


