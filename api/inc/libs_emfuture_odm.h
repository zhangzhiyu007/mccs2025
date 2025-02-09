/*
 * libs_emfuture_odm.h
 *
 *  Created on: 2016-3-3
 *      Author: Administrator
 */

#ifndef LIBS_EMFUTURE_ODM_H_
#define LIBS_EMFUTURE_ODM_H_

#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/socket.h>
#include <dirent.h>
#include <termios.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/rtc.h>
#include <sys/wait.h>
#include <memory.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <getopt.h>
#include <sys/file.h>
#include <signal.h>

//struct serial_rs485
#include <linux/serial.h>


#if defined(EM_AT91_PD)
#define CPU_AT91SAM9X5		1
#define CFG_USART_LIB		1
#define CFG_ENGPIO_LIB		1
#define CFG_DATETIM_LIB		1
#define CFG_HIGHTIMER_LIB	1
#define CFG_TIMER_LIB		1
#define CFG_WDG_LIB			1
#define CFG_MISCS_LIB		1
#define CFG_GPIO_LIB		1
#define CFG_DAC_LIB			1
#define CFG_ADC_LIB			1
#elif defined(EM_AM335X_PD)
#define CPU_TIAM335X		1
#define CFG_USART_LIB		1
#define CFG_GPIO_LIB		1
#define CFG_ENGPIO_LIB		1
#define CFG_DATETIM_LIB		1
#define CFG_TIMER_LIB		1
#define CFG_HIGHTIMER_LIB	1
#define CFG_WDG_LIB			1
#define CFG_MISCS_LIB		1
#define CFG_DAC_LIB			1
#define CFG_ADC_LIB			1
#elif defined(EM_LS1B_PD)
#define CPU_LOONGSON1B		1
#define CFG_USART_LIB		1
#define CFG_GPIO_LIB		1
#define CFG_ENGPIO_LIB		1
#define CFG_DATETIM_LIB		1
#define CFG_TIMER_LIB		1
#define CFG_HIGHTIMER_LIB	1
#define CFG_WDG_LIB			1
#define CFG_MISCS_LIB		1
#elif defined(EM_MPC83XX_PD)
#define CPU_MPC8315E		1
#define CFG_USART_LIB		1
#define CFG_GPIO_LIB		1
#define CFG_ENGPIO_LIB		1
#define CFG_DATETIM_LIB		1
#define CFG_TIMER_LIB		1
#define CFG_HIGHTIMER_LIB	1
#define CFG_WDG_LIB			1
#define CFG_MISCS_LIB		1
#endif

//�����붨��
#define ERROR_NOSUPPORT	-5		//��ǰ��֧�ָù���
#define ERROR_SYS		-4		//ϵͳAPI���ش���ϵͳ���÷��ش���
#define ERROR_INPARA	-3		//����������������Ϸ�����������С�ȣ�
#define ERROR_TIMEOUT	-2		//������ʱ
#define ERROR_FAIL		-1		//��������ԭ����
#define ERROR_OK		0		//�����ɹ�

//�����ֶ���
#define	EMFUTURE_IOCTL_BASE	'E'


//*-----------------------------------------------------------
/* IIO - useful set of util functionality
 *
 * Copyright (c) 2008 Jonathan Cameron
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

/* Made up value to limit allocation sizes */
#define IIO_MAX_NAME_LENGTH 30
#define FORMAT_SCAN_ELEMENTS_DIR "%s/scan_elements"
#define FORMAT_TYPE_FILE "%s_type"

extern const char *iio_dir;

/**
 * struct iio_channel_info - information about a given channel
 * @name: channel name
 * @generic_name: general name for channel type
 * @scale: scale factor to be applied for conversion to si units
 * @offset: offset to be applied for conversion to si units
 * @index: the channel index in the buffer output
 * @bytes: number of bytes occupied in buffer output
 * @mask: a bit mask for the raw output
 * @is_signed: is the raw value stored signed
 * @enabled: is this channel enabled
 **/
struct iio_channel_info {
	char *name;
	char *generic_name;
	float scale;
	float offset;
	unsigned index;
	unsigned bytes;
	unsigned bits_used;
	unsigned shift;
	unsigned mask;
	unsigned be;
	unsigned is_signed;
	unsigned enabled;
	unsigned location;
};

//*-----------------------------------------------------------



//�����Ƿֱ���Կ���֧�ֲ����ĺ�������
#ifdef CFG_USART_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//**               �����豸��ز���
//**
//**
//**  ע������ͬһ����������Ҫ�Զ�����ڽ�����ѯ����  Ϊ�ﵽ���ŵ�
//**      ����Ч��������ֱ��ʹ��select+read���ƣ�
//**      �������ÿ��ж�API����ReadCom�����������Կ�ʹ�á�
//*
//*-----------------------------------------------------------
//************************************************************

//���ڵ�Ĭ�ϲ���
#define COM_BAUDDEF		(115200)
#define	COM_DABITSDEF	(8)
#define COM_STBITSDEF	(1)
#define COM_PARITYDEF	('N')
#define COM_RTSCTSDEF	(0)
#define DEFCFGFORCOM 	"115200,8,1,N,0"

//��д���
typedef enum {
	RS232_MODE = 0,
	RS422_MODE,
	RS485_MODE
} com_type_t;

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  OpenCom
//* ��������:  �򿪲���ʼ������
//* ��ڲ���:
//*           -- dev:      - �����豸������/dev/ttyS1
//*           -- configs:  - ��ʼ��ͨѶ�����ַ���:������,����λ��ֹͣλ��У��λ��
//*                          ��:   "115200,8,1,N"����ΪNULL����ʹ��Ĭ�ϲ���("115200,8,1,N")
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int OpenCom(char *dev, int* fd, char *configs);


//*------------------------------------------------------------------------------------------------
//* ��������:  SetComCfg
//* ��������:  ���ô��ڲ���
//* ��ڲ���:
//*           -- fd:      - �����豸�ļ����
//*           -- configs: - ͨѶ�����ַ���:������,����λ��ֹͣλ��У��λ
//*                         ��:   "115200,8,1,N"
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int SetComCfg(int fd, char *configs);

