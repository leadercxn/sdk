#ifndef __LITTLE_MALLOC_H
#define __LITTLE_MALLOC_H

#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

//���������ڴ��
#define SRAMIN	 0		//�ڲ��ڴ��
#define SRAMEX   1		//�ⲿ�ڴ�� 

// TODO: �˴�ֻ֧��һ��sram
#define SRAMBANK 	1	//����֧�ֵ�SRAM����.	

// mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����.
#define MEM1_BLOCK_SIZE			32  	  							//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			40*1024  							//�������ڴ� 40K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE / MEM1_BLOCK_SIZE 	//�ڴ���С

// mem2�ڴ�����趨.mem2���ڴ�ش����ⲿSRAM����
#define MEM2_BLOCK_SIZE			32  	  							//�ڴ���СΪ32�ֽ�
#define MEM2_MAX_SIZE			960 *1024  							//�������ڴ�960K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE / MEM2_BLOCK_SIZE 	//�ڴ���С 

// �ڴ���������
struct _m_mallco_dev
{
	void		(*init)(uint8_t);			// ��ʼ��
	uint8_t 	(*perused)(uint8_t);		// �ڴ�ʹ����
	uint8_t		*membase[SRAMBANK];			// �ڴ�� ����SRAMBANK��������ڴ�
	uint16_t 	*memmap[SRAMBANK]; 			// �ڴ����״̬��
	uint8_t  	memrdy[SRAMBANK]; 			// �ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 	// ��mallco.c���涨��

void little_memset(void *s,uint8_t c,uint32_t count);		//�����ڴ�
void little_memcpy(void *des,void *src,uint32_t n);			//�����ڴ�     
void little_mem_init(uint8_t memx);							//�ڴ�����ʼ������(��/�ڲ�����)
uint32_t little_mem_malloc(uint8_t memx,uint32_t size);		//�ڴ����(�ڲ�����)
uint8_t little_mem_free(uint8_t memx,uint32_t offset);		//�ڴ��ͷ�(�ڲ�����)
uint8_t little_mem_perused(uint8_t memx);					//����ڴ�ʹ����(��/�ڲ�����) 

//�û����ú���
void little_free(uint8_t memx,void *ptr);  					//�ڴ��ͷ�(�ⲿ����)
void *little_malloc(uint8_t memx,uint32_t size);			//�ڴ����(�ⲿ����)
void *little_realloc(uint8_t memx,void *ptr,uint32_t size);	//���·����ڴ�(�ⲿ����)
#endif
