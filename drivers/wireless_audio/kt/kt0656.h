#ifndef __KT0656_H
#define __KT0656_H

#define KT0656_DEV_ADD_H 0x39
#define KT0656_DEV_ADD_L 0x31


//#define   XTAL_DUAL
#define     XTAL_24M_ONLY
//#define   XTAL_24P576M_ONLY

#ifdef	    XTAL_DUAL
//#define   OLD_SEL_XTAL_MODE 			  //KT0616M选择晶体的程序
#define     NEW_SEL_XTAL_MODE 			  //KT0656M选择晶体的程序
#endif

//#define   AUTO_SEARCH_FREQ	 //自动搜台程序

#ifdef  XTAL_24P576M_ONLY
#define     XTAL_SEL1  1 //0:24MHz xtal 1:24.576MHz xtal
#define     XTAL_SEL2  0 //0:24MHz xtal 1:24.576MHz xtal
#else
#define     XTAL_SEL1  0 //0:24MHz xtal 1:24.576MHz xtal
#define     XTAL_SEL2  1 //0:24MHz xtal 1:24.576MHz xtal
#endif


#define XTAL_24M_FREQ       0
#define XTAL_24P576M_FREQ   1

#define HLSI_INV            0   //0: (HLSI); 1: inv(HLSI)

#define DCDC_EN             1   //DCDC使能位：0：关闭；1：使能。

#define AUTOMUTE_EN //undefine to disable automute
//#define SQUEAL_EN //undefine to disable squeal
#define I2S_EN


#define MUTE                    1
#define UNMUTE                  0

#define FAST_RSSI_MUTE_EN       1     //1:enable 0:disable
#define FAST_RSSI_PD_MUTE_EN    1     //1:enable 0:disable
#define SUPER_RSSI_MUTE_EN      1     //1:enable 0:disable

#define AUTOMUTE_SNR_LOWTH	   0x78
#define AUTOMUTE_SNR_HIGHTH	   0x80

//I2C地址的bit4根据芯片ADDR决定，高为1，低为0. I2C地址的bit3根据主还是从决定：主为1，从为0
#define KTWirelessMicRxw_addressAM  0x72//0x7A
#define KTWirelessMicRxr_addressAM  0x73//0x7B

#define KTWirelessMicRxw_addressAS  0x7a//0x72
#define KTWirelessMicRxr_addressAS  0x7b//0x73

#define KTWirelessMicRxw_addressBM  0x62//0x6A
#define KTWirelessMicRxr_addressBM  0x63//0x6B

#define KTWirelessMicRxw_addressBS  0x6a
#define KTWirelessMicRxr_addressBS  0x6b

#define KTWirelessMicRxw_addressOTP 0x62
#define KTWirelessMicRxr_addressOTP 0x63


#define INIT_FAIL_TH                3

//频点范围及步进
#define    BAND_TOP_CHA            754850//727550//754850//662500
#define    BAND_BOTTOM_CHA         740150//710450//740150
#define    BAND_TOP_CHB            800000//743750//769850
#define    BAND_BOTTOM_CHB         650000//728150//755150
#define    BAND_STEP               50

//电池电压检测
#define    BATTERY_MAX             0x7FF
#define    BATTERY_HIGHTH          0x500
#define    BATTERY_MIDDLETH        0x4C0
#define    BATTERY_LOWTH           0x4A0

#ifdef KT0655M
#define LINEIN_AGC_DIS	 0 			//0：使用自动调整增益的功能 1：不使用自动调整功能
#define LINEIN_LOCAL_VOL_CTRL 30
#define COMPEN_GAIN	   2  			//0：补偿后总增益为0dB 1：补偿后总体增益为6dB 2：补偿后总体增益为12dB 3：补偿后总体增益为18dB
#define PGA_GAIN_SEL 3 				// 2'b00：-6dB 2'b01：0dB 2'b10：6dB	2'b11：12dB
#define	SLNC_MUTE_TIME	0x13		//
#define SLNC_MUTE_DIS  1			//0：使能Silence Mute功能 1：关闭Silence Mute功能。
#define	SLNC_MUTE_LOW_LEVEL	0x04
#define SLNC_MUTE_HIGH_LEVEL 0x06

#define ALC_DIS	1					//0：开启 1：不开启。
#define ALC_VMAX  0x7f				//
#define ALC_SOFTKNEE  1				//
#endif

//REG0x0200
#define ADJUST_GAIN			  1		   //0:75KHz 1:50KHz 2:37.5KHz 3:25KHz 4:20KHz 5:120KHz 6:100KHz

//REG0x0241
#define BPSK_NEW_MODE         1//0//1        //1:new mode  0:old mode
#define AUXDATA_EN            1        //

#define CARRY_NUM_TH          4        // 3'b000:10 3'b001:12  3'b010:14 3'b011:16 3'b100:18 3'b101:20  3'b110:22 3'b111:24
#define AUX_CARRY_NUM         3        //3'b000 32 3'b001 28	3'b010 24  3'b011 22 3'b100 20 3'b101 18  3'b110 16	 3'b111 12

//REG0x026F
#define SQUEAL_ELIM_EN        1    //0: disable;    1: ensable		bit5
#define SQEUAL_DET_EN         3	   //0:关闭 1:保留 2:保留 3:开启	bit4:3
#define FNOTCH_NOTRESP_TH     1    //0: 1/4;    1: 1/2
#define N_OCTAVE              3//0    //0: 1/5;    1: 1/10;    2: 1/20;    3:1/80

//REG0x0270
#define FFT_R_TH            15    //8// 0: 0;        15: 30
#define FRAME_NUM_TH        0    //4    //