//*------------------------------------------------------------------------------------------------
//* ��������:  SetComMode
//* ��������:  ���ô��ڹ���ģʽ(RS232/RS485/RS422)
//* ��ڲ���:
//*           -- dev:      - �����豸������/dev/ttyS1
//*           -- mode: - ���ڵĹ���ģʽ
//*                         ��:   RS232_MODE --- RS232
//*                         	      RS485_MODE --- RS485
//*                         	      RS422_MODE --- RS422
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int SetComMode(char *dev, com_type_t mode);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadCom
//* ��������:  ����������
//* ��ڲ���:
//*           -- fd:         - ָ�������豸�ľ��(��OpenCom()�ķ���ֵ)
//*           -- buffer:     - ��ȡ�����ݴ�Ż�����
//*           -- len:        - ��ȡ�����ݳ���(ָ��)��ͬʱ����ʵ�ʶ�ȡ��������Ŀ
//*           -- timesoutus: - ����������ȡ�ȴ��������λ: useconds(΢��)
//*                            ��timesoutus<=0��������ģʽ����ֱ������ָ�����ݸ�����������.
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1���������ֵ��Ż�����ʱ��һ��Ҫ�㹻���ָ�������ݸ��������������ɻ����������
//*           2������Ϊ������ģʽʱ��ָ���Ķ����ݸ�����Ҫǡ����������û����Ӧ�������ݣ�����̻�������
//*           3���ɹ���ȡ���ݴ�С���ɲ���len���أ�
//*------------------------------------------------------------------------------------------------
extern int ReadCom(int fd, char *buffer, unsigned int *len, int timesoutus);


//*------------------------------------------------------------------------------------------------
//* ��������:  WriteCom
//* ��������:  �򴮿�д����
//* ��ڲ���:
//*           -- fd:         - ָ�������豸�ľ��(��OpenCom()�ķ���ֵ)
//*           -- buffer:     - ��ȡ�����ݴ�Ż�����
//*           -- len:        - ��ȡ�����ݳ���(ָ��)��ͬʱ����ʵ�ʶ�ȡ��������Ŀ
//*           -- timesoutus: - ����������ȡ�ȴ��������λ: useconds(΢��)
//*                            ��timesoutus<=0��������ģʽ����ֱ������ָ�����ݸ�����������.
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1������Ϊ����дģʽʱ��ָ�������ݸ�����Ҫǡ�����������豸��ǰ����д�Ļ������ܻ���ɽ�������;
//*           2��ʵ��д�����ݴ�С���ɲ���len����;
//*------------------------------------------------------------------------------------------------
extern int WriteCom(int fd, char *buffer, unsigned int *len, int timesoutus);

//*------------------------------------------------------------------------------------------------
//* ��������:  CloseCom
//* ��������:  �رմ����豸
//* ��ڲ���:
//*           -- fd:  - ָ�������豸�ľ��(��OpenCom()�ķ���ֵ)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int CloseCom(int fd);

#ifdef __cplusplus
}
#endif
#endif



#ifdef CFG_FIFO_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//**               FIFO �豸��ز���
//**
//**
//**  ע������ͬһ����������Ҫ�Զ�����ڽ�����ѯ����  Ϊ�ﵽ���ŵ�
//**      ����Ч��������ֱ��ʹ��select+read���ƣ�
//**      �������ÿ��ж�API����ReadFifo�����������Կ�ʹ�á�
//*
//*-----------------------------------------------------------
//************************************************************

//FIFO ��Ĭ�ϲ���

#define PORTDEV		"/dev/fifo0"

//port��ioctlָ��
#define FIFO_IOC_MAGIC       'p'
#define FIFO_RESET_IOCFG   _IOW(FIFO_IOC_MAGIC, 1, unsigned char)



#ifdef __cplusplus
extern "C" {
#endif

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadFifo
//* ��������:  ��FIFO ����
//* ��ڲ���:
//*           -- fd:         - ָ��fifo �豸�ľ��(��Open()�ķ���ֵ)
//*           -- buffer:     - ��ȡ�����ݴ�Ż�����
//*           -- len:        - ��ȡ�����ݳ���(ָ��)��ͬʱ����ʵ�ʶ�ȡ��������Ŀ
//*           -- timesoutus: - ����������ȡ�ȴ��������λ: useconds(΢��)
//*                            ��timesoutus<=0��������ģʽ����ֱ������ָ�����ݸ�����������.
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1���������ֵ��Ż�����ʱ��һ��Ҫ�㹻���ָ�������ݸ��������������ɻ����������
//*           2������Ϊ������ģʽʱ��ָ���Ķ����ݸ�����Ҫǡ����������û����Ӧ�������ݣ�����̻�������
//*           3���ɹ���ȡ���ݴ�С���ɲ���len���أ�
//*------------------------------------------------------------------------------------------------
extern int ReadFifo(int fd, char *buffer, unsigned int *len, int timesoutus);


//*------------------------------------------------------------------------------------------------
//* ��������:  WriteFifo
//* ��������:  �򴮿�д����
//* ��ڲ���:
//*           -- fd:         - ָ��fifo �豸�ľ��(��Open()�ķ���ֵ)
//*           -- buffer:     - ��ȡ�����ݴ�Ż�����
//*           -- len:        - ��ȡ�����ݳ���(ָ��)��ͬʱ����ʵ�ʶ�ȡ��������Ŀ
//*           -- timesoutus: - ����������ȡ�ȴ��������λ: useconds(΢��)
//*                            ��timesoutus<=0��������ģʽ����ֱ������ָ�����ݸ�����������.
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1������Ϊ����дģʽʱ��ָ�������ݸ�����Ҫǡ�����������豸��ǰ����д�Ļ������ܻ���ɽ�������;
//*           2��ʵ��д�����ݴ�С���ɲ���len����;
//*------------------------------------------------------------------------------------------------
extern int WriteFifo(int fd, char *buffer, unsigned int *len, int timesoutus);


#ifdef __cplusplus
}
#endif
#endif


#ifdef CFG_ADC_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//*        ADC�豸��ز���
//*
//*-----------------------------------------------------------
//************************************************************

//�������ֵ���(16-bit)
typedef unsigned short cyg_adc_sample;

