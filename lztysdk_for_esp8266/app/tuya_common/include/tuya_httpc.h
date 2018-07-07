/***********************************************************
*  File: tuya_httpc.h 
*  Author: nzy
*  Date: 20150527
***********************************************************/
#ifndef _TUYA_HTTPC_H
    #define _TUYA_HTTPC_H

    #include "com_def.h"
    #include "sys_adapter.h"
    #include "mem_pool.h"
    #include "error_code.h"
    #include "com_struct.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __TUYA_HTTPC_GLOBALS
    #define __TUYA_HTTPC_EXT
#else
    #define __TUYA_HTTPC_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#if 0
//预发
#define TY_SMART_DOMAIN_AY "http://a.gw.cn.wgine.com/gw.json"
#define TY_SMART_DOMAIN_AZ "http://a.gw.getairtake.com/gw.json"
#define TY_SMART_DOMAIN_EU "http://a.gw.tuyaeu.com/gw.json"					//Europe
#define TY_SMART_MQTT "mq.mb.cn.wgine.com"
#define TY_SMART_MQTTBAK "mq.mb.cn.wgine.com"
#else
//线上
//#define TY_SMART_DOMAIN_AY "http://a.gw.airtakeapp.com/gw.json"
//#define TY_SMART_DOMAIN_AZ "http://a.gw.getairtake.com/gw.json"
#define TY_SMART_DOMAIN_AY "http://a.gw.tuyacn.com/gw.json"				//China
#define TY_SMART_DOMAIN_AZ "http://a.gw.tuyaus.com/gw.json"					//American
#define TY_SMART_DOMAIN_EU "http://a.gw.tuyaeu.com/gw.json"					//Europe
#define TY_SMART_MQTT "mq.gw.tuyacn.com"
#define TY_SMART_MQTTBAK "mq.gw1.tuyacn.com"
#endif
#define	WX_TIMEZONE		"+08:00"

// gw interface
#define TI_GW_ACTIVE "s.gw.active" // gw active
#define TI_GW_RESET "s.gw.reset" // gw reset
#define TI_GW_HB "s.gw.heartbeat" // gw heart beat
#define TI_GW_INFO_UP "s.gw.update" // update gw base info

#define TI_BIND_DEV "s.gw.dev.bind" // dev bind
#define TI_DEV_INFO_UP "s.gw.dev.update" // dev info upgrade
#define TI_UNBIND_DEV "s.gw.dev.unbind" // dev unbind
#define TI_DEV_REP_STAT "s.gw.dev.online.report" // dev report status
#define TI_DEV_DP_REP "s.gw.dev.dp.report" // dev dp report

#define TI_GET_GW_DEV_TIMER_COUNT "s.gw.dev.timer.count" //get gw or dev timer count
#define TI_GET_GW_DEV_TIMER "s.gw.dev.timer.get" //get gw or dev timer schema
#define TI_GET_GW_ACL "s.gw.group.get" // get gw access list

// firmware upgrade by gw
#define TI_FW_UG_INFO "s.gw.upgrade" // get gw/dev firmware info
#define TI_FW_STAT "s.gw.upgrade.updatestatus"
#define TI_FW_SELF_UG_INFO "tuya.device.upgrade.silent.get"

#define TI_GW_DEV_ACTIVE "s.gw.dev.active" // gateway device active
#define TI_GW_DEV_PK_ACTV "s.gw.dev.pk.active" // new acktive process
#define TI_GW_DEV_FK_ACTV "s.gw.dev.fk.active" //需考虑老版本固件兼容问题
#define TI_GW_DEV_PK_ACTV_WX "s.gw.dev.pk.active.wx" // gateway device prodect key active wx
#define	TI_GW_TOKEN_GET		"s.gw.token.get"		//gatewaye token get data

#define TI_GW_EXIST "s.gw.exist" // gateway is exist?
#define TI_UG_RST_LOG "atop.online.debug.log" // ug log
#define TI_GW_GET_WTH "tuya.device.public.data.get"
#define TI_UG_RUNSTAT_LOG "tuya.device.log.report" // ug runstat

#define TI_GW_GET_EXT_CFG "tuya.device.extension.config.get"


// fw upgrade status
typedef enum {
    UG_IDLE = 0,
    UG_RD,
    UPGRADING,
    UG_FIN,
    UG_EXECPTION,
}FW_UG_STAT_E;

// firmware upgrade
#define FW_URL_LEN 255
#define FW_MD5_LEN 32

// upgrade type 
typedef enum {
    GW_UPGD = 1,
    DEV_UPGD,
}UPGD_TYPE_E;

typedef struct {
    UPGD_TYPE_E tp;
    CHAR fw_url[FW_URL_LEN+1];
    CHAR fw_md5[FW_MD5_LEN+1];
    CHAR sw_ver[SW_VER_LEN+1];
    CHAR etag[ETAG_LEN+1];
    UINT file_size;
#if 0
    INT  auto_ug;
#endif
}FW_UG_S;


typedef OPERATE_RET(*FILE_HTTPC_CB)(IN CONST BYTE *data,\
                                    IN CONST UINT len);

typedef OPERATE_RET(*FILE_PRE_CB)(IN CONST UINT file_size);


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: httpc_ug_log
*  Input: 
*  Output: 
*  Return: OPERATE_RET
*  note: 上报设备日志，如重启原因，离线时间，联网状态等
***********************************************************/
OPERATE_RET httpc_ug_log(VOID);
OPERATE_RET httpc_ug_log_custom(IN CONST CHAR *log, IN CONST INT log_len);

