#ifndef __24CXX_H
#define __24CXX_H

#include "i2c.h"

uint8_t eeprom_init(i2c_object_t *p_i2c_obj);
uint8_t eeprom_check(i2c_object_t *p_i2c_obj);

uint8_t eeprom_read_byte(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf);
uint8_t eeprom_write_byte(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf);
void eeprom_read(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf, uint8_t len);
void eeprom_write(i2c_object_t *p_i2c_obj, uint16_t reg, uint8_t *buf, uint8_t len);

#endif
