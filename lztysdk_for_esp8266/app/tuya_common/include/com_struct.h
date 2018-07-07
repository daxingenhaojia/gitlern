/***********************************************************
*  File: com_struct.h 
*  Author: nzy
*  Date: 20150522
***********************************************************/
#ifndef _COM_STRUCT_H
    #define _COM_STRUCT_H

    #include "com_def.h"
    #include "sys_adapter.h"
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __COM_STRUCT_GLOBALS
    #define __COM_STRUCT_EXT
#else
    #define __COM_STRUCT_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
// gw information define
#define PROD_IDX_LEN 8 // prodect index len
#define GW_ID_LEN 25 // gw id len
#define DEV_ID_LEN 25 // dev id len
#define UID_LEN 20 // user id len
#define UID_ACL_LMT 1 // user acl limit
#define SW_VER_LEN 10 // sw ver len
#define SEC_KEY_LEN 16 // securt key len
#define LOCAL_KEY_LEN 16 //securt lan dps key len
#define ACTIVE_KEY_LEN 16 // active key len
#define SCHEMA_ID_LEN 10 
#define ETAG_LEN 10
#define HTTP_URL_LMT 128
#define MQ_URL_LMT 128
#define PRODUCT_KEY_LEN 16
#define TIME_ZONE_LEN  10
//
#define WXAPPID_LEN 32
#define REGION_LEN 2
#define REGIST_KEY_LEN 4

// gateway access ability
typedef INT GW_ABI;
#define GW_NO_ABI 0 // virtual gateway
#define GW_ZB_ABI (1<<0) // zigbee
#define GW_BLE_ABI (1<<1) // ble
#define GW_RF433_ABI (1<<2) // RF 315/433
#define GW_DEF_ABI GW_NO_ABI

// device ability
typedef BYTE DEV_ABI_E;
#define DEV_SINGLE 0 // 特指WIFI单品设备
#define DEV_ZB 1 // zigbee
#define DEV_BLE 2
#define DEV_RF433 3
#define DEV_WIFI 4 // 通过实体GW接入的WIFI设备

// dp type
typedef BYTE DP_TYPE_E;
#define T_OBJ 0
#define T_RAW 1
#define T_FILE 2

// dp mode
typedef BYTE DP_MODE_E;
#define M_RW 0
#define M_WR 1
#define M_RO 2

// dp schema type
typedef BYTE DP_PROP_TP_E;
#define PROP_BOOL 0
#define PROP_VALUE 1
#define PROP_STR 2
#define PROP_ENUM 3
#define PROP_BITMAP 4
#define PROP_COMPLEX 5 // add by nzy 20150810 

typedef struct {
    CHAR id[GW_ID_LEN+1];
    CHAR sw_ver[SW_VER_LEN+1]; // xx.xx
    CHAR bs_ver[SW_VER_LEN+1]; // base version xx.xx
    CHAR prtl_ver[SW_VER_LEN+1]; // protocol version
    CHAR etag[ETAG_LEN+1]; // add by nzy 20151015
    GW_ABI ability;
    BOOL sync;
}GW_DESC_IF_S;

typedef struct {
    CHAR id[DEV_ID_LEN+1];
    CHAR sw_ver[SW_VER_LEN+1];
    CHAR schema_id[SCHEMA_ID_LEN+1];
    CHAR etag[ETAG_LEN+1]; // add by nzy 20151015
    CHAR product_key[PRODUCT_KEY_LEN+1]; // add by nzy 20151016,for support product key to activate device
    DEV_ABI_E ability;
    BOOL bind;
    BOOL sync;
}DEV_DESC_IF_S;

typedef struct {
    INT httploglevel;
    INT silentUgprade;
}GW_RUNTIME_COFIG_IF_S;


// dp prop 
typedef struct {
    INT min;
    INT max;
    SHORT step;
    USHORT scale; // 描述value型DP的10的指数
    INT value;
}DP_PROP_VAL_S;

typedef struct {
    INT cnt;
    CHAR **pp_enum;
    INT value;
}DP_PROP_ENUM_S;

typedef struct {
    INT max_len;
    CHAR *value;
}DP_PROP_STR_S;

