/*
 *  Copyright (C) 2010 -2011  Espressif System
 *
 */

#ifndef __UART_H__
#define __UART_H__

typedef enum {
    BIT_RATE_300     = 300,
    BIT_RATE_600     = 600,
    BIT_RATE_1200    = 1200,
    BIT_RATE_2400    = 2400,
    BIT_RATE_4800    = 4800,
    BIT_RATE_9600    = 9600,
    BIT_RATE_19200   = 19200,
    BIT_RATE_38400   = 38400,
    BIT_RATE_57600   = 57600,
    BIT_RATE_74880   = 74880,
    BIT_RATE_115200  = 115200,
    BIT_RATE_230400  = 230400,
    BIT_RATE_460800  = 460800,
    BIT_RATE_921600  = 921600,
    BIT_RATE_1843200 = 1843200,
    BIT_RATE_3686400 = 3686400,
} UART_BautRate; //you can add any rate you need in this range

typedef enum {
    UART0 = 0x0,
    UART1 = 0x1,
} UART_Port;

typedef enum {
    UART_WordLength_5b = 0x0,
    UART_WordLength_6b = 0x1,
    UART_WordLength_7b = 0x2,
    UART_WordLength_8b = 0x3
} UART_WordLength;

typedef enum {
    USART_StopBits_1   = 0x1,
    USART_StopBits_1_5 = 0x2,
    USART_StopBits_2   = 0x3,
} UART_StopBits;

typedef enum {
    USART_Parity_None = 0x2,
    USART_Parity_Even = 0x0,
    USART_Parity_Odd  = 0x1
} UART_ParityMode;

typedef enum {
    PARITY_DIS = 0x0,
    PARITY_EN  = 0x2
} UartExistParity;

typedef enum {
    USART_HardwareFlowControl_None    = 0x0,
    USART_HardwareFlowControl_RTS     = 0x1,
    USART_HardwareFlowControl_CTS     = 0x2,
    USART_HardwareFlowControl_CTS_RTS = 0x3
} UART_HwFlowCtrl;

typedef enum {
    UART_None_Inverse = 0x0,
    UART_Rxd_Inverse  = UART_RXD_INV,
    UART_CTS_Inverse  = UART_CTS_INV,
    UART_Txd_Inverse  = UART_TXD_INV,
    UART_RTS_Inverse  = UART_RTS_INV,
} UART_LineLevelInverse;

/*************************************************************************************
��������: ��ӡ���ں�����
�������: uart_no   ���ں�
�������: ��
�� �� ֵ: ��
��    ע: Ĭ�ϴ�ӡ������74880
          ��uart_io==UART1�����ӡ��Ϣ��IO2�˿����
          tysdkĬ�����еĴ�ӡ��Ϣ��IO2�˿�������û��ڱ�дӦ�ô���ʱ��ͨ���ýӿ�������
*************************************************************************************/
void print_port_init(UART_Port uart_no);

/*************************************************************************************
��������: ��ӡ���ڲ�������
�������: uart_no    ���ں�
          bit_rate   ������(300-3686400)
          data_bits  ����λ
          parity     ��żУ��λ
          stop_bits  ֹͣλ
�������: ��
�� �� ֵ: ��
��    ע: ��uart_io==UART1�����ӡ��Ϣ��IO2�˿����
          tysdkĬ�����еĴ�ӡ��Ϣ��IO2�˿�������û��ڱ�дӦ�ô���ʱ��ͨ���ýӿ�������
*************************************************************************************/
void print_port_full_init(UART_Port uart_no,UART_BautRate bit_rate,UART_WordLength data_bits,\
    UART_ParityMode parity,UART_StopBits stop_bits);

/*************************************************************************************
��������: UART0���ڳ�ʼ������
�������: bit_rate   ������(300-3686400)
          data_bits  ����λ
          parity     ��żУ��λ
          stop_bits  ֹͣλ
�������: ��
�� �� ֵ: ��
��    ע: ������øýӿڳ�ʼ��UART0��I015��·�������⴦������ɼ�����ԭ���ĵ�
          ���øö˿ڽ��д���ͨѶ�ĺô��ǣ����Ա���8266������Ĭ����Ϣ������û����ư�����ݸ���
*************************************************************************************/
void user_uart_full_init(UART_BautRate bit_rate,UART_WordLength data_bits,\
    UART_ParityMode parity,UART_StopBits stop_bits);

/*************************************************************************************
��������: UART0���ڳ�ʼ������
�������: bit_rate   ������(300-3686400)
          data_bits  ����λ
          parity     ��żУ��λ
          stop_bits  ֹͣλ
�������: ��
�� �� ֵ: ��
��    ע: esp8266Ĭ��UART0���շ�IO�˿�
*************************************************************************************/
void user_uart_raw_full_init(UART_BautRate bit_rate,UART_WordLength data_bits,\
    UART_ParityMode parity,UART_StopBits stop_bits);

/*************************************************************************************
��������: UART0���ڳ�ʼ��,esp8266���շ��˿�����ΪI015:TX IO13:RX
�������: bit_rate   ������(300-3686400)
�������: ��
�� �� ֵ: ��
��    ע: Ĭ��data_bits==8,parity==��,stop_bits==0
          ������øýӿڳ�ʼ��UART0��I015��·�������⴦������ɼ�����ԭ���ĵ�
          ���øö˿ڽ��д���ͨѶ�ĺô��ǣ����Ա���8266������Ĭ����Ϣ������û����ư�����ݸ���
*************************************************************************************/
void user_uart_init(UART_BautRate bit_rate);

/*************************************************************************************
��������: UART0���ڳ�ʼ��,esp8266���շ��˿�����ΪIO3:RX IO1:TX
�������: bit_rate   ������(300-3686400)
�������: ��
�� �� ֵ: ��
��    ע: Ĭ��data_bits==8,parity==��,stop_bits==0
          esp8266Ĭ��UART0���շ�IO�˿�
*************************************************************************************/
void user_uart_raw_init(UART_BautRate bit_rate); // no io swap mode

/*************************************************************************************
��������: �����ڻ������ݴ�С
�������: ��
�������: ��
�� �� ֵ: �����Ļ������ݳ���
��    ע: ��
*************************************************************************************/
uint16 user_uart_read_size(void);

/*************************************************************************************
��������: ��ȡ��������
�������: out_len �����С
�������: out ��ȡ������
�� �� ֵ: ��ȡ�����ݳ���
��    ע: ��
*************************************************************************************/
uint16 user_uart_read_data(uint8 *out,uint16 out_len);

/*************************************************************************************
��������: д��������
�������: in Ҫд������
          in_len ���ݳ���
�������: ��
�� �� ֵ: ��
��    ע: ��
*************************************************************************************/
void user_uart_write_data(uint8 *in,uint16 in_len);

#endif
