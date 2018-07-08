/********************************************************************
* 此项目是一个智能插座的demo程序，实现了配网绑定、云端通信（协议数据解析）、*
* 开关控制，同时SDK中集成了OTA升级和定时预约处理。							*
* 引脚说明：GPIO14是开关控制引脚，GPIO3是网络状态指示灯					*
*********************************************************************/

#include <string.h>
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
#include <ra_button.h>
#include "uart/protocol.h"
#include <zj/coroutine.h>
#include "version.h"
#include <ra_gpio.h>
#include <ra_timer.h>

/*product pin define*/
#define PRODUCT_POWR_PIN					14
//#define PRODUCT_NETWORK_STATE_LED_PIN		12 //RXD, GPIO3
//#define CATCH_PIN                           1  //串口 
//#define CATCH_PIN                           2
#define PRODCT_CONFIG_PIN                   13
#define PRODUCT_NETWORK_STATE_LED_PIN		4 //
/*end*/
#define SEND_TIME                           200

#define COUNTS "counts"

/*下发帧命令*/
#define CMD_Query           	0
#define CMD_Set_Forward	        1
#define CMD_Set_Backward	    2
#define CMD_Set_Left            3
#define CMD_Set_Right           4
#define CMD_Set_Enter           5
#define CMD_Set_Coin            7
#define CMD_Set_steplength      8


/*#define CMD_Set_Stopforward		9
#define CMD_Set_Stopbackward	10
#define CMD_Set_Stopleft		11
#define	CMD_Set_Stopright		12*/

/*上报帧命令*/
#define CMD_reportDeviceStatus	6

static ra_uint32_t msgid = 0;

/////////////////////////////////////////////////////////////////////

#define TEST_ROUTER_SSID		"zzj1"
#define TEST_ROUTER_PASSWORD	"62683787"

#define CLOSE        0
#define OPEN         1

typedef struct {
	ETSTimer timer;
	unsigned int ecr_line;
}factory_config_t;
factory_config_t g_factory_config;

bool router_scan_finished_flag = false, router_exist_flag = false, factory_test_flag = false;
void product_status_send_to_cloud(char *buf,ra_bool catchflag);
void Start_wifi_config_SendFrame(void);
void Start_catch(void);
void zj_factorytest(void *arg);

void zj_gpio_output(ra_uint8_t pin, ra_gpio_state_t value);
int send_flag = 1;
int gold_flag = 1;

int ret_test = 0;
int direct = 0;

int key_test = 0;
int DELAY_TIME = 100;
ra_bool report_timer_flag = ra_false;
ra_bool eyes_report_timer_flag = ra_false;

ra_timer_t timer_output;//上下左右下发定时器
ra_timer_t send_flag_timer;//抓取下发定时器
ra_timer_t gold_flag_timer;//投币倒计时定时器
ra_timer_t report_timer;//光眼信号上报定时器

ra_timer_t reenter_timer;//等待抓取回复定时器1
ra_timer_t reenter_timer2;//等待抓取回复第二次定时器

/*控制命令字符*/ 
char command[7]	      = {0x24,0x40,0x77,0x77,0x01,0x0d,0x0a};//此命令是forward，作为一个模板，修改第四位即可

#define MAXREBUF  7 //接收最大

/*串口回复标志位*/
bool restart = 0;
bool reenter = 0;
bool reforward = 0;
bool rebackward = 0;
bool releft = 0;
bool reright = 0;

bool stop_restart = 0;
bool stop_reenter = 0;
bool stop_reforward = 0;
bool stop_rebackward = 0;
bool stop_releft = 0;
bool stop_reright = 0;

FUN_ATTRIBUTE
void wifi_station_scan_cb(void *arg, STATUS status)
{
	uint16_t channel_flag = 0;
	os_printf("status: %u\n", status);
	router_scan_finished_flag = true;
	if (arg)
	{
		router_exist_flag = true;
		os_printf("scanningOK");
	}
	else
	{
		router_exist_flag = false;
		os_printf("scanningFAIL");
	}
}

