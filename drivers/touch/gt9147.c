#include "gt9147.h"

#include "trace.h"
#include "string.h"

//GT9147配置参数表
//第一个字节为版本号(0X60),必须保证新的版本号大于等于GT9147内部
//flash原有版本号,才会更新配置.
const uint8_t GT9147_CFG_TBL[]=
{ 
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
};  

uint8_t gt9147_send_cfg(touch_cfg_t *p_cfg, uint8_t mode)
{
	uint8_t buf[2];
	uint8_t i = 0;
	buf[0] = 0;
	buf[1] = mode;	//是否写入到GT9147 FLASH?  即是否掉电保存
	for (i = 0; i < sizeof(GT9147_CFG_TBL); i++)
    {
        buf[0] += GT9147_CFG_TBL[i];                  //计算校验和
    }
    buf[0] = (~buf[0]) + 1;

	gt9147_write_reg(p_cfg, GT_CFGS_REG, (uint8_t*)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL));  //发送寄存器配置
	gt9147_write_reg(p_cfg, GT_CHECK_REG, buf, 2);  //写入校验和,和配置更新标记
	return 0;
}

uint8_t gt9147_write_reg(touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	uint8_t ret = 0;

    p_cfg->p_i2c_obj.i2c_ops.xfer_start(&p_cfg->p_i2c_obj.i2c_cfg);
    p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, GT_CMD_WR);
    p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);
    p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, (reg >> 8));
    p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);
    p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, (reg & 0XFF));
    p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);


	for (i = 0; i < len; i++)
	{	   
        p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, buf[i]);
        ret = p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);
		if (ret)
            break;  
	}

    p_cfg->p_i2c_obj.i2c_ops.xfer_stop(&p_cfg->p_i2c_obj.i2c_cfg);	//产生一个停止条件	    
	return ret; 
}

void gt9147_read_reg(touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;

    p_cfg->p_i2c_obj.i2c_ops.xfer_start(&p_cfg->p_i2c_obj.i2c_cfg);
    p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, GT_CMD_WR);
    p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);

    p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, (reg >> 8));
    p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);
    p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, (reg&0xFF));
    p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);

    p_cfg->p_i2c_obj.i2c_ops.xfer_start(&p_cfg->p_i2c_obj.i2c_cfg);      
    p_cfg->p_i2c_obj.i2c_ops.send_byte(&p_cfg->p_i2c_obj.i2c_cfg, GT_CMD_RD);
    p_cfg->p_i2c_obj.i2c_ops.wait_ack(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops);

	for (i = 0; i < len; i++)
	{	   
        buf[i] = p_cfg->p_i2c_obj.i2c_ops.read_byte(&p_cfg->p_i2c_obj.i2c_cfg, &p_cfg->p_i2c_obj.i2c_ops, (i==(len-1)?0:1));
		p_cfg->p_i2c_obj.i2c_cfg.delay_us(5);
	} 
    p_cfg->p_i2c_obj.i2c_ops.xfer_stop(&p_cfg->p_i2c_obj.i2c_cfg);	//产生一个停止条件
}