//ADC�豸��Ioctl����ӿڶ���
//����ʹ��
#define EM_SETADC_START_IOCFG      _IOWR(EMFUTURE_IOCTL_BASE, 20, int)
#define EM_SETADC_STOP_IOCFG       _IOWR(EMFUTURE_IOCTL_BASE, 21, int)
#define EM_SETADC_RATE_IOCFG       _IOWR(EMFUTURE_IOCTL_BASE, 22, int)
#define EM_SETADC_MODE_IOCFG       _IOWR(EMFUTURE_IOCTL_BASE, 23, int)
#define EM_SETADC_ENCHAN_IOCFG     _IOWR(EMFUTURE_IOCTL_BASE, 24, int)
#define EM_SETADC_DISCHAN_IOCFG    _IOWR(EMFUTURE_IOCTL_BASE, 25, int)
#define EM_GETADC_STAT_IOCFG       _IOWR(EMFUTURE_IOCTL_BASE, 26, int)
#define EM_GETADC_RATE_IOCFG       _IOWR(EMFUTURE_IOCTL_BASE, 27, int)
#define EM_GETADC_MODE_IOCFG       _IOWR(EMFUTURE_IOCTL_BASE, 28, int)
#define EM_SETADC_SWITCHKEY        _IOWR(EMFUTURE_IOCTL_BASE, 29, int)

//ADC�Ĳ���������ʽ����
//֧��ģʽ��Ӳ��ƽ̨���
//����ʹ��
#define EM_ADCTRIGER_ONESHOT 		_IOWR(EMFUTURE_IOCTL_BASE, 20, char)
#define EM_ADCTRIGER_SOFTPERIOD		_IOWR(EMFUTURE_IOCTL_BASE, 21, char)
#define EM_ADCTRIGER_HARDPERIOD		_IOWR(EMFUTURE_IOCTL_BASE, 22, char)

//AD����ֵ�任�ṹ(cyg_adc_sample -> float)
//����ʹ��
struct __sample_to_float {
	cyg_adc_sample ivalue;
	float fvalue;
};

#ifdef __cplusplus
extern "C" {
#endif

//*------------------------------------------------------------------------------------------------
//* ��������:  OpenAdc
//* ��������:  ��ADC�豸
//* ��ڲ���:
//*           -- dev:        -����ʹ�ã�Ϊ��֤�����ԣ�ָ��ΪNULL��
//*           -- keys:       -����ʹ�ã�Ϊ��֤�����ԣ�ָ��ΪNULL
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//*------------------------------------------------------------------------------------------------
extern int OpenAdc(char *dev, struct __sample_to_float *keys);


//*------------------------------------------------------------------------------------------------
//* ��������:  CloseAdc
//* �������� ���պ�����Ϊ��֤������
//* ��ڲ���:  ��
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int CloseAdc(void);

//*------------------------------------------------------------------------------------------------
//* ��������:  SetAdcCfg
//* ��������:  ��ADC�豸��������
//* ��ע    :  ����ʹ�ã�Ϊ��֤�����ԣ�Ϊ�պ���
//*------------------------------------------------------------------------------------------------
extern int SetAdcCfg(int chan, int cmd, char *buffer, unsigned int len);

//*------------------------------------------------------------------------------------------------
//* ��������:  GetAdcCfg
//* ��������:  ��ȡADC�豸�ĵ�ǰ����
//* ��ע    :  ����ʹ�ã�Ϊ��֤�����ԣ�Ϊ�պ���
//*------------------------------------------------------------------------------------------------
extern int GetAdcCfg(int chan, int cmd, char *buffer, unsigned int *len);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadAdcSampling
//* ��������:  ��ȡADC�Ĳ���ֵ
//* ��ڲ���:
//*           -- chan:     - ��ȡADC����ͨ����
//*                          ��chan>0,  ��ȡָ����ĳ��һͨ��
//*                          ��chan==0, ��ȡ����ͨ��
//*           -- buffer:   - ����ֵ������
//*           -- len:      - ��ȡ����������ͬʱ����ʵ�ʶ�ȡ�Ĳ���ֵ��Ŀ��
//*                          ��chan>0, len��ʾ��ͨ���Ĳ�������
//*                          ��chan=0, len��ʾÿһͨ���Ĳ�������
//*           -- type:     - ��������
//*                          ��type=0,���ȡΪת����ԭʼ����ֵ
//*                          ��type=1,���ȡת�����ģ����
//*           -- timesout�� - �ò�������ʹ�ã�Ϊ��֤�����ԣ�ָ��Ϊ0
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1���������ֵ��Ż�����ʱ��һ��Ҫ�㹻���ָ���Ĳ���ֵ������
//*              ��chan > 0ʱ�� ����ȡԭʼֵ���򻺳�����С >=  len * sizeof(cyg_adc_sample)
//*              				����ȡת����ĸ��������򻺳�����С >=  len * sizeof(float)
//*              ��chan = 0ʱ�� ����ȡԭʼֵ���򻺳�����С >=  ��Ч��ͨ���� * len * sizeof(cyg_adc_sample)
//*              				����ȡת����ĸ��������򻺳�����С >=  ��Ч��ͨ���� * len * sizeof(float)
//*------------------------------------------------------------------------------------------------
extern int ReadAdcSampling(unsigned char chan, void* buffer, unsigned int *len, unsigned char type, int timesout);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadAdcMinScale
//* ��������:  ��ȡADC��С������������ֵ
//* ��ڲ���:
//*           -- val:  - ADC��С�������������������(��λ:����mv)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��valΪADC��ǰ������С������ֵ��ģ��������λ:����mv��
//*------------------------------------------------------------------------------------------------
extern int ReadAdcMinScale(float *val);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadAdcValidChans
//* ��������:  ��ȡAdc��ǰ��Ч��ͨ����
//* ��ڲ���:
//*           -- val:  - ����Adc��ǰ��Чͨ����
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע      :  1��valΪ����Adc��Чͨ����
//*------------------------------------------------------------------------------------------------
extern int ReadAdcValidChans(int *val);

#ifdef __cplusplus
}
#endif
#endif

#ifdef CFG_DAC_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//**   DA�豸��ز���
//*
//*-----------------------------------------------------------
//************************************************************

//Ĭ��DA������(16-bit)
typedef unsigned short cyg_dac_out;