FUN_ATTRIBUTE
bool factory_router_scanning(void)
{
	struct scan_config config;
	char ssid[64];

	//wifi_set_opmode(STATION_MODE);

	config.bssid = NULL;
	config.channel = NULL;
	config.show_hidden = 0;

	memcpy(ssid, TEST_ROUTER_SSID, sizeof(TEST_ROUTER_SSID));
	ssid[sizeof(TEST_ROUTER_SSID)] = '\0';
	config.ssid = ssid;

	return wifi_station_scan(&config, wifi_station_scan_cb);

}

FUN_ATTRIBUTE
void uart_recv_cb(char *buf, ra_size_t len)
{
	//char rebuf[MAXREBUF];
	//memcpy(rebuf,buf,len);
	//ra_uart_send_data(RA_UART0, buf, os_strlen(buf));
	//uart0_tx_buffer(buf, 1);
	if(len > 6)
		{
	if((buf[0]==0x24)&&(buf[6]==0x0a))
		{
			switch(buf[4])
				{
			case 0x01 :
				//ra_uart_send_data(RA_UART0, command, 7);  //做测试用
				restart=1;break;
			case 0x09 :
					os_printf("winner! \n");
					product_status_send_to_cloud(NULL,ra_true);
					gold_flag = 1;
					eyes_report_timer_flag = ra_true;
					break;
			/*case 0x02 :
				reforward = 1;break;
			case 0x03 :
				rebackward = 1;break;
			case 0x04 :
				releft = 1;break;
			case 0x05 :
				reright = 1;break;*/
			case 0x07 :
				reenter = 1;break;
			//case 0x21 :
			//	stop_reforward = 1;break;
			//case 0x22 :
		 	//	stop_rebackward = 1;break;
			//case 0x23 :
			//stop_releft = 1;break;
			//case 0x24 :
			//stop_reright = 1;break;
				}		
		}
		}
		else if(len < 2)
			{
				switch(buf[0])
					{
						case 0xa1 :
							reforward = 1;break;
						case 0xa2 :
							rebackward = 1;break;
						case 0xa3 :
							releft = 1;break;
						case 0xa4 :
							reright = 1;break;
						case 0xb1 :
							stop_reforward = 1;break;
						case 0xb2 :
							stop_rebackward = 1;break;
						case 0xb3 :
							stop_releft = 1;break;
						case 0xb4 :
							stop_reright = 1;break;
					}
			}
}

FUN_ATTRIBUTE
void Start_wifi_config_SendFrame(void)
{
	disconnect_from_server();
	wifi_station_disconnect();
	struct station_config config = { { 0 } };
	bool ret = wifi_station_set_config(&config);
	if (ret == false) { os_printf("REVEIVE_RESET"); }
	start_wifi_config(RA_WIFI_CONFIG_TYPE_HEKR);
}

FUN_ATTRIBUTE
void Start_catch(void)
{
		reforward = 0;rebackward = 0;releft = 0;reright = 0;restart = 0;
		stop_rebackward = 0;stop_reforward = 0;stop_releft = 0;stop_reright = 0;
}

FUN_ATTRIBUTE
void product_status_send_to_cloud(char *buf, ra_bool catchflag)
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
	cJSON_AddNumberToObject(data, "Get", catchflag);
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
	if (devtid) os_free(devtid);
	if (root) cJSON_Delete(root);
	return;
}

void report_timer_callback(ra_timer_t timer, void *arg)
{
	report_timer_flag = ra_false;

	if (eyes_report_timer_flag == ra_false)
	{
		product_status_send_to_cloud(NULL, ra_false);
		gold_flag = 1;
	}
	eyes_report_timer_flag = ra_false;
	/////product_status_send_to_cloud(NULL,catch_flag);
	//os_printf("test"); 
	//ra_gpio_output(direct, CLOSE);
	ra_timer_stop(timer);
	//ra_timer_delete(timer);
}

