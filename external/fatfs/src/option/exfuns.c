#include "string.h"
#include "exfuns.h"
#include "little_malloc.h"


#define FILE_MAX_TYPE_NUM		6			//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		13			//���FILE_MAX_SUBT_NUM��С��

 //�ļ������б�
uint8_t* FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM] =
{
	{"BIN"},																			//BIN�ļ�
	{"LRC"},																			//LRC�ļ�
	{"NES"},																			//NES�ļ�
	{"TXT","C","H"},																	//�ı��ļ�
	{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","AAC","WMA","WAV","MID","FLAC"},	//�����ļ�
	{"BMP","JPG","JPEG","GIF"},															//ͼƬ�ļ� 
};

/*****************************�����ļ���,ʹ��malloc��ʱ��*****************************/
FATFS *fs[_VOLUMES];		//�߼����̹�����.	 
FIL *file;	  				//�ļ�1
FIL *ftemp;	  				//�ļ�2.
UINT br, bw;					//��д����
FILINFO fileinfo;			//�ļ���Ϣ
DIR dir;  					//Ŀ¼
uint8_t *fatbuf;			//SD�����ݻ�����

//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
uint8_t exfuns_init(void)
{
	uint8_t i;
	for (i = 0; i < _VOLUMES; i++)
	{
		fs[i] = (FATFS*)little_mem_malloc(SRAMIN, sizeof(FATFS));	//Ϊ����i�����������ڴ�	
		if (!fs[i])
		{
			break;
		}
	}

	file = (FIL*)little_mem_malloc(SRAMIN, sizeof(FIL));		//Ϊfile�����ڴ�
	ftemp = (FIL*)little_mem_malloc(SRAMIN, sizeof(FIL));		//Ϊftemp�����ڴ�
	fatbuf = (uint8_t*)little_mem_malloc(SRAMIN, 512);			//Ϊfatbuf�����ڴ�
	if (i == _VOLUMES && file && ftemp && fatbuf)				//������һ��ʧ��,��ʧ��.
	{
		return 0;  
	}
	else 
	{
		return 1;	
	}
}

//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
uint8_t char_upper(uint8_t c)
{
	if (c < 'A')
	{
		return c;
	}
	if (c >= 'a')
	{
		return c - 0x20;
	}
	else 
	{
		return c;
	}
}	     

//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
uint8_t f_typetell(uint8_t *fname)
{
	uint8_t tbuf[5];
	uint8_t *attr = '\0';//��׺��
	uint8_t i = 0,j;

	while (i<250)
	{
		i++;
		if (*fname == '\0')
		{
			break;
		}
		fname++;
	}

	if (i == 250)
	{
		return 0XFF;
	}

	for (i = 0; i < 5; i++)		//�õ���׺��
	{
		fname--;
		if (*fname == '.')
		{
			fname++;
			attr = fname;
			break;
		}
	}
	strcpy((char *)tbuf, (const char*)attr);	//copy
	for (i = 0; i < 4; i++)						//ȫ����Ϊ��д 
	{
		tbuf[i]=char_upper(tbuf[i]);
	}

	for (i = 0; i < FILE_MAX_TYPE_NUM; i++)		//����Ա�
	{
		for (j = 0; j < FILE_MAX_SUBT_NUM; j++)	//����Ա�
		{
			if (*FILE_TYPE_TBL[i][j] == 0)      //�����Ѿ�û�пɶԱȵĳ�Ա��.
			{
				break;
			}

			if (strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0)
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;	 			   
}	 

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
uint8_t exf_getfree(uint8_t *drv, uint32_t *total, uint32_t *free)
{
	FATFS *fs1;
	uint8_t res;
    uint32_t fre_clust=0, fre_sect=0, tot_sect=0;

    //�õ�������Ϣ�����д�����
    res = (uint32_t)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);

    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent-2) * fs1->csize;	//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	 
		
#if _MAX_SS != 512				  					//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect *= fs1->ssize / 512;
		fre_sect *= fs1->ssize / 512;
#endif	  
		*total = tot_sect >> 1;	//��λΪKB
		*free = fre_sect >> 1;	//��λΪKB 
	}
	return res;
}	