//DA����ֵ�任�ṹ(float -> cyg_dac_out)
//����ʹ��
struct __float_to_dacout {
	float fvalue;
	cyg_dac_out ivalue;
};

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  OpenDac
//* ��������:  ��DAC�豸
//* ��ڲ���:
//*           -- dev:        -����ʹ�ã�Ϊ��֤�����ԣ�ָ��ΪNULL��
//*           -- keys:       -����ʹ�ã�Ϊ��֤�����ԣ�ָ��ΪNULL
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//*------------------------------------------------------------------------------------------------
extern int OpenDac(char *dev, struct __float_to_dacout *keys);

//*------------------------------------------------------------------------------------------------
//* ��������:  SetDacCfg
//* ��������:  ����DAC��ͨ��ת������
//* ��ע    :  ����ʹ�ã�Ϊ��֤�����ԣ�Ϊ�պ���
//*------------------------------------------------------------------------------------------------
extern int SetDacCfg(int chan, struct __float_to_dacout *keys);


//*------------------------------------------------------------------------------------------------
//* ��������:  WriteDacRawVal
//* ��������:  ��DAC������ݣ�����Ϊת���������ֵ
//* ��ڲ���:
//*           -- chan:    - ����DACͨ����
//*                         ��chan>0, ��ֻ���ĳ��һͨ��
//*                         ��chan==0, ������ͨ�����
//*           -- intval:  - ���������
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��intvalΪ�������16λ������
//*------------------------------------------------------------------------------------------------
extern int WriteDacRawVal(int chan,unsigned short intval);


//*------------------------------------------------------------------------------------------------
//* ��������:  WriteDacfloatVal
//* ��������:  ��DAC������ݣ�����Ϊδ��ת����ģ����
//* ��ڲ���:
//*           -- chan:    - ����DACͨ����
//*                         ��chan>0, ��ֻ���ĳ��һͨ��
//*                         ��chan==0, ������ͨ�����
//*           -- val:  - ���ģ������������
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��valΪ�����ģ��������������
//*------------------------------------------------------------------------------------------------
extern int WriteDacfloatVal(int chan,float val);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadDacRawVal
//* ��������:  ��ȡDAC��ǰ���ֵ������Ϊת���������ֵ
//* ��ڲ���:
//*           -- chan:    - ����DACͨ����
//*                         ��chan>0, ��ָĳ��һͨ��
//*                         ��chan==0, ��������ͨ�����ֵ
//*           -- intval:  - ��ǰ���������
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��intvalΪ�������16λ������
//*------------------------------------------------------------------------------------------------
extern int ReadDacRawVal(int chan,unsigned short *intval);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadDacfloatVal
//* ��������:  ��ȡDAC��ǰ���ֵ������Ϊת����ĸ�����ֵ
//* ��ڲ���:
//*           -- chan:    - ����DACͨ����
//*                         ��chan>0, ��ֻ���ĳ��һͨ��
//*                         ��chan==0, ������ͨ�����
//*           -- val:  - ��ǰ���ģ������������(��λ:����mv)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��valΪ��ǰ�����ģ��������λ:����mv��
//*------------------------------------------------------------------------------------------------
extern int ReadDacfloatVal(int chan,float *val);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadDacMinScale
//* ��������:  ��ȡDAC��С�����������ֵ
//* ��ڲ���:
//*           -- val:  - DAC��С�������������������(��λ:����mv)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��valΪ��ǰ�������С���������ֵ��ģ��������λ:����mv��
//*------------------------------------------------------------------------------------------------
extern int ReadDacMinScale(float *val);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadDacValidChans
//* ��������:  ��ȡDAC��ǰ��Чͨ����
//* ��ڲ���:
//*           -- val:  - ����DAC��ǰ��Чͨ����
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע      :  1��valΪ����DAC��Чͨ����
//*------------------------------------------------------------------------------------------------
extern int ReadDacValidChans(int *val);

//*------------------------------------------------------------------------------------------------
//* ��������:  CloseDac
//* �������� ���պ�����Ϊ��֤������
//* ��ڲ���:  ��
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int CloseDac(void);
#ifdef __cplusplus
}
#endif
#endif

#ifdef CFG_ENGPIO_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//**   GPIO�豸��ز���
//*
//*-----------------------------------------------------------
//************************************************************
#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  SetEnIOOutState
//* ��������:  ����GPIOΪ��ƽ�����������IO�����״̬
//* ��ڲ���:
//*           -- gpio:    - IO�˿ں�
//*           -- state:   - IO���״̬(0:Low/1:High)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :   ��
//*------------------------------------------------------------------------------------------------
extern int SetEnIOOutState(int gpio, int state);

//*------------------------------------------------------------------------------------------------
//* ��������:  GetEnIOOutState
//* ��������:  ��ȡΪ��ƽ�������IO�ĵ�ǰ���״̬
//* ��ڲ���:
//*           -- gpio:     - IO�˿ں�
//*           -- curstate: - ����IO�ĵ�ǰ���״̬(0:Low/1:High)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :   ��
//*------------------------------------------------------------------------------------------------
extern int GetEnIOOutState(int gpio,int *curstate);

//*------------------------------------------------------------------------------------------------
//* ��������:  SetEnIOPulseState
//* ��������:  ����GPIOΪ�����������ָ���������
//* ��ڲ���:
//*           -- gpio:       - IO�˿ں�
//*           -- hitime10us: - ָ������ߵ�ƽ���ʱ�䣬��λ10us
//*           -- period10us: - ָ������������ڣ���λ��10us
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��hitime10us��period10usָ����λ��Ϊ10us
//*           2������ֵ����hitime10us==0����һֱ����͵�ƽ
//*           3������ֵ����hitime10us!=0,period10us==0,��ֻ���һ��hitime10usʱ��ĸߵ�ƽ����
//*           4������ֵ����hitime10us>=period10us>0,��һֱ����ߵ�ƽ
//*------------------------------------------------------------------------------------------------
extern int SetEnIOPulseState(int gpio, int hitime10us, int period10us);

