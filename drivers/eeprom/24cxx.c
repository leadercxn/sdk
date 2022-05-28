#include "24cxx.h"

uint8_t eeprom_write_reg(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	uint8_t ret = 0;

    p_i2c_obj->i2c_ops.xfer_start(&p_i2c_obj->i2c_cfg);
    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, 0xA0);
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    // p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, (reg >> 8));
    // p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);
    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, (reg & 0XFF));
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);


	for (i = 0; i < len; i++)
	{	   
        p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, buf[i]);
        ret = p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);
		if (ret)
            break;  
	}

    p_i2c_obj->i2c_ops.xfer_stop(&p_i2c_obj->i2c_cfg);	//产生一个停止条件	    
	return ret; 
}

void eeprom_read_reg(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i = 0;

    p_i2c_obj->i2c_ops.xfer_start(&p_i2c_obj->i2c_cfg);
    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, 0xA0);
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, (reg & 0xff));
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

    p_i2c_obj->i2c_ops.xfer_start(&p_i2c_obj->i2c_cfg);     
    p_i2c_obj->i2c_ops.send_byte(&p_i2c_obj->i2c_cfg, 0xA1);
    p_i2c_obj->i2c_ops.wait_ack(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops);

	for (i = 0; i < len; i++)
	{	   
        buf[i] = p_i2c_obj->i2c_ops.read_byte(&p_i2c_obj->i2c_cfg, &p_i2c_obj->i2c_ops, (i==(len-1)?0:1));
		p_i2c_obj->i2c_cfg.delay_us(30);
	} 
    p_i2c_obj->i2c_ops.xfer_stop(&p_i2c_obj->i2c_cfg);	//产生一个停止条件
}

uint8_t eeprom_init(i2c_object_t *p_i2c_obj)
{
    p_i2c_obj->i2c_ops.init(&p_i2c_obj->i2c_cfg);

    return 0;
}