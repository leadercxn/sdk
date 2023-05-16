#include "board_config.h"
#include "kt0646.h"

#define MID_KT0646_READ(reg,p_data)     \
{                                       \
    err_code = mid_kt0646_read_one_reg(&p_kt0646_obj->mid_kt0646_object, p_kt0646_obj->device_addr, reg, p_data);   \
    if(err_code)                        \
    {                                   \
        trace_error("mid_kt0646_read_one_reg 0x%02x error %d\n\r", reg ,err_code);                         \
        return err_code;                \
    }                                   \
}\


#define MID_KT0646_WRITE(reg,p_data)    \
{                                       \
    err_code = mid_kt0646_write_one_reg(&p_kt0646_obj->mid_kt0646_object, p_kt0646_obj->device_addr, reg, p_data);   \
    if(err_code)                        \
    {                                   \
        trace_error("mid_kt0646_write_one_reg 0x%02x  error %d\n\r",reg ,err_code);                         \
        return err_code;               \
    }                                   \
}\

static uint8_t g_lopa_div_tab[13] = {
    6, 8, 10, 12,
      16, 20, 24,
      32, 40, 48,
      64, 80, 96,
};

static uint8_t g_lopa_div_tab2[13][2]={
    {0,0},
    {0,1}, 
    {0,2}, 
    {0,3},
    {1,1}, 
    {1,2}, 
    {1,3},
    {2,1}, 
    {2,2}, 
    {2,3},
    {3,1}, 
    {3,2}, 
    {3,3},
};

/**
 * @brief 获取芯片ID
 */
int kt0646_chid_id_get(kt0646_object_t *p_kt0646_obj, uint16_t *p_id)
{
    IS_NULL(p_kt0646_obj);
    IS_NULL(p_id);

    int      err_code = 0;
    uint8_t  rty      = 3;

    for(uint8_t i = 0; i < rty; i++)
    {
        MID_KT0646_READ(0x01,p_id);
        if(p_id == KT0646_CHID_ID)
        {
            return 0;
        }
        delay_ms(10);
    }

    return err_code;
}

/**
 * @brief 设置麦克风灵明度
 */
int kt0646_mic_sensitivity_set(kt0646_object_t *p_kt0646_obj, uint8_t sensitivity)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    if(sensitivity > 0x0F)
    {
        sensitivity = 0x0F;
    }

    MID_KT0646_READ(0x1C,&value);

    value &= 0xFE1F;
    value |= (((uint16_t)sensitivity) << 5);
    MID_KT0646_READ(0x1C,&value);

    return err_code;
}

/**
 * 压缩器使能
 */
int kt0646_compandor_enable(kt0646_object_t *p_kt0646_obj, bool enable)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    MID_KT0646_READ(0x1C, &value);

    if(enable)
    {
        SET_BIT(value,4);
        MID_KT0646_WRITE(0x1C, &value);

        MID_KT0646_READ(0x1D, &value);
        value &= 0x8000;
        SET_BIT(value,14);
        MID_KT0646_WRITE(0x1D, &value);

        MID_KT0646_READ(0x1E, &value);
        value &= 0xFFF0;
        MID_KT0646_WRITE(0x1E, &value);
    }
    else
    {
        CLR_BIT(value, 4);
        MID_KT0646_WRITE(0x1C, &value);
    }

    return err_code;
}

#ifdef IS_OTHER_RX              //使用其他方案的接收机
/**
 * @brief 是否设置前导码频率
 */
int kt0646_pilot_freq_set(kt0646_object_t *p_kt0646_obj, bool enable)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    if(enable)
    {
        MID_KT0646_WRITE(0x2C, ((PILOT_FREQ << 13) / 9375));
    }
    else
    {
        MID_KT0646_WRITE(0x2C, ((PILOT_FREQ << 13) / 9600));
    }

    return err_code;
}
#endif

int kt0646_mute_set(kt0646_object_t *p_kt0646_obj, bool enable)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    MID_KT0646_READ(0x1C, &value);
    CLR_BIT(value,0);

    if(enable)
    {
        SET_BIT(value,0);
    }
    MID_KT0646_WRITE(0x1C, &value);

    return err_code;
}

int kt0646_pilot_enable(kt0646_object_t *p_kt0646_obj, bool enable)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    MID_KT0646_READ(0x1F, &value);

#ifdef IS_OTHER_RX
    value = (value & 0xFFBF) | (enable << 6);
#endif

#ifdef IS_KT_RX
    value = (value & 0x7FFF) | (enable << 15);
#endif

    MID_KT0646_WRITE(0x1F, &value);

    delay_ms(20);

    return err_code;
}