//*------------------------------------------------------------------------------------------------
//* ��������:  GetEnIOPulseCfg
//* ��������:  ��ȡΪ�����������IO�ĵ�ǰ�������
//* ��ڲ���:
//*           -- gpio:       - IO�˿ں�
//*           -- hitime10us: - ������������ĸߵ�ƽ���ʱ���������λ10us
//*           -- period10us: - ������������ĵ�ǰ������ڲ�������λ��10us
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��hitime10us��period10usָ����λ��Ϊ10us
//*------------------------------------------------------------------------------------------------
extern int GetEnIOPulseCfg(int gpio, int *hitime10us, int *period10us);

//*------------------------------------------------------------------------------------------------
//* ��������:  GetEnIOInValue
//* ��������:  ��ȡΪ��ƽ��������IO�ĵ�ǰ����ֵ
//* ��ڲ���:
//*           -- gpio:     - IO�˿ں�
//*           -- curval:   - ����IO�ĵ�ǰ����״̬(0:Low/1:High)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :   ��
//*------------------------------------------------------------------------------------------------
extern int GetEnIOInValue(int gpio,int *curval);


//*------------------------------------------------------------------------------------------------
//* ��������:  SetEnIOEventCfg
//* ��������:  ����GPIOΪ�¼�IO���룬��ָ���¼���������
//* ��ڲ���:
//*           -- gpio:       - IO�˿ں�
//*           -- func:     	 - IO�¼��ص�ִ�к���������⵽IO�¼��������ú����ᱻ����ִ��
//*           -- funcmode:   - �ص�������Ӧģʽ����ͬʱ��⵽IO�¼��������ص�����ִ��ģʽ
//*                            0: ִ��һ�Σ�1��ִ�д������⵽IO�¼�������ͬ
//*           -- trigermode: - IO�¼�����
//*                            0: HI_to_LOW��1��LOW_to_HI��2��ALL
//*           -- filter10us:   - �����˲���������λ��10us,����Ч��ƽ����ʱ��
//*                          ����ƽ����ʱ��<filter10us,��ٶ�Ϊ����,�¼�����Ӧ
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1��funcΪ�ص�����ָ�룬��ΪNULL,��������¼�IO��
//*              ��˵���Ҫ���¼�IO����ΪΪ��������IOʱ����ָ��funcΪNULL������SetEventIOCfg����
//*           2��func�ص������Ĳ�����Ϊ��ǰgpio
//*------------------------------------------------------------------------------------------------
extern int SetEnIOEventCfg(int gpio,void(*func)(int gpio),int funcmode,int trigermode,int filter10us);


//*------------------------------------------------------------------------------------------------
//* ��������:  GetEnIOEventCfg
//* ��������:  ��ȡ��������Ϊ�¼�IO�Ĳ���
//* ��ڲ���:
//*           -- gpio:       - IO�˿ں�
//*           -- trigermode: - ����IO�¼��������
//*                          0: HI_to_LOW��1��LOW_to_HI��2��ALL
//*           -- filter10us: - �������������������λ��10us
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ������ERROR_INPARA����˵���ÿ���û���趨Ϊ�¼�IO��������������
//*------------------------------------------------------------------------------------------------
extern int GetEnIOEventCfg(int gpio,int *trigermode,int *filter10us);
#ifdef __cplusplus
}
#endif
#endif


#ifdef CFG_DATETIM_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//**   RTCʱ��������ز���
//*
//*-----------------------------------------------------------
//************************************************************

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  GetRtcTime
//* ��������:  ��ȡRTCʱ��
//* ��ڲ���:
//*           -- data:        - ���ص�ǰRTCʱ��
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1������data��ʱ��������£�
//*              data[0] --- year
//*              data[1] --- month
//*              data[2] --- day
//*              data[3] --- hour
//*              data[4] --- minute
//*              data[5] --- second
//*              data[6] --- wday
//*------------------------------------------------------------------------------------------------
extern int GetRtcTime(unsigned short* data);

//*------------------------------------------------------------------------------------------------
//* ��������:  SetRtcTime
//* ��������:  ����RTCʱ��
//* ��ڲ���:
//*           -- configs:     - ���趨ʱ�����
//*           -- syn:         - ͬ��ϵͳʱ��(0-NO/1-Yes)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1������configs��ʱ��������£�
//*              configs[0] --- year
//*              configs[1] --- month
//*              configs[2] --- day
//*              configs[3] --- hour
//*              configs[4] --- minute
//*              configs[5] --- second
//*------------------------------------------------------------------------------------------------
extern int SetRtcTime (unsigned short* configs, unsigned char syn);
#ifdef __cplusplus
}
#endif
#endif


#ifdef CFG_TIMER_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//*               ��ʱ����ز���
//*
//*-----------------------------------------------------------
//************************************************************

//��ʱ��ִ�к���ָ�붨��
typedef void (*FUNC) (int sig);

//��ʱ������ģʽ
#define	EMTIMER_SINSHOT_MODE 	    _IOWR(EMFUTURE_IOCTL_BASE, 40, int) // ���δ���
#define	EMTIMER_PERSHOT_MODE 		_IOWR(EMFUTURE_IOCTL_BASE, 41, int) // �����Դ���

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  CreateTimer
//* ��������:  �����û���ʱ��
//* ��ڲ���:
//*           -- handler:    - ��ʱ���Ĵ����������
//*           -- args:       - ���ݸ����������Ĳ���
//*           -- usecs:      - ��ʱ���Ķ�ʱ�������λ: useconds(΢��)
//*           -- mode :      - ��ʱ���Ĵ���ģʽ��
//*                            EMTIMER_SINSHOT_MODE 	    ���� ���δ���
//*                            EMTIMER_PERSHOT_MODE 		���� �����Դ���
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int CreateTimer(FUNC handler, void *args, unsigned int usecs, int mode);

//*------------------------------------------------------------------------------------------------
//* ��������:  UserusDelayms
//* ��������:  �û�����ʱ��������Ծ�ȷ�ĺ��뼶��ʱ��
//* ��ڲ���:
//*           -- millsecs:    - ��ʱ��С(��λ������ms)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int UserusDelayms(unsigned int millsecs);
#ifdef __cplusplus
}
#endif
#endif

#ifdef CFG_HIGHTIMER_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//*               �߾��ȶ�ʱ����ز���
//*               ����ҪӲ��֧�֣�
//*-----------------------------------------------------------
//************************************************************

