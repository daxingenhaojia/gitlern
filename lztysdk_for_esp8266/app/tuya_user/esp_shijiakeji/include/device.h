
/***********************************************************
*  File: device.h 
*  Author: nzy
*  Date: 20150605
***********************************************************/
#ifndef _DEVICE_H
#define _DEVICE_H

#include "sys_adapter.h"
#include "error_code.h"
    
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __DEVICE_GLOBALS
    #define __DEVICE_EXT
#else
    #define __DEVICE_EXT extern
#endif

typedef BYTE  DPID_T;
typedef BYTE  CMT_T;
typedef BYTE  RD_WF_STAT_E;
typedef PVOID SEMAPHO_H;




typedef enum
{
  	CMD_GAME_START =0x01,
	CMD_ROOT_BACK ,
	CMD_ROOT_FRONT,
	CMD_ROOT_LEFT ,
	CMD_ROOT_RIGHT,
	CMD_ROOT_GRAB  =0x07,
	CMD_ROOT_BSTOP =0x21,
	CMD_ROOT_FSTOP,
	CMD_ROOT_LSTOP,
    CMD_ROOT_RSTOP,
}CMD_SEND;


typedef enum
{
   	CMD_R_GAME_START =0x01,
   	CMD_R_ROOT_BACK  =0xa1,
	CMD_R_ROOT_FRONT =0xa2,
	CMD_R_ROOT_LEFT  =0xa3,
	CMD_R_ROOT_RIGHT =0xa4,
	CMD_R_ROOT_GRAB  =0x07,
	CMD_R_GRAB_SUCC  =0x09,
	CMD_R_GAME_OVER  =0x0a,
	CMD_R_ROOT_BSTOP =0xb1,
	CMD_R_ROOT_FSTOP =0xb2,
	CMD_R_ROOT_LSTOP =0xb3,
    CMD_R_ROOT_RSTOP =0xb4,
    
    CMD_R_MACHINE_CHANCE=0xb5,  
    CMD_R_MACHINE_GETCHANCE=0xb6,
    CMD_R_MACHINE_STOP=0xb7,
}CMD_RECV;


typedef enum
{
    FUNC_TRANSFER = 0x03,
    FUNC_QUER,
    FUNC_INTERNET,
    FUNC_RESET,
    FUNC_WEATHER,
    FUNC_SENSE,
}FUNC_TYPE_S;

    
typedef struct
{
  BYTE P_head[7];	//24 40 64 64 1a 03 
  BYTE P_cmd[1];	//data
  BYTE P_len3[1];	//01
  BYTE P_len0[16];	//00
  BYTE P_len1[6];	//01 02 03 04 05 06
  BYTE P_sum[1];	//异或校验码
  BYTE P_end[2];	//0D 0A
  
}glo_quer_str;

typedef struct
{
  BYTE GET_DATA[15];
}GET_quer_str;


#define P_DATA_LEN   3
#define P_HEAD_LEN   2
#define P_END_LEN    2

typedef struct
{
    BYTE   pack_head[P_HEAD_LEN];        
    BYTE   pack_data[P_DATA_LEN];
    BYTE   pack_end[P_END_LEN];
    
}TY_WIFI_SET;


typedef struct
{
    BYTE   rev_head[P_HEAD_LEN+2];  
    BYTE   rev_cmd;
    BYTE   rev_end[P_END_LEN];

}REV_DATA_S;



typedef struct
{
    BYTE   rev_head[P_HEAD_LEN+2];  
    BYTE   rev_cmd;
    BYTE   rev_end[P_END_LEN];

}REV_chanceDATA_S;      //返回的概率帧


typedef struct {
    BYTE len;
    BYTE data[0];
}UR_SEND_S;


typedef enum{

	DP_GAME_START        =  101,
	DP_ROOT_FRONT_STOP   =  102,
	DP_ROOT_BACK_STOP    =  103,
	DP_ROOT_LEFT_STOP    =  104,
	DP_ROOT_RIGHT_STOP   =  105,
    DP_ROOT_GRAB         =  106,
    DP_ROOT_GRAB_SUCCES  =  111,  //只上报
    DP_GAME_OVER         =  112,  //只上报
    DP_MACHINE_STOP      =  113,  //只上报 故障
    DP_GET_CHANCE		 =  114,   //只下发
    DP_CHANGE_CHANCE	 =  115,
}DPID_E;



#define SW_VER                    USER_SW_VER
#define PRODECT_KEY               "uAzdaHYStpzEeqIS"     
#define PRODECT_TEST_NAME         "tuya_mdev_test"
#define PRO_TEST_OK      1
#define UART_TX_DEBUG  1          //实际打固件需要关闭
#define UART_RX_DEBUG  1
#define CHECK_SUCCES   1
#define CHECK_FAIL     0

#define DATA_BUF_SZ  255
#define QUE_NUM_SZ   5
#define CMD_TIMEOUT_MS  200
#define TIMEOUT_CNT_LMT  3
#define TY_FR_LEN_MAX   60       //fr_len max
#define TY_FR_LEN_MIN   8        //sizeof(TY_PRO_HEAD_S) + XOR
#define TY_WIFI_LEN   7          //reset buf len
#define RESET_FR_LEN 0x0B        //recv reset command len
#define MIN_RECV_lENTH     1  

#define POWER_CLO        1
#define WIFI_STA_CLO     2


__DEVICE_EXT \
OPERATE_RET device_init(VOID);





#ifdef __cplusplus
}
#endif
#endif