int kt0646_pa_enable(kt0646_object_t *p_kt0646_obj, bool enable)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    MID_KT0646_READ(0x0F, &value);

    value = (value & 0xFFF7) | (enable << 3);

    MID_KT0646_WRITE(0x0F, &value);

    return err_code;
}

/**
 * @brief 设置PA增益
 */
int kt0646_pa_gain_set(kt0646_object_t *p_kt0646_obj, uint8_t gain)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    if(gain > 0x3F)
    {
        gain = 0x3F;
    }

    MID_KT0646_READ(0x11, &value);
    value &= 0xFF00;
    value |= gain;
    MID_KT0646_WRITE(0x11, &value);

    return err_code;
}

/**
 * @brief 读取电池电量
 */
int kt0646_battery_level_get(kt0646_object_t *p_kt0646_obj, uint16_t *p_level)
{
    IS_NULL(p_kt0646_obj);
    IS_NULL(p_level);

    int      err_code = 0;
    uint16_t value;

    MID_KT0646_READ(0x07, &value);

    *p_level = value & 0x7F;

    return err_code;
}

/**
 * @brief 设置数字辅助信道的频偏
 */
int kt0646_pilot_freq_set(kt0646_object_t *p_kt0646_obj, uint8_t freq_level)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    if(freq_level > 0x03)
    {
        freq_level = 3;
    }

    MID_KT0646_READ(0x1F, &value);
#ifdef IS_OTHER_RX
    value &= 0xFFCF;
    value |= freq_level << 4;
#endif

#ifdef IS_KT_RX
    value &= 0xFE7F;
    value |= freq_level << 7;
#endif

    MID_KT0646_WRITE(0x1F, &value);

    delay_ms(20);

    return err_code;
}

int kt0646_chan_reg_calc(int32_t freq, uint8_t *p_lopa_div1_ptr,uint8_t *p_lopa_div2_ptr,
                                   uint16_t *p_chan_ptr, uint16_t *p_chan_frac_ptr, 
                                   uint8_t *p_chan_frac_msb_ptr, uint32_t *p_chan_frac_temp_ptr,int32_t xtal_freq)
{
    uint32_t    chan_frac;
    int32_t     fvco;
    uint32_t    temp;
    uint32_t    temp_min = xtal_freq;
    uint16_t    lopa_div;
    long        vco_high_th = 4320000;

}

/**
 * @brief 进入待机模式
 */
int kt0646_standby_mode_set(kt0646_object_t *p_kt0646_obj)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    kt0646_mute_set(p_kt0646_obj, true);
    kt0646_pilot_enable(p_kt0646_obj, false);
    kt0646_pa_enable(p_kt0646_obj, false);

    MID_KT0646_READ(0x0A, &value);
    SET_BIT(value,12);
    MID_KT0646_WRITE(0x0A, &value);

    MID_KT0646_READ(0x03, &value);
    SET_BIT(value,15);
    MID_KT0646_WRITE(0x03, &value);

    delay_ms(10);
}

/**
 * @brief 唤醒进入工作模式
 */
int kt0646_normal_mode_set(kt0646_object_t *p_kt0646_obj)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    MID_KT0646_READ(0x03, &value);
    CLR_BIT(value,15);
    MID_KT0646_WRITE(0x03, &value);

    delay_ms(50);
    kt0646_reg_init(p_kt0646_obj);
}

