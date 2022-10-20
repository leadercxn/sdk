#include "ht1621.h"

static uint8_t Ht1621Tab[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*********************************************************
函数名: 162x_bit_set()
返回值:无
功  能:HT1621数据写入函数
Data为数据，cnt为数据位数
//发送指定的位，先发送最高位,用于发送命令与SEG
//dat的高cnt位写入1621，高位在前，如0x80的高三位是100
*********************************************************/
int ht162x_bit_send(ht162x_t *p_dev, uint8_t data, uint8_t cnt)
{
	IS_NULL(p_dev);

	uint8_t i;

	for (i = 0; i < cnt; i++)
	{
        p_dev->wr_clk_pin.gpio_ops.gpio_output_set(&p_dev->wr_clk_pin.gpio_cfg, 0);
		if ((data & 0x80) == 0) //判断最高位是否为1
		{
            p_dev->data_pin.gpio_ops.gpio_output_set(&p_dev->data_pin.gpio_cfg, 0);
		}
		else
		{
            p_dev->data_pin.gpio_ops.gpio_output_set(&p_dev->data_pin.gpio_cfg, 1);
		}

        p_dev->wr_clk_pin.gpio_ops.gpio_output_set(&p_dev->wr_clk_pin.gpio_cfg, 1);
		data <<= 1; //左移一位，丢弃已经写入的最高位
	}

	return 0;
}


/********************************************************
函数名称：void send_data_bit_162x(uchar data,uchar cnt)
功能描述: HT1621在指定地址写入数据函数
全局变量：无
参数说明：Addr为写入初始地址，Data为写入数据
返回说明：无
说 明：因为HT1621的数据位4位，所以实际写入数据为参数的后4位
//发送指定的位，先发送最低位,用于发送com
//dat的低cnt位写入1621，低位在前
//a5-a4-a3-a2-a1-a0-d0-d1-d2-d3，  a为地址：seg     d 为com
********************************************************/
int ht162x_data_bit_send(ht162x_t *p_dev, uint8_t data, uint8_t cnt)
{
	IS_NULL(p_dev);

	uint8_t i;
	for (i = 0; i < cnt; i++)
	{
        p_dev->wr_clk_pin.gpio_ops.gpio_output_set(&p_dev->wr_clk_pin.gpio_cfg, 0);
		if ((data & 0x01) == 0)
		{
            p_dev->data_pin.gpio_ops.gpio_output_set(&p_dev->data_pin.gpio_cfg, 0);
		}
		else
		{
            p_dev->data_pin.gpio_ops.gpio_output_set(&p_dev->data_pin.gpio_cfg, 1);
		}

        p_dev->wr_clk_pin.gpio_ops.gpio_output_set(&p_dev->wr_clk_pin.gpio_cfg, 1);
		data >>= 1;
	}

	return 0;
}

/********************************************************
函数名称：void ht162x_cmd_set(uchar Cmd)
功能描述: HT1621命令写入函数
全局变量：无
参数说明：Cmd为写入命令数据
返回说明：无
说 明：写入命令标识位100
********************************************************/
int ht162x_cmd_send(ht162x_t *p_dev, uint8_t cmd)
{
	IS_NULL(p_dev);

    p_dev->cs_pin.gpio_ops.gpio_output_set(&p_dev->cs_pin.gpio_cfg, 0);
	p_dev->delay_us(10);
	ht162x_bit_send(p_dev, 0x80, 3); // - - 写入命令标志100//写入标志码"100"命令模式
	ht162x_bit_send(p_dev, cmd, 9);  // - - 写入命令数据//写入9位数据，其中前8位是命令，最后一位任意。
    p_dev->cs_pin.gpio_ops.gpio_output_set(&p_dev->cs_pin.gpio_cfg, 1);
	p_dev->delay_us(10);

	return 0;
}

/********************************************************
函数名称：void Write_1621(uchar Cmd)
功能描述: HT1621命令写入函数
全局变量：无
参数说明：Cmd为写入命令数据
返回说明：无
//发送命令+地址+数据    地址SEG   com0-com4
********************************************************/
int ht162x_write(ht162x_t *p_dev, uint8_t addr, uint8_t data)
{
	IS_NULL(p_dev);

    p_dev->cs_pin.gpio_ops.gpio_output_set(&p_dev->cs_pin.gpio_cfg, 0);
	p_dev->delay_us(10);
	ht162x_bit_send(p_dev, 0xA0, 3);	   // - - 写入数据标志101
	ht162x_bit_send(p_dev, addr << 2, 6); // - - 写入地址数据//写入6位地址，SEG
	ht162x_data_bit_send(p_dev, data, 4);  //写入DAT低4位，分别对应COM3,COM2,COM1,COM0
	gpio_output_set(&p_dev->cs_pin, 1);
    p_dev->cs_pin.gpio_ops.gpio_output_set(&p_dev->cs_pin.gpio_cfg, 1);

	return 0;
}

/********************************************************
函数名称：void ht162x_all_write(uchar Addr,uchar *p,uchar cnt)
功能描述: HT1621连续写入方式函数
全局变量：无
参数说明：Addr为写入初始地址，*p为连续写入数据指针，
cnt为写入数据总数
返回说明：无
说 明：HT1621的数据位4位，此处每次数据为8位，写入数据
总数按8位计算
********************************************************/
int ht162x_all_write(ht162x_t *p_dev, uint8_t addr, uint8_t *p, uint8_t cnt)
{
	IS_NULL(p_dev);

	uint8_t i;

    p_dev->cs_pin.gpio_ops.gpio_output_set(&p_dev->cs_pin.gpio_cfg, 0);
	ht162x_bit_send(p_dev, 0xA0, 3);	   // - - 写入数据标志101
	ht162x_bit_send(p_dev, addr << 2, 6); // - - 写入地址数据
	for (i = 0; i < cnt; i++)
	{
		ht162x_data_bit_send(p_dev, *p, 8); // - - 写入数据
		p++;
	}
    p_dev->cs_pin.gpio_ops.gpio_output_set(&p_dev->cs_pin.gpio_cfg, 1);
	p_dev->delay_us(10);

	return 0;
}

/**
 * 显示全部
 */
int ht162x_all_show(ht162x_t *p_dev)
{
	IS_NULL(p_dev);

	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		ht162x_write(p_dev, i, 0xFF);
	}

	return 0;
}