void timer_callback(ra_timer_t timer, void *arg)
{
    if((restart == 0)\
		&&(stop_rebackward == 0)&&(stop_releft== 0)&&(stop_reforward == 0)&&(stop_reright == 0))
		{
			ra_uart_send_data(RA_UART0, command, 7);
		}
	//ra_gpio_output(direct, CLOSE);
	//ra_timer_delete(timer_output);
	/////os_printf("timer1=%u",ra_timer_remaining_time(timer));
	ra_timer_stop(timer);
	//ra_timer_delete(timer);
	/////os_printf("timer2=%u", ra_timer_remaining_time(timer));
}

void send_flag_timer_callback(ra_timer_t timer, void *arg)
{
	send_flag = 1;
	ra_timer_stop(timer);
	//ra_timer_delete(timer);
}

void gold_flag_timer_callback(ra_timer_t timer, void *arg)
{
	gold_flag = 1;
	ra_timer_stop(timer);
	//ra_timer_delete(timer);
}

void reenter_timer_callback(ra_timer_t timer, void *arg)
{
	if(reenter == 0){command[4]=0x07;ra_uart_send_data(RA_UART0, command, 7);}
	//ra_timer_start(reenter_timer2, 500, ra_false);
	ra_timer_stop(timer);
	//ra_timer_delete(timer);
}

void reenter_timer2_callback(ra_timer_t timer, void *arg)
{
	if(reenter == 0){command[4]=0x07;ra_uart_send_data(RA_UART0, command, 7);}
	ra_timer_stop(timer);
	//ra_timer_delete(timer);
}

void zj_gpio_output(ra_uint8_t pin, ra_gpio_state_t value) 
{
	if (send_flag == 1)
	{
		ra_gpio_output(pin, value);
	}
}