uint8_t gt9147_init(touch_cfg_t *p_cfg)
{
	trace_info("start touch init.\r\n");
    uint8_t temp[5];  

    // 1. 硬件初始化
    p_cfg->p_rst_obj.gpio_ops.gpio_init(&p_cfg->p_rst_obj.gpio_cfg);
    p_cfg->p_int_obj.gpio_ops.gpio_init(&p_cfg->p_int_obj.gpio_cfg);

    p_cfg->p_i2c_obj.i2c_ops.init(&p_cfg->p_i2c_obj.i2c_cfg);

    // 1.1 复位
    p_cfg->p_rst_obj.gpio_ops.gpio_output_set(&p_cfg->p_rst_obj.gpio_cfg, 0);
    p_cfg->p_i2c_obj.i2c_ops.delay_ms(10);
    p_cfg->p_rst_obj.gpio_ops.gpio_output_set(&p_cfg->p_rst_obj.gpio_cfg, 1);
    p_cfg->p_i2c_obj.i2c_ops.delay_ms(10);


    // TODO: 若需要使能INT，需要配置INT中断使能
	// p_cfg->p_int_obj.gpio_ops.gpio_fix_input(&p_cfg->p_int_obj.gpio_cfg);

    p_cfg->p_i2c_obj.i2c_ops.delay_ms(1000);
    gt9147_read_reg(p_cfg, GT_PID_REG, temp, 4);

	temp[4] = '\0';
	trace_info("CTP ID:%s\r\n", temp);		//打印ID

	if (strcmp((char*)temp, "9147") == 0)	//ID==9147
	{
		temp[0] = 0x02;	
        gt9147_write_reg(p_cfg, GT_CTRL_REG, temp, 1);		   //软复位GT9147
        gt9147_write_reg(p_cfg, GT_CFGS_REG, temp, 1);		   //读取GT_CFGS_REG寄存器

		if (temp[0] < 0X60)                                     //默认版本比较低,需要更新flash配置
		{
			trace_info("Default Ver:%d\r\n",temp[0]);
			gt9147_send_cfg(p_cfg, 1);//更新并保存配置
		}
		p_cfg->p_i2c_obj.i2c_ops.delay_ms(10);
		temp[0] = 0X00;	 
		gt9147_write_reg(p_cfg, GT_CTRL_REG, temp, 1);	        //结束复位   	
		return 0;
	} 
	return 1;
}


//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
const uint16_t GT9147_TPX_TBL[5] = {GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
uint8_t gt9147_scan(touch_cfg_t *p_cfg, uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i = 0;
	uint8_t res = 0;
	uint8_t temp;
	uint8_t tempsta;
    static uint8_t t = 0;//控制查询间隔,从而降低CPU占用率   
    tp_dev_t tp_dev = *(tp_dev_t *)p_cfg->p_touch_cfg;

	t++;
	if (((t % 10) == 0) || (t < 10))        //空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{
        gt9147_read_reg(p_cfg, GT_GSTID_REG, &mode, 1);
 		if ((mode & 0x80) && ((mode & 0XF) < 6))
		{
			temp = 0;
            gt9147_write_reg(p_cfg, GT_GSTID_REG,&temp,1);      // 清标志
		}	
        
        if ((mode & 0XF) && ((mode & 0XF) < 6))
		{
			temp = 0XFF << (mode & 0xF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tempsta = tp_dev.sta;			    //保存当前的tp_dev.sta值
			tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES; 
			tp_dev.x[4] = tp_dev.x[0];	//保存触点0的数据
			tp_dev.y[4] = tp_dev.y[0];

			for (i = 0; i < 5; i++)
			{
				if (tp_dev.sta&(1<<i))	                                //触摸有效?
				{
                    gt9147_read_reg(p_cfg, GT9147_TPX_TBL[i],buf,4);
					if (tp_dev.touchtype & 0X01)                        //横屏
					{
						tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
						tp_dev.x[i] = 800 - (((uint16_t)buf[3] << 8) + buf[2]);
					}
                    else
					{
						tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
						tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
					}  
					//printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
				}			
			} 

			res=1;
			// TODO: if (tp_dev.x[0] > lcddev.width || tp_dev.y[0] > lcddev.height)      //非法数据(坐标超出了)
			if (1)
            { 
				if ((mode & 0XF) > 1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					tp_dev.x[0] = tp_dev.x[1];
					tp_dev.y[0] = tp_dev.y[1];
					t=0;				    //触发一次,则会最少连续监测10次,从而提高命中率
				}
                else					    //非法数据,则忽略此次数据(还原原来的)  
				{
					tp_dev.x[0] = tp_dev.x[4];
					tp_dev.y[0] = tp_dev.y[4];
					mode = 0X80;		
					tp_dev.sta = tempsta;	//恢复tp_dev.sta
				}
			}
            else 
            {
                t=0;					//触发一次,则会最少连续监测10次,从而提高命中率
            }
		}
	}
	if ((mode & 0x8F) == 0x80)          //无触摸点按下
	{ 
		if (tp_dev.sta & TP_PRES_DOWN)	    //之前是被按下的
		{
			tp_dev.sta &= ~(1<<7);	    //标记按键松开
		}
        else						    //之前就没有被按下
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	        //清除点有效标记	
		}	 
	} 	
	if (t>240)
    {
        t=10;                           //重新从10开始计数
    }

	return res;
}