int kt0646_reg_init(kt0646_object_t *p_kt0646_obj)
{
    IS_NULL(p_kt0646_obj);

    int      err_code = 0;
    uint16_t value;

    /**
     * PA 由 PA_ON 控制
     */
    MID_KT0646_READ(0x03, &value);
    SET_BIT(value,0);
    MID_KT0646_WRITE(0x03, &value);

    //打开电池电压检测
    MID_KT0646_READ(0x24, &value);
    SET_BIT(value,12);
    MID_KT0646_WRITE(0x24, &value);

    //选择晶体1
    MID_KT0646_READ(0x47, &value);
    SET_BIT(value,5);   //bit待定，数据手册bit5, demo例程bit12
    MID_KT0646_WRITE(0x47, &value);

    //调谐
    MID_KT0646_READ(0x0a, &value);
    SET_BIT(value,12);
    MID_KT0646_WRITE(0x0a, &value);

    //麦克风灵敏度 10dB,
    MID_KT0646_READ(0x1C, &value);
    value &= 0xF1E1;
    value |= FDEV_MONITOR_TC_250ms << 10;
    value |= PRE_EMPHASIS_ENABLE << 9;
    value |= COMPANDOR_TC_12ms << 1;
    value |= 3 << 5;
    MID_KT0646_WRITE(0x1C, &value);

    kt0646_compandor_enable(p_kt0646_obj, false);

    kt0646_mic_sensitivity_set(p_kt0646_obj,MIC_SENS_GAIN_5);

    //干扰检测
    MID_KT0646_READ(0x1E, &value);
    SET_BIT(value,5);
    MID_KT0646_WRITE(0x1E, &value);

    //
    MID_KT0646_READ(0x30, &value);
    value &= 0x8000;
    value |= ((AGC_VHIGH << 11) | (AGC_VLOW << 7) | (AGC_ATTACK << 4) | (AGC_RELEASE << 1) | AGC_DIS);
    MID_KT0646_WRITE(0x30, &value);

    MID_KT0646_READ(0x31, &value);
    value &= 0xFF00;
    value |= ((GAIN_SEL << 6) | (COMPEN_GAIN << 4) | (BLANK_EN << 3) | BLANK_TIME);
    MID_KT0646_WRITE(0x31, &value);

    MID_KT0646_READ(0x39, &value);
    value &= 0xFC00;
    value |= ((HARD_LIMIT << 6) | (CPRS_1XLPF_BP << 5) |  (CPRS_KNEE_DIS << 4) | CPRS_THRSH);
    MID_KT0646_WRITE(0x39, &value);

    MID_KT0646_READ(0x3a, &value);
    value |= ((ALC_DIS << 15) | (ALC_SOFTKNEE << 14) | (ALC_VMAX << 7) | (ALC_ATTACK << 4) | ALC_RELEASE);
    MID_KT0646_WRITE(0x3a, &value);

#ifdef SILENCE_MUTE
    MID_KT0646_READ(0x25, &value);
    value &= 0xC000;
    value |= ((SLNC_MUTE_DIS << 13) | (SLNC_MUTE_TIME << 8) | (SLNC_MUTE_LOW_LEVEL << 4) | SLNC_MUTE_HIGH_LEVEL);
    MID_KT0646_WRITE(0x25,value);

    MID_KT0646_READ(0x26, &value);
    value &= 0xfffb;
    value |= (SILENCE_MUTE_ACT_MCU << 2);
    MID_KT0646_WRITE(0x26,value);
#endif

 #ifdef AUX_CH
    regx=KT_Bus_Read(0x1f);
    KT_Bus_Write(0x1f,(regx&0x80f0)|(AUXDATA_EN << 14)|(AUX_REG_NUM << 12) |
                 (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE); 
    KT_Bus_Write(0x20,(AUX_ADDRB << 8) | AUX_ADDRA);
    KT_Bus_Write(0x21,(AUX_ADDRD << 8) | AUX_ADDRC);    

    MID_KT0646_READ(0x1F, &value);
    value &= 0x80f0;
    value |= ((AUXDATA_EN << 14)|(AUX_REG_NUM << 12) | (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE);
    MID_KT0646_WRITE(0x1F,value);

    value = 0;
    value = (AUX_ADDRB << 8) | AUX_ADDRA);
    MID_KT0646_WRITE(0x20,value);
    value = 0;
    value = (AUX_ADDRD << 8) | AUX_ADDRC);
    MID_KT0646_WRITE(0x21,value);
#endif

#ifdef IS_OTHER_RX
    #ifdef XTAL_24M_ONLY
        kt0646_pilot_freq_set(p_kt0646_obj, false);
    #endif

    #ifdef XTAL_24P576M_ONLY
        kt0646_pilot_freq_set(p_kt0646_obj, true);
    #endif
#endif

    MID_KT0646_READ(0x10, &value);
    SET_BIT(value,4);
    MID_KT0646_WRITE(0x10, &value);

    MID_KT0646_READ(0x45, &value);
    value = value & 0xFFF0 | 0x0007;
    MID_KT0646_WRITE(0x45, &value);

    MID_KT0646_READ(0x2D, &value);
    value = value & 0xFFF8 | 0x0003;
    MID_KT0646_WRITE(0x2D, &value);

#ifdef PLL_UNLOCK
    MID_KT0646_READ(0x2F, &value);
    SET_BIT(value,7);
    MID_KT0646_WRITE(0x2F, &value); //pll_unlock_en

    MID_KT0646_READ(0x44, &value);
    value = value & 0x1FFF | (1<<13);
    MID_KT0646_WRITE(0x44, &value); //lo_lock_hth_vsel

    MID_KT0646_READ(0x44, &value);
    value = value & 0xE3FF | (3<<10);
    MID_KT0646_WRITE(0x44, &value); //lo_lock_lth_vsel

    MID_KT0646_READ(0x44, &value);
    value = value & 0xFDFF;
    MID_KT0646_WRITE(0x44, &value); //lo_lock_det_pd
#endif

    return err_code;
}