typedef struct {
    BOOL value;
}DP_BOOL_S;

typedef struct {
    UINT max_len;
    UINT value;
}DP_PROP_BITMAP;

// 用于描述DP的附加属性，日期类型
typedef struct {
    ULONG value; 
}DP_DATE_S;

typedef union {
    DP_PROP_VAL_S prop_value;
    DP_PROP_ENUM_S prop_enum;
    DP_PROP_STR_S prop_str;
    DP_BOOL_S prop_bool;
    DP_PROP_BITMAP prop_bitmap;
}DP_PROP_VALUE_U;

typedef BYTE DP_TRIG_T_E;
#define TRIG_PULSE 0
#define TRIG_DIRECT 1

/*
标识某DP状态是否为主动上报
DP SCHEMA中记录值为bool型
*/
typedef BYTE DP_PSV_E;
#define PSV_FALSE 0
#define PSV_TRUE 1
#define PSV_F_ONCE 2

typedef struct {
    BYTE dp_id;
    DP_MODE_E mode;
    DP_PSV_E passive;
    DP_TYPE_E type;
    DP_PROP_TP_E prop_tp; // type == obj时有效
    DP_TRIG_T_E trig_t; // 联动触发类型
}DP_DESC_IF_S;

typedef BYTE DP_PV_STAT_E;
#define INVALID 0 // 数据无效
#define VALID_LC 1 // 本地有效数据
#define VALID_ULING 2 // 数据云端上传中 add by nzy 20150808
#define VALID_CLOUD 3 // 本地有效数据与服务端一致

typedef struct {
    DP_DESC_IF_S dp_desc;
    DP_PROP_VALUE_U prop;
    DP_PV_STAT_E pv_stat; 
}DP_CNTL_S;

typedef struct dev_cntl_n_s {
    struct dev_cntl_n_s *next;
    DEV_DESC_IF_S dev_if;
    BOOL online;
    BOOL preprocess; // 指示该设备是否预处理
    BYTE dp_num;
    DP_CNTL_S dp[0];
}DEV_CNTL_N_S;

typedef struct {
    CHAR token[SEC_KEY_LEN+1];
    CHAR key[SEC_KEY_LEN+1];
	CHAR local_key[LOCAL_KEY_LEN+1];
    CHAR http_url[HTTP_URL_LMT+1];
    CHAR mq_url[MQ_URL_LMT+1];
    CHAR mq_url_bak[MQ_URL_LMT+1];
	CHAR time_zone[TIME_ZONE_LEN+1];
    INT uid_cnt;
    CHAR uid_acl[UID_ACL_LMT][UID_LEN+1];
	CHAR region[REGION_LEN + 1];
	CHAR regist_key[REGIST_KEY_LEN + 1];
    CHAR wx_app_id[WXAPPID_LEN+1];
}GW_ACTV_IF_S;

typedef BYTE GW_STAT_E;
#define UN_INIT 0 // 未初始化，比如生产信息未写入
#define PROD_TEST 1 // 产品产测模式
#define UN_ACTIVE 2 // 未激活
#define ACTIVE_RD 3 // 激活就绪态
#define STAT_WORK 4 // 正常工作态

typedef BYTE GW_WIFI_STAT_E;
#define STAT_LOW_POWER 0    // low power 
#define STAT_UNPROVISION 1 
#define STAT_AP_STA_UNCONN 2 // ap WIFI not config
#define STAT_AP_STA_CFG_UNC 3 // ap WIFI already config,station disconnect
#define STAT_AP_STA_CONN 4
#define STAT_STA_UNCONN 5
#define STAT_STA_CONN 6

typedef struct {
    GW_DESC_IF_S gw;
    GW_ACTV_IF_S active;
    GW_STAT_E stat;
    BOOL mq_conn_stat;
    INT dev_num;
    DEV_CNTL_N_S *dev;
}GW_CNTL_S;

/***********************************************************
*************************variable define********************
***********************************************************/
#define BS_VER "5.25"
#define PT_VER "2.1"
#define LAN_PRO_VER "3.1"
/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: 
*  Input: 
*  Output: 
*  Return: 
***********************************************************/

#ifdef __cplusplus
}
#endif
#endif