//��ʱ��ִ�к���ָ�붨��
typedef void (*HFUNC) (int sig, siginfo_t *extra, void *args);

//�߾��ȶ�ʱ������ģʽ
#define	EMTIMER_HIGHSINSHOT_MODE 	    _IOWR(EMFUTURE_IOCTL_BASE, 42, int) // ���δ���
#define	EMTIMER_HIGHSPERSHOT_MODE 		_IOWR(EMFUTURE_IOCTL_BASE, 43, int) // �����Դ���

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  GreateHighResTimer
//* ��������:  �����û��߾��ȶ�ʱ��
//* ��ڲ���:
//*           -- handler:    - ��ʱ���Ĵ����������
//*           -- timerid:    - �߾��ȶ�ʱ�����ָ�루��ɾ����ʱ��ʱʹ�ã�
//*           -- nsecs:      - ��ʱ���Ķ�ʱ�������λ: neconds(����)
//*           -- mode :      - ��ʱ���Ĵ���ģʽ��
//*                            EMTIMER_HIGHSINSHOT_MODE 	  ���� ���δ���
//*                            EMTIMER_HIGHSPERSHOT_MODE      ���� �����Դ���
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1���ú�����Ҫƽ̨֧�ָ߾��ȶ�ʱ������
//*           2���߾��ȶ�ʱ��ʹ�������Ҫ���ö�ʱ��ɾ������������timerid����������ܻ�Ӱ��ϵͳЧ��
//*           3������Ŀʹ�ø߾��ȶ�ʱ������Ҫ�ڱ������ӳ���ʱָ����-lrt����
//*------------------------------------------------------------------------------------------------
extern int GreateHighResTimer(HFUNC handler, timer_t *timerid, unsigned int nsecs,int mode);

//*------------------------------------------------------------------------------------------------
//* ��������:  DelHighResTimer
//* ��������:  ɾ���߾��ȶ�ʱ��
//* ��ڲ���:
//*           -- timerid:    - �߾��ȶ�ʱ�����ָ�루��GreateHighResTimer���أ�
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1������������GreateHighResTimer���صĶ�ʱ�����
//*------------------------------------------------------------------------------------------------
extern int DelHighResTimer(timer_t timerid);

//*------------------------------------------------------------------------------------------------
//* ��������:  UserusDelayus
//* ��������:  �û�����ʱ��������Ծ�ȷ��΢�뼶��ʱ��
//* ��ڲ���:
//*           -- usecs:    - ��ʱ��С(��λ��΢��us)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int UserusDelayus(unsigned int usecs);

#ifdef __cplusplus
}
#endif
#endif

#ifdef CFG_PORT_LIB
//************************************************************
//************************************************************
//*
//**   �˿��豸��ز���
//*
//************************************************************
//************************************************************

//��д���
typedef enum {
	PORT_WIDTH_8 = 0,
	PORT_WIDTH_16,
	PORT_WIDTH_32
} port_wid_t;

//�Ϸ���BANK���
typedef enum {
	PORT_BANK_2 = 0,
	PORT_BANK_4,
	PORT_BANK_5,
	PORT_BANK_6,
	PORT_BANK_7,
} port_bank_t;

//BANK����
typedef struct {
	port_bank_t bank;
	port_wid_t width;
	short reserved;
} port_attr_t;

//
typedef struct {
	int regAddr;
	int regData;
} port_data_t;

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  OpenPortDev
//* ��������:  �򿪶˿��豸
//* ��ڲ���:
//*           -- dev:         - �˿��豸���ƣ���/dev/port5
//*           -- fd:          - �����ļ����
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int OpenPortDev(char *dev,int* fd);


//*------------------------------------------------------------------------------------------------
//* ��������:  ClosePortDev
//* ��������:  �رն˿��豸
//* ��ڲ���:
//*           -- fd:          - �ļ����
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int ClosePortDev(int fd);


//*------------------------------------------------------------------------------------------------
//* ��������:  SetPortCfg
//* ��������:  ���ö˿�����
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- bank:        - �˿ڵ�Bank����
//*           -- width:       - �˿ڵĶ�д���(8/16/32),��Ӳ��ƽ̨�й�
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  1���˿ڵ�Bank����дwidth��Ӳ�����þ���
//*------------------------------------------------------------------------------------------------
extern int SetPortCfg(int fd, unsigned char bank, unsigned char width);


//*------------------------------------------------------------------------------------------------
//* ��������:  GetPortCfg
//* ��������:  ��ȡ�˿�����
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- bank:        - ���ض˿ڵ�Bank����
//*           -- width:       - ���ض˿ڵĶ�д���(8/16/32)
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int GetPortCfg(int fd, unsigned char *bank, unsigned char *width);


//*------------------------------------------------------------------------------------------------
//* ��������:  InPortw
//* ��������:  16-bit���˿�
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- addr:        - ��ȡ�Ķ˿�ƫ���ַ
//*
//* ����ֵ    :  �ɹ����ض�ȡֵ�����򷵻�-1
//* ��ע         :  1��addr����2�ֽڶ���
//*------------------------------------------------------------------------------------------------
extern inline short InPortw(int fd, unsigned long addr);

//*------------------------------------------------------------------------------------------------
//* ��������:  OutPortw
//* ��������:  16-bitд�˿�
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- addr:        - �˿ڵ�ƫ���ַ
//*           -- data:        - ��ֵ
//*
//* ����ֵ    :  �ɹ�����ERROR_OK�����򷵻�ERROR_FAIL
//* ��ע         :  1��addr����2�ֽڶ���
//*------------------------------------------------------------------------------------------------
extern inline int OutPortw(int fd, unsigned long addr, short data);


//*------------------------------------------------------------------------------------------------
//* ��������:  InPortl
//* ��������:  32-bit���˿�
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- addr:        - �˿ڵ�ƫ���ַ
//*
//* ����ֵ    :  �ɹ����ض�ȡֵ�����򷵻�-1
//* ��ע         :  1��addr����4�ֽڶ���
//*------------------------------------------------------------------------------------------------
extern inline int InPortl(int fd, unsigned long addr);


