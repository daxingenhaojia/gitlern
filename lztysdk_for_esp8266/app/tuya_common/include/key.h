/***********************************************************
*  File: key.h 
*  Author: nzy
*  Date: 20150522
***********************************************************/
#ifndef _KEY_H
    #define _KEY_H

    #include "com_def.h"
    #include "sys_adapter.h"
    #include "gpio.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __KEY_GLOBALS
    #define __KEY_EXT
#else
    #define __KEY_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef enum {
    NORMAL_KEY = 0,
    SEQ_KEY,
    LONG_KEY,
}PUSH_KEY_TYPE_E;

typedef enum {
    KEY_DOWN = 0,
    KEY_DOWN_CONFIRM,
    KEY_DOWNNING, 
    KEY_UP_CONFIRM,
    KEY_UPING,
    KEY_FINISH,
}KEY_STAT_E;

// NORMAL_KEY or SEQ_KEY's trigger type
typedef enum {
    KEY_UP_TRIG = 0,
    KEY_DOWN_TRIG
}KEY_TRIGGER_TP_E;

typedef enum {
	RST_ERROR = 0,
	RST_NORMAL,
	RST_EXCEPT,	
} RSET_REASON_E;

typedef VOID(* KEY_CALLBACK)(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt);

// (cnt >= 2) ==> SEQ_KEY
// time < long_key_time && (cnt = 1) ==> NORMAL_KEY
// time >= long_key_time && (cnt = 1) ==> LONG_KEY
typedef struct {
    // user define
    INT gpio_no;
    INT long_key_time; // ms (long_key_time == 0 ? ���γ�����)
    KEY_CALLBACK call_back;

    // run variable
    KEY_STAT_E status;
    INT down_time; // ms
    INT up_time;
    INT seq_key_cnt;
    KEY_TRIGGER_TP_E trig_ty;
    BOOL down_trig_cont; // is down trigger continuous,���´����Ƿ���������
    BOOL first_trig;
}KEY_ENTITY_S;

#define TIMER_SPACE_MAX 100 // ms
/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: set_key_detect_time
*  Input: ms �����������ʱ����(��λ:����)
*  Output: 
*  Return: VOID
*  Note: Ĭ��ֵ400ms,��Ҫ��key_init����֮ǰ���ò���Ч
***********************************************************/
__KEY_EXT \
VOID set_key_detect_time(IN CONST INT ms);

/***********************************************************
*  Function: set_key_detect_high_valid
*  Input: is_high TRUE:�ߵ�ƽ��Ч  FALSE:�͵�ƽ��Ч
*  Output: 
*  Return: VOID
*  Note: Ĭ�ϵ͵�ƽ��Ч,��Ҫ��key_init����֮ǰ���ò���Ч
***********************************************************/
__KEY_EXT \
VOID set_key_detect_high_valid(BOOL is_high);

/***********************************************************
*  Function: key_init
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__KEY_EXT \
OPERATE_RET key_init(IN KEY_ENTITY_S *p_tbl,\
                     IN CONST INT cnt,
                     IN CONST INT timer_space);

/***********************************************************
*  Function: reg_proc_key
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__KEY_EXT \
OPERATE_RET reg_proc_key(IN CONST INT gpio_no,IN CONST INT long_key_time,IN CONST KEY_CALLBACK call_back);

/***********************************************************
*  Function: set_key_trig_type,set key trigger type,only used for normal key or seq key
*  Input: 
*  Output: 
*  Return: OPERATE_RET
*  NOTE:if (down_trig_cont == TRUE) then long key is invalid
***********************************************************/
__KEY_EXT \
VOID set_key_trig_type(IN CONST INT gpio_no,\
                       IN CONST KEY_TRIGGER_TP_E trig_ty,\
                       IN CONST BOOL down_trig_cont);

#ifdef __cplusplus
}
#endif
#endif

