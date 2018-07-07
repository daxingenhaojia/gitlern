/***********************************************************
*  File: sys_timer.h
*  Author: nzy
*  Date: 20150901
***********************************************************/
#ifndef _SYS_TIMER_H
#define _SYS_TIMER_H
    
    #include "sys_adapter.h"
    #include "uni_time_queue.h"
    
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: system_timer_init ϵͳ��ʱ����ʼ��
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET system_timer_init(void);

/***********************************************************
*  Function: sys_add_timer ���һ��ϵͳ��ʱ��
*  Input: timerID->��ʱ��ID
*         pTimerFunc->��ʱ��������
*         pTimerArg->��ʱ�������������
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET sys_add_timer(IN CONST P_TIMER_FUNC pTimerFunc,\
                          IN CONST PVOID pTimerArg,\
                          OUT TIMER_ID *p_timerID);

/***********************************************************
*  Function: sys_delete_timer ɾ��һ����ʱ��
*  Input: timerQueHandle->��ʱ�����й���ṹ���
          timerID->��ʱ��ID
*  Output: none 
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
extern \
OPERATE_RET sys_delete_timer(IN CONST TIMER_ID timerID);

/***********************************************************
*  Function: sys_stop_timer ֹͣһ����ʱ��
*  Input: timerID->��ʱ��ID
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET sys_stop_timer(IN CONST TIMER_ID timerID);

/***********************************************************
*  Function: ��ϵͳ��ʱ���Ƿ�����
*  Input: timerID->��ʱ��ID
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
BOOL IsThisSysTimerRun(IN CONST TIMER_ID timer_id);

/***********************************************************
*  Function: system_timer_release ϵͳ��ʱ����Դ�ͷ�
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET system_timer_release(void);

/***********************************************************
*  Function: sys_start_timer ����һ����ʱ��
*  Input: timerID->��ʱ��ID
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET sys_start_timer(IN CONST TIMER_ID timerID,\
                            IN CONST TIME_MS timeCycle,\
                            IN CONST TIMER_TYPE timer_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif








