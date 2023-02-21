#include "little_malloc.h"	    

//�ڴ��(32�ֽڶ���)
// __align(32) uint8_t mem1base[MEM1_MAX_SIZE];												//�ڲ�SRAM�ڴ��
// __align(32) uint8_t mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));				//�ⲿSRAM�ڴ��

uint8_t mem1base[MEM1_MAX_SIZE] __attribute__ ((aligned(32)));
uint8_t mem2base[MEM1_MAX_SIZE] __attribute__ ((aligned(32)));

//�ڴ�����
uint16_t mem1mapbase[MEM1_ALLOC_TABLE_SIZE];												//�ڲ�SRAM�ڴ��MAP
uint16_t mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE)));	//�ⲿSRAM�ڴ��MAP
//�ڴ�������	   
const uint32_t memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE};			//�ڴ���С
const uint32_t memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE};						//�ڴ�ֿ��С
const uint32_t memsize[SRAMBANK] = {MEM1_MAX_SIZE, MEM2_MAX_SIZE};								//�ڴ��ܴ�С


//�ڴ���������
struct _m_mallco_dev mallco_dev=
{
	little_mem_init,				//�ڴ��ʼ��
	little_mem_perused,			    //�ڴ�ʹ����
	mem1base,mem2base,			    //�ڴ��
	mem1mapbase,mem2mapbase,	    //�ڴ����״̬��
	0, 0,  		 				    //�ڴ����δ����
};

//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void little_memcpy(void *des, void *src, uint32_t n)  
{  
    uint8_t *xdes = des;
	uint8_t *xsrc = src; 
    while (n--)
    {
        *xdes++ = *xsrc++; 
    } 
}  

//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void little_memset(void *s,uint8_t c,uint32_t count)  
{  
    uint8_t *xs = s;  
    while (count--)
    {
        *xs++ = c;
    }  
}	   

//�ڴ�����ʼ��  
//memx:�����ڴ��
void little_mem_init(uint8_t memx)  
{  
    little_memset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 2);    //�ڴ�״̬����������  
	little_memset(mallco_dev.membase[memx], 0, memsize[memx]);	        //�ڴ��������������  
	mallco_dev.memrdy[memx] = 1;								        //�ڴ�����ʼ��OK  
}  

//��ȡ�ڴ�ʹ����
//memx:�����ڴ��
//����ֵ:ʹ����(0~100)
uint8_t little_mem_perused(uint8_t memx)  
{  
    uint32_t used = 0;  
    uint32_t i;  
    for (i = 0; i < memtblsize[memx]; i++)  
    {  
        if (mallco_dev.memmap[memx][i])
        {
            used++;
        } 
    } 
    return (used*100)/(memtblsize[memx]);  
}  

//�ڴ����(�ڲ�����)
//memx:�����ڴ��
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ 
uint32_t little_mem_malloc(uint8_t memx, uint32_t size)  
{  
    signed long offset = 0;  
    uint32_t nmemb;	                //��Ҫ���ڴ����  
	uint32_t cmemb=0;               //�������ڴ����
    uint32_t i;  

    if (!mallco_dev.memrdy[memx])    //δ��ʼ��,��ִ�г�ʼ�� 
    {
        mallco_dev.init(memx);             
    }

    if (size == 0)
    {
        return 0XFFFFFFFF;
    }

    nmemb = size / memblksize[memx];  	//��ȡ��Ҫ����������ڴ����
    if (size % memblksize[memx])
    {
        nmemb++;  
    }

    for (offset = memtblsize[memx] - 1; offset >= 0; offset--)  //���������ڴ������  
    {     
		if (!mallco_dev.memmap[memx][offset])       //�������ڴ��������
        {
            cmemb++;
        }
		else                                        //�����ڴ������
        {
            cmemb=0;								
        }

		if (cmemb == nmemb)							//�ҵ�������nmemb�����ڴ��
		{
            for (i = 0; i < nmemb; i++)  			//��ע�ڴ��ǿ� 
            {  
                mallco_dev.memmap[memx][offset+i] = nmemb;  
            }  
            return (offset * memblksize[memx]);     //����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;                              //δ�ҵ����Ϸ����������ڴ��  
}  

//�ͷ��ڴ�(�ڲ�����) 
//memx:�����ڴ��
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;  
uint8_t little_mem_free(uint8_t memx, uint32_t offset)  
{  
    int i;  
    if (!mallco_dev.memrdy[memx])                   //δ��ʼ��,��ִ�г�ʼ��
	{
		mallco_dev.init(memx);    
        return 1;                                   //δ��ʼ��  
    }  

    if (offset < memsize[memx])                     //ƫ�����ڴ����. 
    {  
        int index = offset / memblksize[memx];		//ƫ�������ڴ�����  
        int nmemb = mallco_dev.memmap[memx][index];	//�ڴ������
        for (i = 0; i < nmemb; i++)  				//�ڴ������
        {  
            mallco_dev.memmap[memx][index+i]=0;  
        }  
        return 0;  
    }
    else                                            //ƫ�Ƴ�����.
    {
        return 2;
    }  
}  

//�ͷ��ڴ�(�ⲿ����) 
//memx:�����ڴ��
//ptr:�ڴ��׵�ַ 
void little_free(uint8_t memx, void *ptr)  
{  
	uint32_t offset;   
	if (ptr == NULL)        //��ַΪ0.  
    {
        return;
    }
    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];     
    little_mem_free(memx, offset);	        //�ͷ��ڴ�      
}  

//�����ڴ�(�ⲿ����)
//memx:�����ڴ��
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
void *little_malloc(uint8_t memx, uint32_t size)  
{  
    uint32_t offset;   
	offset = little_mem_malloc(memx,size);  	   	 	   
    if (offset == 0XFFFFFFFF)
    {
        return NULL;  
    }
    else 
    {
        return (void*)((uint32_t)mallco_dev.membase[memx]+offset);  
    }
}  

//���·����ڴ�(�ⲿ����)
//memx:�����ڴ��
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *little_realloc(uint8_t memx, void *ptr, uint32_t size)  
{  
    uint32_t offset;    
    offset = little_mem_malloc(memx,size);   	

    if (offset == 0XFFFFFFFF)
    {
        return NULL;    
    } 
    else  
    {  									   
	    little_memcpy((void*)((uint32_t)mallco_dev.membase[memx]+offset), ptr, size);	//�������ڴ����ݵ����ڴ�   
        little_free(memx, ptr);  											  		    //�ͷž��ڴ�
        return (void*)((uint32_t)mallco_dev.membase[memx]+offset);  				    //�������ڴ��׵�ַ
    }  
}
