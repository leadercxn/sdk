#ifndef __GT9147_H
#define __GT9147_H

#include "touch.h"

//IO操作函数	   
#define GT_RST    		PFout(11)	//GT9147复位引脚
#define GT_INT    		PFin(10)	//GT9147中断引脚	

//I2C读写命令	
#define GT_CMD_WR 		0X28     	//写命令
#define GT_CMD_RD 		0X29		//读命令

//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址  

#define TP_PRES_DOWN 0x80  		//触屏被按下	  
#define TP_CATH_PRES 0x40  		//有按键按下了 
#define CT_MAX_TOUCH  5    		//电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct 
{
	uint8_t (*init)(void);			//初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	uint8_t  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)

//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
} tp_dev_t;

uint8_t gt9147_send_cfg(touch_cfg_t *p_cfg, uint8_t mode);
uint8_t gt9147_write_reg(touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len);
void    gt9147_read_reg(touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len);
uint8_t gt9147_init(touch_cfg_t *p_cfg);
uint8_t gt9147_scan(touch_cfg_t *p_cfg, uint8_t mode);


#endif
