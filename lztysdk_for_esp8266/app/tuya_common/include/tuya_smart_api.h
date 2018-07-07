#ifndef _TUYA_SMART_API_H
#define _TUYA_SMART_API_H

#include "sys_adapter.h"
#include "com_struct.h"
#include "uart.h"
#include "base64.h"
#include "cJSON.h"
#include "tuya_gpio_api.h"
#include "tuya_led_api.h"
#include "tuya_key_api.h"
#include "smart_wf_frame.h"
#include "system/sys_timer.h"
#include "system/uni_msg_queue.h"
#include "system/uni_mutex.h"
#include "system/uni_thread.h"
#include "system/uni_semaphore.h"


/*************************************************************************************
函数功能: 获取sdk版本号
输入参数: 无
输出参数: 无
返 回 值: sdk版本号
备    注: 无
*************************************************************************************/
CHAR *tuya_get_sdk_ver(VOID);

/*************************************************************************************
函数功能: 设置wifi配置模式和产测回调
输入参数: mthd
           0    普通模式(do not have low power)
           1    低功耗模式(with low power)
           2    特殊配网模式(special with low power)
          callback
          低功耗模式或特殊配网模式下，设备处于低功耗时扫描到ssid为
          tuya_mdev_test1的热点,自动调用产测回调
输出参数: 无
返 回 值: 无
备    注: 需用户自行在产测回调中实现产测功能
*************************************************************************************/
VOID tuya_app_cfg_set(IN CONST CHAR mthd, APP_PROD_CB callback);

/*************************************************************************************
函数功能: 设置wifi配置模式
输入参数: mode
           0    普通模式(do not have low power)
           1    低功耗模式(with low power)
           2    特殊配网模式(special with low power)
输出参数: 无
返 回 值: 无
备    注: 默认普通模式,必须在app_init中调用
*************************************************************************************/
VOID tuya_set_wf_cfg(IN CONST BYTE mode);

/*************************************************************************************
函数功能: 设备初始化
入口参数: product_id 产品ID
          cb 手机App命令回调函数指针,
          VOID (*)(SMART_CMD_E cmd,cJSON *root)
          <1> cmd 命令类型
           0 表示局域网下发的命令
           1 表示外网下发的命令
          <2> root 命令数据
           例，{"1":100,"2":200}
          app_ver 应用版本号，如"1.0.0"
出口参数: 无
返 回 值: 参照返回值列表
备	  注: 初始化设备，注册数据处理函数
*************************************************************************************/
OPERATE_RET tuya_device_init(IN CONST CHAR *product_id,IN CONST SMART_FRAME_CB cb,CONST CHAR *app_ver);

/*************************************************************************************
函数功能: 设备首次激活成功回调
输入参数: callback 回调函数(用于同步设备状态)
输出参数: 无
返 回 值: 无
备    注: 设备首次激活会调用callback
*************************************************************************************/
VOID tuya_active_reg(IN CONST SYN_DATA_CB callback);

/*************************************************************************************
函数功能: 获取设备ID
输入参数: 无
输出参数: 无
返 回 值: 设备ID
备    注: 无
*************************************************************************************/
CHAR *tuya_get_devid(VOID);

/*************************************************************************************
函数功能: 数据上报
输入参数: data 上报的数据,例{"1":100,"2":200}
输出参数: 无
返 回 值: 参照返回值列表
备    注: 调用此接口固件会保存各DP的数据状态，如再次上传的DP数据与保存的状态相同
          则忽略上传，推荐使用该函数，可使APP、云端、固件三方性能最佳
*************************************************************************************/
OPERATE_RET tuya_obj_dp_report(IN CONST CHAR *data);

/*************************************************************************************
函数功能: 数据上报(透传)
输入参数: data 上报的数据,例{"1":100,"2":200}
输出参数: 无
返 回 值: 参照返回值列表
备    注: 数据透传到服务器，固件内部不做状态处理
*************************************************************************************/
OPERATE_RET tuya_obj_dp_trans_report(IN CONST CHAR *data);

/*************************************************************************************
函数功能: RAW数据上报
输入参数: data 上报的数据
输出参数: 无
返 回 值: 参照返回值列表
备    注: 无
*************************************************************************************/
OPERATE_RET tuya_raw_dp_report(IN CONST BYTE dpid,IN CONST BYTE *data, IN CONST UINT len);
 
