#ifndef __EXFUNS_H
#define __EXFUNS_H 			

#include "stm32f10x.h"
#include "ff.h"
#include "fatfsobj.h"
#include <stdint.h>


uint8_t exfuns_init(struct fatfs_cfg *p_cfg);	
uint8_t exfuns_deinit(struct fatfs_cfg *p_cfg);						       
uint8_t f_typetell(struct fatfs_cfg *p_cfg, uint8_t *fname);						   
uint8_t exf_getfree(struct fatfs_cfg *p_cfg, uint8_t *drv);	
uint32_t exf_fdsize(struct fatfs_cfg *p_cfg, uint8_t *fdname);					
uint8_t* exf_get_src_dname(struct fatfs_cfg *p_cfg, uint8_t* dpfn);																		   
uint8_t exf_copy(struct fatfs_cfg *p_cfg, fun_cpymsg p_fun, uint8_t *psrc, 
                    uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);	   
uint8_t exf_fdcopy(struct fatfs_cfg *p_cfg, fun_cpymsg p_fun, uint8_t *psrc, 
                    uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode); 

#endif


