/********************************************************************
* 此项目是一个智能插座的demo程序，实现了配网绑定、云端通信（协议数据解析）、*
* 开关控制，同时SDK中集成了OTA升级和定时预约处理。							*
* 引脚说明：GPIO14是开关控制引脚，GPIO3是网络状态指示灯					*
*********************************************************************/

#include <c_types.h>
#include <osapi.h>
#include <mem.h>
#include <cJSON.h>
#include <user_interface.h>
#include <ra_wifi.h>
#include <ra_device_state.h>
#include <ra_utils.h>
#include <ra_parameter.h>
#include <ra_uart.h>
#include <ra_cloud.h>


/*product pin define*/
#define PRODUCT_POWR_PIN					14
#define PRODUCT_NETWORK_STATE_LED_PIN		3 //RXD, GPIO3
/*end*/


/*下发帧命令*/
#define CMD_queryDeviceStatus	0
#define CMD_setPower			2

/*上报帧命令*/
#define CMD_reportDeviceStatus	1

static ra_uint32_t msgid = 0;

FUN_ATTRIBUTE
void  product_power_control(uint8 power)
{
	os_printf("product_power_control: %u\n", power);
	ra_gpio_output(PRODUCT_POWR_PIN, power);
}

FUN_ATTRIBUTE
void product_status_send_to_cloud(void)
{
	char *root_str = NULL, *devtid = NULL;
	cJSON *root = NULL, *params = NULL, *arr = NULL, *data = NULL;
	
	root = cJSON_CreateObject();
	if (root == NULL) goto fail;

	cJSON_AddNumberToObject(root, "msgId", msgid++);
	cJSON_AddStringToObject(root, "action", "devSend");
	cJSON_AddItemToObject(root, "params", params = cJSON_CreateObject());

	cJSON_AddStringToObject(params, "devTid", devtid = ra_get_devtid());
	cJSON_AddItemToObject(params, "appTid", arr = cJSON_CreateArray());
	cJSON_AddItemToObject(params, "data", data = cJSON_CreateObject());

	cJSON_AddNumberToObject(data, "cmdId", CMD_reportDeviceStatus);
	cJSON_AddNumberToObject(data, "power", ra_gpio_input(PRODUCT_POWR_PIN));

	root_str = cJSON_PrintUnformatted(root);
	if (root_str == NULL) goto fail;

	ra_send_data_to_cloud(root_str, os_strlen(root_str));

	goto done;
done:
	os_free(root_str);
	os_free(devtid);
	cJSON_Delete(root);
	return;
fail:
	if(devtid) os_free(devtid);
	if(root) cJSON_Delete(root);
	return;
}

FUN_ATTRIBUTE
ra_uint32_t product_task_cb(cJSON *params)
{
	ra_uint32_t code = CODE_SUCCESS;
	cJSON *power;
	cJSON *data = params; //传入参数params是指向appSend中的data

	cJSON *cmdId = cJSON_GetObjectItem(data, "cmdId"); //获取命令Id
	if (cmdId == NULL) return CODE_ERROR_PARAMS;

	switch (cmdId->valueint)
	{
	case CMD_queryDeviceStatus:
		break;

	case CMD_setPower:
		power = cJSON_GetObjectItem(data, "power"); //获取参数power
		if (power == NULL) return CODE_ERROR_PARAMS;
		product_power_control(power->valueint);
		break;

	default:
		code = CODE_ERROR_CMDID_INVALID;
		goto fail;
		break;
	}

	goto done;
done:
fail:
	return code;
}

FUN_ATTRIBUTE
void report_device_status(void *arg)
{
	product_status_send_to_cloud();
}

FUN_ATTRIBUTE
void device_state_changed_callback(ra_device_state_type_t item, ra_bool current_state)
{
	if (item == RA_DEVICE_STATE_WLAN_CONNECTED&&
		current_state == ra_true)
	{
		os_printf("wifi connect successed\n");

		struct station_config config;
		wifi_station_get_config(&config);
		wifi_station_set_config(&config);

		//路由器连接成功，接着登录服务器
		ra_connect_to_cloud();
	}
	else if (item == RA_DEVICE_STATE_WLAN_CONNECT_FAILED&&
		current_state == ra_true)
	{
		os_printf("wifi connect failed\n");
	}

	if (item == RA_DEVICE_STATE_CLOUD_CONNECTED&&
		current_state == ra_true)
	{
		ra_enable_cloud_data_parse(); //使能内部解析云端数据
		ra_register_appsend_parse_callback(product_task_cb);
		ra_register_gettimerlistresp_parse_callback(product_task_cb); //定时预约任务通常和appSend任务是相同的
		ra_register_appsend_finished_callback(report_device_status); //appsend处理完成后，上报一下设备状态
	}
}

FUN_ATTRIBUTE
void wifi_config_finish_callback(char *ssid, char *password)
{
	if ((ssid != NULL) && (password != NULL))
	{
		ra_connect_wifi(ssid, password, NULL, 30 * 1000); //配网成功，获取到ssid及password，接着连接路由器
	}
	else
	{
		os_printf("wifi_config failed\n");
		//配网失败
	}
}

FUN_ATTRIBUTE
ra_bool  wifi_config_is_exist(void)
{
	struct station_config config;
	wifi_station_get_config(&config);
	return (config.ssid[0] == '\0' ? 0 : 1);
}

FUN_ATTRIBUTE
void product_hardware_init(void)
{
	ra_gpio_set_pullup(PRODUCT_POWR_PIN, 1);
	ra_device_state_led_task_install(PRODUCT_NETWORK_STATE_LED_PIN, 0); //注册设备网络状态指示灯
}

FUN_ATTRIBUTE
void ra_user_main(void)
{
	product_hardware_init();
	ra_set_parameter_string("prodKey", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");	//设置prodKey, prodKey时在console平台上创建产品时获取
	ra_set_current_firmware_version("4.1.2.1"); //固件版本号设置，格式必须是"xx.xx.xx.xx"
	ra_uart_set_terminal(RA_UART1); //设置打印终端，可将调试信息输出至GPIO2

	ra_register_device_state_changed_callback(device_state_changed_callback); //注册设备状态改变时的回调函数

	//判断是否配过网
	if (wifi_config_is_exist() == 1)
	{
		os_printf("wifi to connect\n");
		wifi_station_connect();
	}
	else
	{
		os_printf("wifi_config start\n");
		ra_register_wifi_config_callback(wifi_config_finish_callback); //注册wifi config配网的回调函数
		ra_start_wifi_config(RA_WIFI_CONFIG_TYPE_HEKR); //启动wifi config配网
	}
}