FUN_ATTRIBUTE
ra_uint32_t product_task_cb(cJSON *params)
{
	ra_uint32_t code = CODE_SUCCESS;
	cJSON *data = params; //传入参数params是指向appSend中的data
	cJSON *cmdId = cJSON_GetObjectItem(data, "cmdId"); //获取命令Id
	cJSON *Steplength = cJSON_GetObjectItem(data, "Steplength"); //获取命令Id
    
	cJSON *Right = cJSON_GetObjectItem(data,"Right");
	cJSON *Left  = cJSON_GetObjectItem(data,"Left");
	cJSON *Forward = cJSON_GetObjectItem(data,"Forward");
	cJSON *Backward = cJSON_GetObjectItem(data,"Backward");
	/*
	ra_timer_t timer_output;
	ra_timer_new(&timer_output);
	ra_timer_set_callback(timer_output, timer_callback, NULL);
	
	ra_timer_t send_flag_timer;
	ra_timer_new(&send_flag_timer);
	ra_timer_set_callback(send_flag_timer, send_flag_timer_callback, NULL);

	ra_timer_t gold_flag_timer;
	ra_timer_new(&gold_flag_timer);
	ra_timer_set_callback(gold_flag_timer, gold_flag_timer_callback, NULL);

	ra_timer_t report_timer;
	ra_timer_new(&report_timer);
	ra_timer_set_callback(report_timer, report_timer_callback, NULL);
	*/

	if (cmdId == NULL) return CODE_ERROR_PARAMS;
	switch (cmdId->valueint)
	{

	case CMD_Query:

		break;

	case CMD_Set_Forward:
		if (send_flag == 1)
		{
		    switch(Forward -> valueint)
		    	{
		    	  case 1 :
				  	  //Start_catch();
				  	  command[4] = 0x03;
					  ra_uart_send_data(RA_UART0, command, 7);
	                  //ra_timer_start(timer_output, 450, ra_false);
					  break;
				  case 2:
				  	    os_printf("stopforward is request \n");
				  		//Start_catch();
				  		command[4]=0x22;
						ra_uart_send_data(RA_UART0, command, 7);
						//ra_timer_start(timer_output, 350, ra_false);
				 	    break;
		    	}
			//direct = 3;
			//ra_gpio_output(direct, OPEN);
			//ra_timer_start(timer_output, Steplength->valueint, ra_false);
		}
		break;
	case CMD_Set_Backward:
		if (send_flag == 1)
		{
		 	switch(Backward -> valueint)
		 		{
				case 1 :
					//Start_catch();
					command[4]=0x02;
					ra_uart_send_data(RA_UART0, command, 7);
					//ra_timer_start(timer_output, 450, ra_false);
					break;
		    	case 2 :
					os_printf("stopbackward is request \n");
					//Start_catch();
					command[4] = 0x21;
					ra_uart_send_data(RA_UART0, command, 7);
					//ra_timer_start(timer_output, 350, ra_false);
					break;
		 		}
			//direct = 5;
			//ra_gpio_output(direct, OPEN);
			//ra_timer_start(timer_output, Steplength->valueint, ra_false);
		}
		break;

	case CMD_Set_Left:
		if (send_flag == 1)
		{
			switch(Left -> valueint)
				{
				case 1 :
					//Start_catch();
					command[4] = 0x04;
					ra_uart_send_data(RA_UART0, command, 7);
					//ra_timer_start(timer_output, 450, ra_false);			
					break;
				case 2:
					os_printf("stopleft is request \n");
					//Start_catch();
					command[4] = 0x23;
					ra_uart_send_data(RA_UART0, command, 7);
					//ra_timer_start(timer_output, 350, ra_false);					
					break;
				}
			//direct = 12;
			//ra_gpio_output(direct, OPEN);
			//ra_timer_start(timer_output, Steplength->valueint, ra_false);
		}

		break;

	case CMD_Set_Right:
		if (send_flag == 1)
		{
			switch(Right -> valueint)
				{
				case 1 :
					//Start_catch();
					command[4]=0x05;
					ra_uart_send_data(RA_UART0, command, 7);
					//ra_timer_start(timer_output, 450, ra_false);					
					break;
				case 2 :
					os_printf("stopright is request \n");
					//Start_catch();
					command[4] = 0x24;
					ra_uart_send_data(RA_UART0, command, 7);
					//ra_timer_start(timer_output, 350, ra_false);			
					break;
				}
			//direct = 14;
			//ra_gpio_output(direct, OPEN);
			//ra_timer_start(timer_output, Steplength->valueint, ra_false);
		}
		break;

	case CMD_Set_Enter:
		if (send_flag == 1)
		{
			//direct = 15;
			//ra_gpio_output(direct, OPEN);
			//ra_timer_start(timer_output, 130, ra_false);
			os_printf("Enter ok \n");
			reenter = 0;
			command[4]=0x07;
			ra_uart_send_data(RA_UART0, command, 7);
			ra_timer_start(reenter_timer, 1000, ra_false);
			
					
			report_timer_flag = ra_true;
			ra_timer_start(report_timer, 10000, ra_false);
		}
		//ra_timer_delete(report_timer);
		break;
		
	case CMD_Set_Coin:

		if (gold_flag == 1)
		{
			//direct = 1;
			//direct = 2;    //注释之前
			//direct = 16;
			//ra_gpio_output(direct, OPEN);
			//ra_timer_start(timer_output, 130, ra_false);
			//Start_catch();
			command[4]=0x01;
			ra_uart_send_data(RA_UART0, command, 7);
			ra_timer_start(timer_output, 350, ra_false);

			gold_flag = 0;
			ra_timer_start(gold_flag_timer, 27000, ra_false);
		}

		break;
	/*	
	case CMD_Set_steplength:

		switch (Steplength->valueint)
		{
		case 1:
			DELAY_TIME = 100;
			break;
		case 2:
			DELAY_TIME = 200;
			break;
		case 4:
			DELAY_TIME = 400;
			break;
		}

		break;
*/
	default:
		code = CODE_ERROR_CMDID_INVALID;
		goto fail;
		break;
	}
	if (send_flag == 1)
	{
		send_flag = 0;
		ra_timer_start(send_flag_timer, SEND_TIME, ra_false);//发送倒计时200ms
		//ra_timer_start(send_flag_timer, 2500, ra_false);
	}
	goto done;
done:
fail:
	return code;
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
																	  //ra_register_appsend_finished_callback(report_device_status); //appsend处理完成后，上报一下设备状态
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
void  product_key_change(void)
{
    ra_uart_send_data(RA_UART0, command, 7);
    ////product_status_send_to_cloud(NULL,report_timer_flag);
}

FUN_ATTRIBUTE
void  catch_change(void)
{
	if (report_timer_flag == 1)
	{
		product_status_send_to_cloud(NULL,ra_true);
		gold_flag = 1;
		eyes_report_timer_flag = ra_true;
	}
	////catch_flag = ra_true;
	//product_status_send_to_cloud(NULL);
	
	/*
	if (key_test == 0)
	{
		key_test = 1;
		//ra_gpio_output(5, OPEN);
		ra_gpio_output(14, OPEN);
	}
	else
		if (key_test == 1)
		{
			key_test = 0;
			//ra_gpio_output(5, CLOSE);
			ra_gpio_output(14, CLOSE);
		}
		*/
}

FUN_ATTRIBUTE
void product_hardware_init(void)
{
	ra_timer_new(&timer_output);                                               //上下左右下发定时器
	ra_timer_set_callback(timer_output, timer_callback, NULL);

	ra_timer_new(&send_flag_timer);                                            //抓取下发定时器
	ra_timer_set_callback(send_flag_timer, send_flag_timer_callback, NULL);    

	ra_timer_new(&gold_flag_timer);                                            //投币倒计时定时器
	ra_timer_set_callback(gold_flag_timer, gold_flag_timer_callback, NULL);

	ra_timer_new(&report_timer);                                               //光眼信号上报定时器
	ra_timer_set_callback(report_timer, report_timer_callback, NULL); 

	ra_timer_new(&reenter_timer);                                               //抓定时器1
	ra_timer_set_callback(reenter_timer, reenter_timer_callback, NULL); 
	
	ra_timer_new(&reenter_timer2);                                               //抓定时器2
	ra_timer_set_callback(reenter_timer2, reenter_timer2_callback, NULL);
	
	ra_register_button_irq_handler(
		PRODCT_CONFIG_PIN,
		RA_GPIO_IRQ_TYPE_NEGEDGE,  //上升沿触发
		//RA_GPIO_IRQ_TYPE_POSEDGE,//下降沿触发
		3000,
		(ra_button_pressed_callback_t)product_key_change,
		(ra_button_pressed_callback_t)Start_wifi_config_SendFrame
	);
	
	/* ra_register_button_irq_handler(
		CATCH_PIN,
	    RA_GPIO_IRQ_TYPE_NEGEDGE,  //上升沿触发
		//RA_GPIO_IRQ_TYPE_POSEDGE,//下降沿触发
		3000,
		(ra_button_pressed_callback_t)catch_change,
		(ra_button_pressed_callback_t)Start_catch
	); */
	
	 //ra_gpio_set_pullup(PRODUCT_POWR_PIN, 1);
	ra_device_state_led_task_install(PRODUCT_NETWORK_STATE_LED_PIN, 0); //注册设备网络状态指示灯
	ra_uart_set_rate(RA_UART0, RA_UART_RATE_9600);
	ra_uart_set_data_bits(RA_UART0, RA_UART_DATA_BITS_8);
	ra_uart_set_stop_bits (RA_UART0, RA_UART_STOP_BITS_1);
	ra_uart_set_parity(RA_UART0,0x00);   
	ra_uart_register_recv_callback(RA_UART0, uart_recv_cb);
	ra_uart_recv_enable(RA_UART0);
	/*ra_gpio_set_direction(3, RA_GPIO_DIRECTION_OUTPUT);
	ra_gpio_set_direction(5, RA_GPIO_DIRECTION_OUTPUT);
	ra_gpio_set_direction(12, RA_GPIO_DIRECTION_OUTPUT);
	ra_gpio_set_direction(14, RA_GPIO_DIRECTION_OUTPUT);
	ra_gpio_set_direction(15, RA_GPIO_DIRECTION_OUTPUT);

	//ra_gpio_set_direction(2, RA_GPIO_DIRECTION_OUTPUT);
	ra_gpio_set_direction(2, RA_GPIO_DIRECTION_INPUT);
	ra_gpio_set_direction(1, RA_GPIO_DIRECTION_INPUT);//    抓完信号输入判断 

	//ra_gpio_set_direction(16, RA_GPIO_DIRECTION_OUTPUT);
	  
	ra_gpio_output(3, CLOSE);
	ra_gpio_output(5, CLOSE);
	ra_gpio_output(12, CLOSE);
	ra_gpio_output(14, CLOSE);
	ra_gpio_output(15, CLOSE);

	ra_gpio_output(2, CLOSE);
	ra_gpio_output(1, CLOSE);
	//ra_gpio_output(16, CLOSE);

	//wifi_set_opmode(STATION_MODE); */

}

FUN_ATTRIBUTE
void zj_factorytest(void *arg)
{
	{
		ecr_init(&g_factory_config.ecr_line, &g_factory_config.timer);
		ecr_begin(&zj_factorytest, NULL);
		bool ret = factory_router_scanning();
		//os_printf("testret= %u",ret); 
		if (ret == false) goto done;

		while (1)
		{
			if (router_scan_finished_flag == true)
			{
				goto done;
			}
			ecr_delay_ms(200);
		}
		ecr_finish_noret;
	}
	goto done;

done:
	if (router_exist_flag == true)
	{
		factory_test_flag = true;
	}
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

FUN_ATTRIBUTE inline
static int wifi_config_clean(void)
{
	int32_t count = 0;

	int ret = ra_get_parameter_integer(COUNTS, &count);
	if (ret == 0)
	{
		count++;
		if (count >= 3)
		{
			count = 0;
			ra_set_parameter_integer(COUNTS, count);

			struct station_config config = { { 0 } };
			wifi_station_set_config(&config);
			return 0;
		}
		ra_set_parameter_integer(COUNTS, count);
	}
	else
	{
		count = 1;
		ra_set_parameter_integer(COUNTS, count);
	}

	return -1;
}

FUN_ATTRIBUTE
void ra_user_main(void)
{
	product_hardware_init();
	//int ret = wifi_config_clean();
	ra_set_parameter_string("prodKey", "a951cbee879ce0cd3243ccf0869cc5c5");	//设置prodKey, prodKey时在console平台上创建产品时获取
	ra_set_current_firmware_version(BIN_VERSION); //固件版本号设置，格式必须是"xx.xx.xx.xx"
	ra_uart_set_terminal(RA_UART1); //设置打印终端，可将调试信息输出至GPIO2
	//ra_uart_set_rate(RA_UART0, 9600);
    //ra_uart_set_data_bits(RA_UART0, 8);
	//ra_uart_set_terminal(RA_UART0); //设置打印终端，可将调试信息输出至GPIO1
	//ra_uart_register_recv_callback(RA_UART0, uart_recv_cb);
	//ra_uart_recv_enable(RA_UART0);
	ra_register_device_state_changed_callback(device_state_changed_callback); //注册设备状态改变时的回调函数

	ra_register_system_init_done_callback(zj_factorytest, NULL);

}