/*************************************************************************************
函数功能: 恢复出厂设置
输入参数: 无
输出参数: 无
返 回 值: 无
备    注: 清除配网和设备信息
          <1> 设备已激活，调用会将设备重置成smartconfig配网状态并清除激活信息
          <2> 设备未激活，重复调用该函数会导致设备在smartconfig、ap配网状态来回切换
*************************************************************************************/
VOID tuya_dev_reset_factory(VOID);

/*************************************************************************************
函数功能: 恢复出厂设置并切换到指定状态
输入参数: mode 配置状态
          <1> NW_SMART_CFG EZ状态
          <2> NW_AP_CFG    AP状态
输出参数: 无
返 回 值: 无
备    注: 清除配网和设备信息
*************************************************************************************/
VOID tuya_dev_reset_select(NW_CFG_MODE_E mode);

/*************************************************************************************
函数功能: 获取网关状态
输入参数: 无
输出参数: 无
返 回 值: typedef enum {
                UN_INIT = 0, // 未初始化，比如生产信息未写入
                PROD_TEST, // 产品产测模式
                UN_ACTIVE, // 未激活
                ACTIVE_RD, // 激活就绪态
                STAT_WORK, // 正常工作态
          }GW_STAT_E;
备    注: 无
*************************************************************************************/
GW_STAT_E tuya_get_gw_status(VOID);

/*************************************************************************************
函数功能: 获取WIFI状态
输入参数: 无
输出参数: 无
返 回 值: typedef enum {
                STAT_LOW_POWER = 0 //低功耗状态
                STAT_UNPROVISION, //EZ未配置EZ状态
                STAT_AP_STA_UNCONN,   // AP未配置状态
                STAT_AP_STA_CFG_UNC,  // ap WIFI already config,station disconnect
                STAT_AP_STA_CONN,  //AP和STA已连接状态
                STAT_STA_UNCONN,   //STA未连接状态
                STAT_STA_CONN,     //STA已连接状态
          }GW_WIFI_STAT_E;
备    注: 无
*************************************************************************************/
GW_WIFI_STAT_E tuya_get_wf_status(VOID);

/*************************************************************************************
函数功能: 获取WIFI信号强度
输入参数: ssid 热点名称
输出参数: rssi 信号强度(单位dbm)
返 回 值: 参照返回值列表
备    注: 扫描过程会阻塞，请勿在初始化函数中使用
*************************************************************************************/
OPERATE_RET tuya_get_wf_rssi(IN UCHAR *ssid, OUT CHAR *rssi);

/*************************************************************************************
函数功能: 获取云连接状态
输入参数: 无
输出参数: 无
返 回 值: TRUE 已连接 FALSE 未连接
备    注: 无
*************************************************************************************/
BOOL tuya_get_cloud_stat(VOID);

/*************************************************************************************
函数功能: 获取设备升级状态
输入参数: 无
输出参数: 无
返 回 值: TRUE 升级中 FALSE 未升级
备    注: 无
*************************************************************************************/
BOOL tuya_get_ug_stat(VOID);

/*************************************************************************************
函数功能: 获取本地时间
输入参数: 无
输出参数: st_time
返 回 值: 参照返回值列表
备    注: 无
*************************************************************************************/
OPERATE_RET tuya_get_local_time(OUT struct tm *st_time);
 
/*************************************************************************************
函数功能: 注册PSM模块名称和扇区名称
输入参数: module_name   模块名称
          partition_key 扇区名称
输出参数: 无
返 回 值: 参照返回值列表
备    注: 无
*************************************************************************************/
OPERATE_RET tuya_psm_register_module(IN CONST CHAR *module_name,IN CONST CHAR *partition_key);

/*************************************************************************************
函数功能: 写入PSM数据
输入参数: module_name   模块名称
          variable      变量名
          value         变量值
输出参数: 无
返 回 值: 参照返回值列表
备    注: 只支持字符串类型
*************************************************************************************/
OPERATE_RET tuya_psm_set_single(IN CONST CHAR *module,IN CONST CHAR *variable,IN CONST CHAR *value);

/*************************************************************************************
函数功能: 读取PSM数据
输入参数: module_name   模块名称
          variable      变量名
          max_len       缓冲区大小
输出参数: value         变量值
返 回 值: 参照返回值列表
备    注: 只支持字符串类型
*************************************************************************************/
OPERATE_RET tuya_psm_get_single(IN CONST CHAR *module,IN CONST CHAR *variable,OUT CHAR *value,IN CONST unsigned max_len);

#endif