/***********************************************************
*  Function: httpc_aes_init
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_aes_init(VOID);

/***********************************************************
*  Function: httpc_aes_init
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_aes_set(IN CONST BYTE *key,IN CONST BYTE *iv);

/***********************************************************
*  Function: httpc_gw_active
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
#if (defined(SW_VER) && defined(DEF_NAME) && defined(DEV_ETAG))
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_active();
#endif

/***********************************************************
*  Function: httpc_gw_reset
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_reset(VOID);

/***********************************************************
*  Function: httpc_gw_update
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_update(VOID);

/***********************************************************
*  Function: httpc_gw_hearat
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_hearat(VOID);

/***********************************************************
*  Function: httpc_dev_bind
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
#if (defined(SW_VER) && defined(DEF_NAME) && defined(DEV_ETAG))
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_bind(IN CONST DEV_DESC_IF_S *dev_if);
#endif

/***********************************************************
*  Function: httpc_dev_unbind
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_unbind(IN CONST CHAR *id);

/***********************************************************
*  Function: httpc_dev_update
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_update(IN CONST DEV_DESC_IF_S *dev_if);

/***********************************************************
*  Function: httpc_dev_stat_report
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_stat_report(IN CONST CHAR *id,IN CONST BOOL online);

/***********************************************************
*  Function: httpc_device_dp_report
*  Input: data->format is:{"devid":"xx","dps":{"1",1}}
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_device_dp_report(IN CONST DP_TYPE_E type,\
                                   IN CONST CHAR *data);

#if 0
/***********************************************************
*  Function: httpc_dev_raw_dp_report
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_raw_dp_report(IN CONST CHAR *id,IN CONST BYTE dpid,\
                                    IN CONST BYTE *data, IN CONST UINT len);


/***********************************************************
*  Function: httpc_dev_obj_dp_report
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_obj_dp_report(IN CONST CHAR *id,IN CONST CHAR *data);
#endif

/***********************************************************
*  Function: httpc_get_fw_ug_info
*  Input: etag
*  Output: p_fw_ug
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_get_fw_ug_info(IN CONST CHAR *etag,OUT FW_UG_S *p_fw_ug);

/***********************************************************
*  Function: httpc_get_self_fw_ug_info
*  Input: etag
*  Output: p_fw_ug
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET httpc_get_self_fw_ug_info(OUT FW_UG_S *p_fw_ug);


/***********************************************************
*  Function: httpc_up_fw_ug_stat
*  Input: etag devid stat
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_up_fw_ug_stat(IN CONST CHAR *devid,\
                                IN CONST FW_UG_STAT_E stat);

/***********************************************************
*  Function: httpc_upgrade_fw
*  Input: url_str
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_upgrade_fw(IN CONST CHAR *url_str);

/***********************************************************
*  Function: httpc_upgd_mcu_cb_reg
*  Input: callback
*  Output: 
*  Return: none
***********************************************************/
__TUYA_HTTPC_EXT \
VOID httpc_upgd_mcu_cb_reg(IN CONST FILE_HTTPC_CB callback, IN CONST FILE_PRE_CB pre_callback);

OPERATE_RET httpc_fsize_fw_out(IN CONST UINT fsize);

/***********************************************************
*  Function: httpc_upgrade_fw_out
*  Input: url_str
*  Output: 
*  Return: OPERATE_RET
*  note: use module as the data transmission channel to upgrade the outside device
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_upgrade_fw_out(IN CONST CHAR *url_str);

/***********************************************************
*  Function: httpc_gw_dev_active 
*  Input: dev_if
*  Output: 
*  Return: OPERATE_RET
*  说明:用于单品设备绑定+实体网关附带虚拟设备绑定
***********************************************************/
#if (defined(SW_VER) && defined(DEF_NAME) && defined(DEV_ETAG))
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_active(IN CONST DEV_DESC_IF_S *dev_if);
#endif

/***********************************************************
*  Function: httpc_gw_dev_active_pk 
*  Input: dev_if
*  Output: 
*  Return: OPERATE_RET
*  说明:通过prodect_key激活,用于单品设备绑定+实体网关附带虚拟设备绑定
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_active_pk(IN CONST DEV_DESC_IF_S *dev_if);

/***********************************************************
*  Function: httpc_gw_dev_timer_count 
*  Input: posix 本地定时时间戳
*  Output: 
*  Return: OPERATE_RET
*  None:
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_timer_count(IN CONST INT posix, OUT INT *count, OUT INT *fechTime);

/***********************************************************
*  Function: httpc_gw_dev_timer 
*  Input: offset 偏移位置
*         limit  数量
*  Output: scharr 定时记录,格式[{},{},{}]
*  Return: OPERATE_RET
*  参照返回值列表
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_timer(IN CONST INT offset, IN CONST INT limit, CHAR **scharr);

/***********************************************************
*  Function: httpc_get_weather 
*  Input: paraIn 参数数组,如:["w.temp","w.pm25"]
*  Output: ppOut 参数结果,如{"w.temp":20,"w.pm25":17}
*  Return: OPERATE_RET
*  参照返回值列表
***********************************************************/
OPERATE_RET httpc_get_weather(IN CHAR *paraIn,OUT CHAR **ppOut);

OPERATE_RET httpc_get_ext_cfg(VOID);



/***********************************************************
*  Function: get_pub_exptime 
*  Input: 
*  Output: 
*  Return: 过期时间,单位分钟
*  note:默认30分钟
***********************************************************/
INT get_pub_exptime(VOID);

#ifdef __cplusplus
}
#endif
#endif