//清屏
int ht162x_all_clean(ht162x_t *p_dev)
{
	IS_NULL(p_dev);

	ht162x_all_write(p_dev, 0, Ht1621Tab, 16); //清除1621寄存器数据，暨清屏

	return 0;
}

/**
 * @brief ht162x 虚拟io初始化
 */
int ht162x_init(ht162x_t *p_dev)
{
	IS_NULL(p_dev);

    p_dev->cs_pin.gpio_ops.gpio_init(&p_dev->cs_pin.gpio_cfg);
    p_dev->wr_clk_pin.gpio_ops.gpio_init(&p_dev->wr_clk_pin.gpio_cfg);
    p_dev->data_pin.gpio_ops.gpio_init(&p_dev->data_pin.gpio_cfg);

    p_dev->cs_pin.gpio_ops.gpio_output_set(&p_dev->cs_pin.gpio_cfg, 1);
    p_dev->wr_clk_pin.gpio_ops.gpio_output_set(&p_dev->wr_clk_pin.gpio_cfg, 1);
    p_dev->data_pin.gpio_ops.gpio_output_set(&p_dev->data_pin.gpio_cfg, 1);

	ht162x_cmd_send(p_dev, 0x52);
	ht162x_cmd_send(p_dev, 0X30); // - - 使用内部振荡器 0X30
	ht162x_cmd_send(p_dev, 0X00); // - - 关振系统荡器和LCD偏压发生器 0X00
	ht162x_cmd_send(p_dev, 0X0A); // - - 禁止看门狗  0X0A
	ht162x_cmd_send(p_dev, 0X02); // - - 打开系统振荡器 0X02
	ht162x_cmd_send(p_dev, 0X06); // - - 打开LCD偏压 0X06

	ht162x_cmd_send(p_dev, BIAS1_3_4COM); //设定1/3偏压，4个COM  0X29
								  //    SendCmd(RC256); // - - 使用内部振荡器
								  //    SendCmd(SYSDIS); // - - 关振系统荡器和LCD偏压发生器
								  //    SendCmd(WDTDIS); // - - 禁止看门狗
	ht162x_cmd_send(p_dev, SYSEN);		  // - - 打开系统振荡器 0X01
	ht162x_cmd_send(p_dev, LCDON);		  // - - 打开LCD偏压 LCDON

	return 0;
}
