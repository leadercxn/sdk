#ifndef _HT1621_H_
#define _HT1621_H_

#include "stdbool.h"
#include "gpio.h"

//HT1621控制命令
#define SYSDIS      0X00    //0B 100	0000	0000	X  关振荡器和LCD偏压发生器
#define SYSEN       0X01    //0B 100	0000	0001	X  打开系统振荡器
#define LCDOFF      0X02    //0B 100	0000	0010	X  关LCD偏压
#define LCDON       0X03    //0B 100	0000	0011	X  开LCD偏压
#define TIMERDIS    0X04    //0B 100	0000	0100	X  时基输出失效
#define WDTDIS      0X05    //0B 100	0000	0101	X  WDT溢出标志输出失效
#define TIMEREN     0X06    //0B 100	0000	0110	X  时基输出使能
#define WDTEN       0X07    //0B 100	0000	0111	X  WDT溢出标志输出有效
#define TONEOFF     0X08    //0B 100	0000	1000	X  关闭声音输出
#define TONEON      0X09    //0B 100	0000	1001	X  打开声音输出
#define CLRTIMER    0X0C    //0B 100	0000	1100	X  时基发生器清零
#define CLRWDT      0X0E    //0B 100	0000	1110	X  清除WDT状态
#define XTAL_32K    0X14    //0B 100	0001	0100	X  系统时钟源，晶振
#define RC256       0X18    //0B 100	0001	1000	X  系统时钟源，片内RC
#define EXT256      0X1C    //0B 100	0001	1100	X  系统时钟源，外部时钟源

#define BIAS1_2_2COM 0X20   //0B 100	0010	0000	X 	LCD 1/2偏压选项，2个公共口
#define BIAS1_2_3COM 0X24   //0B 100	0010	0100	X 	LCD 1/2偏压选项，3个公共口
#define BIAS1_2_4COM 0X28   //0B 100	0010	1000	X 	LCD 1/2偏压选项，4个公共口

#define BIAS1_3_2COM 0X21   //0B 100	0010	0001	X 	LCD 1/3偏压选项，2个公共口
#define BIAS1_3_3COM 0X25   //0B 100	0010	0101	X 	LCD 1/3偏压选项，3个公共口
#define BIAS1_3_4COM 0X29   //0B 100	0010	1001	X 	LCD 1/3偏压选项，4个公共口

#define TONE4K      0X40    //0B 100	0100	0000	X  声音频率4K
#define TONE2K      0X50    //0B 100	0110	0000	X  声音频率2K
#define IRQ_DIS     0X80    //0B 100	1000	0000	X  使/IRQ输出失效
#define IRQ_EN      0X88    //0B 100	1000	1000	X  使/IRQ输出有效
#define BIAS        0x52    //0b1000 0101 0010 1/3duty 4com

//地址，SEG值
#define DisTime_shi_H 0X11 //Seg17
#define DisTime_shi_L 0X10 //Seg16

#define DisTime_ge_H 0X0F //Seg15
#define DisTime_ge_L 0X0E //Seg14

#define DisPower_shi_H 0X0D //Seg13
#define DisPower_shi_L 0X0C //Seg12

#define DisPower_ge_H 0X0B //Seg11
#define DisPower_ge_L 0X0A //Seg10

#define DisSoundInten_shi_H 0X04 //Seg4
#define DisSoundInten_shi_L 0X03 //Seg3

#define DisSoundInten_ge_H 0X02 //Seg2
#define DisSoundInten_ge_L 0X01 //Seg1

#define DisMHz_H 0X08 //Seg8
#define DisMHz_L 0X07 //Seg7

#define DisDuty_H 0X06 //Seg6
#define DisDuty_L 0X05 //Seg5

#define DisFlash 0X09 //Seg9

#define Run_Mode 0X00 //Seg0

//seg0,数据，COM值。
#define Mode_Serial 0X04      //com2
#define Mode_Interrupted 0X08 //com3
#define Mode_Mode 0X02        //com1

//seg9,mark掩码
#define MHz_Fresh 0X01  //com0
#define Time_Fresh 0X08 //com3
#define Out_Power 0X04  //com2

#define COM_1 0X01 //com0
#define COM_2 0X02 //com1
#define COM_3 0X04 //com2
#define COM_4 0X08 //com3

#define READ_DATA_CODE 0xC0  // 读命令
#define WRITE_DATA_CODE 0xA0 // 写命令
#define RD_DATA_CODE 0xA0    // 读写命令
#define COMMAND_CODE 0x80    // 命令码

typedef struct
{
    gpio_object_t   cs_pin;
    gpio_object_t   wr_clk_pin;
    gpio_object_t   rd_clk_pin;
    gpio_object_t   data_pin;

    void (*delay_us)         (uint32_t us);
    /* 可使用标准 spi 接口 */
} ht162x_t;

int ht162x_bit_send(ht162x_t *p_dev, uint8_t data, uint8_t cnt);
int ht162x_data_bit_send(ht162x_t *p_dev, uint8_t data, uint8_t cnt);
int ht162x_cmd_send(ht162x_t *p_dev, uint8_t cmd);
int ht162x_write(ht162x_t *p_dev, uint8_t addr, uint8_t data);
int ht162x_all_write(ht162x_t *p_dev, uint8_t addr, uint8_t *p, uint8_t cnt);
int ht162x_all_show(ht162x_t *p_dev);
int ht162x_init(ht162x_t *p_dev);
int ht162x_all_clean(ht162x_t *p_dev);

#endif