//REG0x0271
#define    PMAX_HITH          14  //8
#define    PMAX_LOWTH         12  //6
#define    AFC_RNG		  	  2//3	   //2'b00: +/-20kHz;2'b01: +/-40kHz;2'b10: +/-60kHz;2'b11: +/-90kHz;

//REG0x0272
#define    FDIFF_HITH         7//15
#define    FDIFF_LOWTH        1//7

#define ECHO_EN               1
#define ECHO_DIS              0

#define ECHO_STRU         1    //0: 全通; 1: 梳状
#define ECHO_GAIN_DOWN    0    //0: -13dB; 1: -10dB; 2: -7dB;
#define ECHO_GAIN_UP      7    //0: 0dB; 1: 1.9dB; 2: 3.5dB; 3: 5.5dB; 4: 7dB; 5: 9.4dB; 6: 10.9dB; 7: 13.1dB;
#define ECHO_RATIO        10    //0~25: 0~25/32 ECHO反馈比例
#define ECHO_DELAY        23    //0~23:    22ms~197ms ECHO信号延时

#define EXCITER_EN        1
#define EXCITER_DIS       0

#define EXCITER_TUNE    2    //0: 600Hz; 1: 1KHz; 2: 2KHz; 3: 3.8KHz; 激励起始频率
#define EXCITER_DRIVE   0    //0: 0dB; 1: 3.5dB; 2: 6dB; 3: 9dB; 4: 12dB; 5: 15dB; 激励剩余增益
#define EXCITER_ODD     1    //0~6:    负无穷~0dB 奇次激励衰减量
#define EXCITER_EVEN    1    //0~6:    负无穷~0dB 偶次激励衰减量

#define EQ_EN           1
#define EQ_DIS          0

#define EQ_25Hz         0
#define EQ_40Hz         1
#define EQ_63Hz         2
#define EQ_100Hz        3
#define EQ_160Hz        4
#define EQ_250Hz        5
#define EQ_400Hz        6
#define EQ_630Hz        7
#define EQ_1KHz         8
#define EQ_1K6Hz        9
#define EQ_2K5Hz        10
#define EQ_4KHz         11
#define EQ_6K3Hz        12
#define EQ_10KHz        13
#define EQ_16KHz        14

#define EQ_Neg12dB       0
#define EQ_Neg11dB       1
#define EQ_Neg10dB       2
#define EQ_Neg9dB        3
#define EQ_Neg8dB        4
#define EQ_Neg7dB        5
#define EQ_Neg6dB        6
#define EQ_Neg5dB        7
#define EQ_Neg4dB        8
#define EQ_Neg3dB        9
#define EQ_Neg2dB        10
#define EQ_Neg1dB        11
#define EQ_Pos0dB        12
#define EQ_Pos1dB        13
#define EQ_Pos2dB        14
#define EQ_Pos3dB        15
#define EQ_Pos4dB        16
#define EQ_Pos5dB        17
#define EQ_Pos6dB        18
#define EQ_Pos7dB        19
#define EQ_Pos8dB        20
#define EQ_Pos9dB        21
#define EQ_Pos10dB       22
#define EQ_Pos11dB       23
#define EQ_Pos12dB       24

#define PRESET_VOL       31 //0: mute; 31: maximum volume

#define  PKGSYNC         1
#define  NON_PKGSYNC     0

#define KT_I2S_LRCLK48K		    0
#define KT_I2S_LRCLK96K		    1
#define KT_I2S_LRCLK192K		2
#define KT_I2S_MCLK24P576M		1
#define KT_I2S_MCLK12P288M		0
#define KT_I2S_MASTER 			1
#define KT_I2S_SLAVE  			0
#define KT_I2S_STRREO 			1
#define KT_I2S_MONO   			0
#define KT_I2S_LEFT   			1
#define KT_I2S_RIGHT  			0
#define KT_I2S_NOMODE  		    0
#define KT_I2S_I2SMODE  		1
#define KT_I2S_LEFTMODE 		2
#define KT_I2S_RIGHTMODE 		3
#define KT_I2S_LENGHT16BIT		0
#define KT_I2S_LENGHT20BIT		1
#define KT_I2S_LENGHT24BIT		2
#define KT_I2S_LENGHT8BIT		3

typedef struct
{
    int32_t memery_frequency;
    uint8_t echoflag;       //1:ON 0:OFF
    uint8_t equalierflag;   //1:ON 0:OFF
    uint8_t exciterflag;    //1:ON 0:OFF
    uint8_t diversityflag;  //1:ON 0:OFF
    uint8_t pilotflag;      //1:ON 0:OFF
    uint8_t echo_depth;
    uint8_t echo_delay;
    uint8_t equalier16Sel[15];
    uint8_t exciter_odd;
    uint8_t exciter_even;
} kt0656_sound_effect_t;

typedef struct
{
	uint8_t lrclk;
	uint8_t mclk;
	uint8_t is_master;          //1:master 0:slave
	uint8_t stereo_or_mono;     //1:stereo 0:Mono
	uint8_t left_or_right;      //1:left 0:right
	uint8_t mode;               //0:不能选 1:I2S 2:左对齐 3:右对齐
	uint8_t data_length;        //0:16 1:20 2:24 3:8
} kt0656_i2s_t;




typedef struct
{
    uint16_t  reg;
    uint8_t   value;
} kt0656_reg_value_t;

typedef struct 
{
    uint8_t                     device_addr;
    uint16_t                    chip_id;
    mid_kt0656_t                mid_kt0656_object;

    void                        *p_rst_gpio;    //指向复位引脚
} kt0656_object_t;



#endif