//*------------------------------------------------------------------------------------------------
//* ��������:  OutPortl
//* ��������:  32-bitд�˿�
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- addr:        - �˿ڵ�ƫ���ַ
//*           -- data:        - ��ֵ
//*
//* ����ֵ    :  �ɹ�����ERROR_OK�����򷵻�ERROR_FAIL
//* ��ע         :  1��addr����4�ֽڶ���
//*------------------------------------------------------------------------------------------------
extern inline int OutPortl(int fd, unsigned long addr, int data);


//*------------------------------------------------------------------------------------------------
//* ��������:  InPortb
//* ��������:  8-bit���˿�
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- addr:        - ��ȡ�Ķ˿�ƫ���ַ
//*
//* ����ֵ    :  �ɹ����ض�ȡֵ�����򷵻�-1
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern inline char InPortb(int fd, unsigned long addr);

//*------------------------------------------------------------------------------------------------
//* ��������:  OutPortb
//* ��������:  8-bitд�˿�
//* ��ڲ���:
//*           -- fd:          - �ļ����(��OpenPortDev()�ķ���ֵ)
//*           -- addr:        - �˿ڵ�ƫ���ַ
//*           -- data:        - ��ֵ
//*
//* ����ֵ    :  �ɹ�����ERROR_OK�����򷵻�ERROR_FAIL
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern inline int OutPortb(int fd, unsigned long addr, char data);
#ifdef __cplusplus
}
#endif
#endif


#ifdef CFG_WDT_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//*                ���Ź��豸��ز���
//*
//*-----------------------------------------------------------
//************************************************************
#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  EnableWtd
//* ��������:  ʹ�ܿ��Ź��豸
//* ��ڲ���:
//*           -- TimeOut:     - ָ�����Ź������ι��������룩
//*
//* ����ֵ��      �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:     ��
//*------------------------------------------------------------------------------------------------
extern int EnableWtd(int TimeOut);

//*------------------------------------------------------------------------------------------------
//* ��������:  FeedWtd
//* ��������:  ι������
//* ��ڲ���:  ��
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int FeedWtd(void);

//*------------------------------------------------------------------------------------------------
//* ��������:  DisableWtd
//* ��������:  �رտ��Ź�
//* ��ڲ���:  ��
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int DisableWtd(void);
#ifdef __cplusplus
}
#endif
#endif

#ifdef CFG_KBD_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//*                �����豸��ز���
//*
//*-----------------------------------------------------------
//************************************************************

//���ֵ���/���µ���ֵ����������������
#define UP_THRESHOLD_DOWN	(0x80)

//
#define DOWN_KEY			(0x0)
#define UP_KEY				(0x1)

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  OpenKbd
//* ��������:  �򿪼����豸
//* ��ڲ���:
//*           -- dev:     - �����豸�ļ�����/dev/kbd0
//*           -- fd:      - �����ļ����
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int OpenKbd(char *dev,int *fd);


//*------------------------------------------------------------------------------------------------
//* ��������:  FlushKbd
//* ��������:  ��ռ��̻�����
//* ��ڲ���:
//*           -- fd:      - �ļ����
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int FlushKbd(int fd);

//*------------------------------------------------------------------------------------------------
//* ��������:  GetKeyValue
//* ��������:  ��ȡ����ֵ����Ӧ״̬
//* ��ڲ���:
//*           -- fd:         - �ļ����
//*           -- keyValue:   - ���ذ���ֵ
//*           -- status:     - ���ذ����ĵ�ǰ����״̬(1��UP/0��DOWN)
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int GetKeyValue(int fd, unsigned short *keyValue, unsigned char *status);

//*------------------------------------------------------------------------------------------------
//* ��������:  CloseKbd
//* ��������:  �رռ����豸���ͷ������Դ
//* ��ڲ���:
//*           -- fd:         - �ļ��������OpenKbd����
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int CloseKbd(int fd);
#ifdef __cplusplus
}
#endif
#endif


#ifdef CFG_MISCS_LIB
//************************************************************
//*-----------------------------------------------------------
//*
//*               ID����оƬ�豸��ز���
//*						��Ӳ��֧��
//*
//*-----------------------------------------------------------
//************************************************************
#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  ReadSerialID
//* ��������:  ��ȡ����ID(DI)�豸��ΨһID��
//* ��ڲ���:
//*           -- id:        - ����ID��������
//*           -- len:       - ID����ĳ���
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int ReadSerialID(unsigned char* id, unsigned char* len);
#ifdef __cplusplus
}
#endif

//************************************************************
//*-----------------------------------------------------------
//*
//*               ��ʪ�ȴ����豸��ز���
//*
//*-----------------------------------------------------------
//************************************************************
/******   TM control cmd   *********/
#define	EM_SET_SENSOR_RES 	    _IOWR(EMFUTURE_IOCTL_BASE, 30, int) // ���ò����ֱ���
#define	EM_GET_SENSOR_RES 		_IOWR(EMFUTURE_IOCTL_BASE, 31, int) //  ��ȡ��ǰ�����ֱ���

#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  SetSensorCfg
//* ��������:  �����¶ȴ������Ĺ�������
//* ��ڲ���:
//*           -- cmd:        - �û�������
//*                            EM_SET_SENSOR_RES 	    ���� ���ò����ֱ���
//*                            EM_GET_SENSOR_RES 		���� ��ȡ��ǰ�����ֱ���
//*           -- arg:        - ����/�������ò���
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע         :  ��
//*------------------------------------------------------------------------------------------------
extern int SetSensorCfg(int cmd, unsigned short* arg);

//*------------------------------------------------------------------------------------------------
//* ��������:  ReadSensorVal
//* ��������:  ��ȡ���������¶�ֵ
//* ��ڲ���:
//*           -- value:        - �����¶�ֵ
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע      :  ��ȡֵ=ʵ��ֵ*�Ŵ���(10000)
//*------------------------------------------------------------------------------------------------
extern int ReadSensorVal(unsigned int* value);

#ifdef __cplusplus
}
#endif

//************************************************************
//*-----------------------------------------------------------
//*
//**   ������������LED�豸��ز���
//*
//*-----------------------------------------------------------
//************************************************************

