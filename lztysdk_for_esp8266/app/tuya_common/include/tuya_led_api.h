/***********************************************************
*  File: tuya_led_api.h 
*  Author: anby
*  Date: 20160629
***********************************************************/
#ifndef _TUYA_LED_API_H
#define _TUYA_LED_API_H

#include "led_indicator.h"

/*************************************************************************************
��������: ����LED���ƾ��
�������: gpio_no GPIO���
�������: handle  LED���
�� �� ֵ: ���շ���ֵ�б�
��    ע: ��
*************************************************************************************/
OPERATE_RET tuya_create_led_handle(IN CONST INT gpio_no,OUT LED_HANDLE *handle);

/*************************************************************************************
��������: LED����
�������: handle  LED���
          type    ��������
          <1> OL_LOW    LED�͵�ƽ
          <2> OL_HIGH   LED�ߵ�ƽ
          <3> OL_FLASH_LOW  LED�͵�ƽ��˸
          <4> OL_FLASH_HIGH LED�ߵ�ƽ��˸
          flh_mstime ��˸���ʱ��
�������: ��
�� �� ֵ: ���շ���ֵ�б�
��    ע: ��
*************************************************************************************/
VOID tuya_set_led_type(IN CONST LED_HANDLE handle,IN CONST LED_LT_E type,IN CONST USHORT flh_mstime);

#endif