#ifdef __cplusplus
extern "C" {
#endif

//*------------------------------------------------------------------------------------------------
//* ��������:  IoctlBuzzer
//* ��������:  �����������豸
//* ��ڲ���:
//*           -- ulaston10ms:   - ���г���ʱ�䣬��λ10���롣
//*           -- ulastoff10ms:  - ����ر�ʱ�䣬��λ10���롣
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:  	  1��ulaston10ms��ulastoff10ms�ĵ�λ��Ϊ10���룻
//*           2��ulaston10ms��ulastoff10ms��ȡֵ����
//*			     ��ulaston10ms = 0 ----  �رշ�����
//*				 ��ulaston10ms = 0xFFFF ---- ��������������
//*				 ��ulaston10ms >0 && ulastoff10ms=0  ------ ����������1�Σ�����ʱ��ulaston10ms
//*				 ��ulaston10ms >0 && ulastoff10ms>0  ------ ����������������
//*				 											����ʱ��ulaston10ms������(ulaston10ms+ulastoff10ms)
//*------------------------------------------------------------------------------------------------
extern int IoctlBuzzer(unsigned short ulaston10ms, unsigned short ulastoff10ms);

//*------------------------------------------------------------------------------------------------
//* ��������:  IoctlRunLed
//* ��������:  ��������ָʾ��(Running LED)
//* ��ڲ���:
//*           -- ulaston10ms:   - ����ָʾ��������ʱ�䣬��λ10���롣
//*           -- ulastoff10ms:  - ����ָʾ�ƹر�ʱ�䣬��λ10���롣
//*
//* ����ֵ    :  �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:  	  1��ulaston10ms��ulastoff10ms�ĵ�λ��Ϊ10���룻
//*           2��ulaston10ms��ulastoff10ms��ȡֵ����
//*			     ��ulaston10ms = 0 ----  �ر�����ָʾ��
//*				 ��ulaston10ms = 0xFFFF ---- ����ָʾ�Ƴ�����
//*				 ��ulaston10ms >0 && ulastoff10ms=0  ------ ����ָʾ����˸1�Σ�����ʱ��ulaston10ms
//*				 ��ulaston10ms >0 && ulastoff10ms>0  ------ ����ָʾ����������˸
//*				 											������ʱ��ulaston10ms������(ulaston10ms+ulastoff10ms)
//*------------------------------------------------------------------------------------------------
extern int IoctlRunLed(unsigned short ulaston10ms, unsigned short ulastoff10ms);

#ifdef __cplusplus
}
#endif
#endif


#ifdef CFG_GPIO_LIB
//************************************************************
//************************************************************
//*
//**   �û����GPIO��������
//*
//************************************************************
//************************************************************


//GPIO�˿ڶ���
#define PORTA_GPIO(x)	x
#define PORT0_GPIO(x)	PORTA_GPIO(x)
#define PORTB_GPIO(x)	(32+x)
#define PORT1_GPIO(x)	PORTB_GPIO(x)
#define PORTC_GPIO(x)	(64+x)
#define PORT2_GPIO(x)	PORTC_GPIO(x)
#define PORTD_GPIO(x)	(96+x)
#define PORT3_GPIO(x)	PORTD_GPIO(x)
#define PORTE_GPIO(x)	(128+x)
#define PORT4_GPIO(x)	PORTE_GPIO(x)
#define PORTF_GPIO(x)	(160+x)
#define PORT5_GPIO(x)	PORTF_GPIO(x)

//�˿�����
typedef enum {
	PIO_MODE_OUT = 0,
	PIO_MODE_IN,
} gpio_mode_t;


#ifdef __cplusplus
extern "C" {
#endif
//*------------------------------------------------------------------------------------------------
//* ��������:  SetGPIOCfg
//* ��������:  ����GPIO�Ķ˿�����
//* ��ڲ���:
//*           -- gpio:         - gpio�˿�
//*           -- attr:         - ָ��GPIO�����ԣ�ģʽ��
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int SetGPIOCfg(int gpio, gpio_mode_t attr);

//*------------------------------------------------------------------------------------------------
//* ��������:  GPIOOutValue
//* ��������:  GPIO�˿����
//* ��ڲ���:
//*           -- gpio:         - gpio�˿�
//*           -- val:          - ָ�����״̬
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int GPIOOutValue(int gpio, int val);

//*------------------------------------------------------------------------------------------------
//* ��������:  GPIOInValue
//* ��������:  ��GPIO�˿�����
//* ��ڲ���:
//*           -- gpio:         - gpio�˿�
//*           -- state:        - ����GPIO����״̬
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int GPIOInValue(int gpio,int *state);

//*------------------------------------------------------------------------------------------------
//* ��������:  FreeGPIOCfg
//* ��������:  �ͷ�GPIO����
//* ��ڲ���:
//*           -- gpio:         - gpio�˿�
//*
//* ����ֵ��        �ɹ�����ERROR_OK(==0)��ʧ�ܷ���(ERROR_INPARA/ERROR_TIMEOUT/ERROR_SYS/ERROR_NOSUPPORT/ERROR_FAIL)
//* ��ע:      ��
//*------------------------------------------------------------------------------------------------
extern int FreeGPIOCfg(int gpio);

#ifdef __cplusplus
}
#endif
#endif

//************************************************************
//************************************************************
//*
//**   ������������
//*
//************************************************************
//************************************************************
#ifdef __cplusplus
extern "C" {
#endif

#if defined(CPU_AT91SAM9263) || defined(CPU_AT91SAM9260) || defined(CPU_AT91SAM9G20) \
		|| defined(CPU_AT91SAM9G45) || defined(CPU_AT91SAM9X5) || defined(CPU_TIAM335X)

extern inline unsigned short _inw(unsigned long addr);
extern inline void _outw(unsigned long addr, unsigned short dat);
extern inline unsigned long _inl(unsigned long addr);
extern inline void _outl(unsigned long addr, unsigned long dat);
extern inline unsigned char _inb(unsigned long addr);
extern inline void _outb(unsigned long addr, unsigned char dat);

#endif

extern void _nops(unsigned long nums);
extern int _Readn(int fd, void *buf, int n);
extern int _Writen(int fd, const void *buf, int n);

#ifdef __cplusplus
}
#endif

#define DEBUG
#ifdef DEBUG
#define TRACE(str, args...)	printf(str, ## args)
#else
#define TRACE(str, args...)
#endif

#endif /* LIBS_EMFUTURE_ODM_H_ */
