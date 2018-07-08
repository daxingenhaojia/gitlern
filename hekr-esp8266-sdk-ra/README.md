#氦氪4.x嵌入式SDK应用手册
**v1.1.4 by [zejun.zhao@hekr.me](zejun.zhao@hekr.me)** 2016/3/30 11:03:38   

##总体描述
- 编写目的： 该文档旨在指导开发者快速上手氦氪4.x嵌入式SDK，进行对WIFI模块ESP8266的二次开发，熟悉目录结构、了解api接口各模块功能，以及编程中需注意的事项。

- SDK开发者：基于esp8266硬件平台，采用Hekr配网模式、连接氦氪云进行开发的开发者群体。同时，开发者也无需关注网络重连及服务器重连机制，便于开发者进行二次应用开发。

- api接口使用说明，请参考文档`document/ra_embedded_api.chm`

## 1. SDK目录

---

###### 目录结构

	├── app
	| 	 └─ user
	├── bin
	|	 └─ upgrade
	├── document
	├── include
	|	  └─ ra
	├── ld
	├── lib
	└── tools

###### 目录结构说明

- `app`：用户工作区。用户在此目录下执行make编译操作，用户级代码及头文件均放在此目录下
	- `user`：用户级代码目录，SDK入口函数为`ra_user_main()`,在`user_main.c`中
- `bin`：二进制文件目录。该目录存放了编译生成的bin文件
	- `upgrade`：该子目录存放编译生成的支持云端升级（FOTA）的固件（如`user1.bin`或`user2.bin`）
- `document`：文档目录
- `include`：头文件目录，包含了用户可使用的API函数声明以及相关宏定义
	- `ra`：RA库头文件目录，开发者进行二次开发所需API函数、宏定义、类型定义均在此目录头文件中
- `ld`：SDK编译链接时所需文件，用户无需修改
- `lib`：SDK编译链接时所需的库文件。
	- `libra.a`	 
- `tools`：包含编译工具和烧录工具等，用户无需修改

## 2. 固件编译

---

- 在`app`目录下，执行编译命令`make`
```
注：make编译时，请根据具体SPI flash类型设定相应参数，参数值默认为BOOT=new、APP=1、SPI_SPEED=40、SPI_MODE=QIO、SPI_SIZE_MAP=3
```

## 3. 固件烧录

---
- flash size不同，flash map亦不同，具体请参考esp8266官方 `NON_RTOS_SDK`用户手册。
- `params_section_blank.bin`用于擦除flash中的所有配置参数（如`prodKey`等）以及`ra_set_parameter_string/ra_set_parameter_integer`接口存入的参数，烧录地址总为用户参数区的第二个扇区起始地址，烧录地址取决于flasp map，即`SPI_SIZE_MAP`。

<table class="table table-bordered table-striped table-condensed">
	<tr>
 		<td>SPI_SIZE_MAP</td>
 		<td>烧录地址</td>
	</tr>
	<tr>
 		<td>0</td>
 		<td>0x3D000</td>
	</tr>
	<tr>
 		<td>2/3/4</td>
 		<td>0x7D000</td>
	</tr>
	<tr>
 		<td>5/6</td>
 		<td>0xFD000</td>
	</tr>
</table>


## 4. 用户代码入口

---
- 用户代码入口函数为`ra_user_main()`,在`app/user/user_main.c`中。
- 在进入函数`ra_user_main()`前，系统已经做了部分初始化，具体有：CPU频率为`80MHz`、wifi mode为`STATION_MODE`、设定uart0和uart1波特率为`9600`且打印终端为`uart0`。

此外，在`ra_user_main()`中，需要执行一些函数完成对配置初始化，请参考以下代码：

	#include <ra_parameter.h>
	#include <ra_utils.h>
	#include <ra_uart.h>

	void ra_user_main(void)
	{
		//必选
		ra_set_parameter_string("prodKey", "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");	//prodKey设置
		ra_set_current_firmware_version("4.1.1.1");	//固件版本号设置，格式必须是"xx.xx.xx.xx"

		//可选
		ra_uart_set_terminal(RA_UART1); //设置打印终端，可将SDK内部调试信息输出
	}

## 5. 功能模块

---
###5.1 打印终端配置

esp8266有两个串口（uart0和uart1），均可作为打印终端，调用`ra_uart_set_terminal()`。`os_printf`的打印终端默认为uart0。

示例代码：(设置uart1为打印终端)

    #include <ra_uart.h>
    
	ra_uart_set_terminal(RA_UART1);
	os_printf("hello world\n");

uart1输出：

	hello world

**注：`ra_uart_set_terminal()`设置后，SDK内部调试信息也会打印出来，如云端登陆流程的信息，方便开发者调试**

串口相关属性配置可参考：**5.5.1 UART操作接口**


###5.2 用户参数存取接口

SDK提供K/V格式的存参接口。开发者可自定义Key，并可选择两种Value类型（int和string）。此外还提供了K/V的读取和删除接口。接口说明请参考`ra_parameter.h`。

- Key最大长度为32Byte，string类型的Value最大长度为64Byte。
- 开发者请勿使用这三个Key，分别是`ctrlKey`、`bindKey`和`token`，Hekr云端登录机制有使用到它们。
- 此接口旨在用于存储用户参数，不建议用于存放用户数据。用户数据请使用底层`spi_flash_write()`接口。

示例代码：

	#include <ra_parameter.h>

	int ret = -1;
	char *key_1 = "key_1";
	char *key_2 = "key_2";
	ra_int32_t value_1 = 1000, value_int = 0;
	char *value_2 = "12345678123456781234567812345678";
	char value_str[64];

	//store params
	ra_set_parameter_integer(key_1, value_1);
	ra_set_parameter_string(key_2, value_2);

	//load params
	ra_get_parameter_integer(key_1, &value_int);
	ret = ra_get_parameter_string(key_2, value_str, 64);

	os_printf("value_int = %d\n", value_int);
	os_printf("ret = %d, value_str = %s\n", ret, value_str);

	//delete key_2
	ra_delete_parameter(key_2);
	os_memset(value_str, 0, 64);
	ret = ra_get_parameter_string(key_2, value_str, 64);
	os_printf("ret = %d, value_str = %s\n", ret, value_str);

uart1输出：

	value_int = 1000
	ret = 32, value_str = 12345678123456781234567812345678
	[log]ra_parameter.c:173: key_2 isn't exist //此为SDK内部打印
	
	ret = -1, value_str =


###5.2 GPIO接口

#####5.2.1 逻辑引脚和物理引脚映射

SDK中gpio逻辑序号与esp8266的gpio物理序号一致。如gpio逻辑序号为12对应GPIO12，而它的引脚名称为`MTDI_U`

#####5.2.2 配置GPIO为输出

配置GPIO14引脚为输出，且输出高电平：

示例代码：

    #include <ra_gpio.h>
    
	ra_gpio_set_direction(14, RA_GPIO_DIRECTION_OUTPUT);
	ra_gpio_output(14, 1);


#####5.2.3 配置GPIO为输入

配置GPIO13引脚为输入，且输出高电平：

示例代码：

    #include <ra_gpio.h>

	ra_gpio_set_direction(13, RA_GPIO_DIRECTION_INPUT);
	ra_gpio_state_t state = ra_gpio_input(13);
	os_printf("gpio13 = %d\n", state);


###5.3 按键接口

SDK中基于gpio封装了注册按键中断处理回调接口，输入参数包括gpio序号、gpio中断类型、长按时间阈值、短按处理函数指针以及长按处理函数指针。

示例代码：

按键管脚为序号为13的管脚，下降沿触发，长按阈值为3000ms，短按处理函数指针指向`short_press_handler()`，长按处理函数指针指向`long_press_handler()`。

    #include <ra_button.h>
    
	void short_press_handler(void)
	{
		os_printf("short press\n");
	}
	
	void long_press_handler(void)
	{
		os_printf("long press\n");
	}
	
	void ra_user_main(void)
	{
		ra_register_button_irq_handler(13, RA_GPIO_IRQ_TYPE_NEGEDGE, 3000, short_press_handler, long_press_handler);
	}

gpio13电平拉低3s，则进入长按处理回调函数，打印`long press`;gpio13电平拉低短于3s，则进入短按处理回调函数，打印`short press`。

- 注：中断类型推荐使用下降沿触发`RA_GPIO_IRQ_TYPE_NEGEDGE`


###5.4 定时任务接口

SDK中定时接口是对底层api进行的二次封装，且增加了一些新特性，如查询定时任务剩余时间、查询定时任务是否可重复。

#####5.4.1 定时任务新建和启动

示例代码：

注册一个每隔2s重复执行的定时任务，每次执行打印输出`hello world`

    #include <ra_timer.h>
    
    void timer_callback(ra_timer_t timer, void *arg)
    {
    	os_printf("hello world\n");
    }
    
    void ra_user_main(void)
    {
    	ra_timer_t timer; 
    	ra_timer_new(&timer);
    	ra_timer_set_callback(timer, timer_callback, NULL);
    	ra_timer_start(timer, 2000, ra_true);
    }

- 注：`ra_timer_start()`第三个参数值为`ra_false`时，定时任务不可重复

#####5.4.2 定时任务的停止与注销

`ra_timer_stop()`：定时器停止工作，但不释放定时器timer。无需调用`ra_timer_new()`来新建定时器


`ra_timer_delete()`：定时器停止工作，且释放定时器timer。


###5.5 UART接口

SDK可以设置UART0或者UART1作为串口输出，UART0的TXD、RXD管脚分别对应ESP8266的TXD、RXD引脚，UART1对应的TXD管脚为GPIO2。

- 注：esp8266的串口接收中断仅对UART0有效，即`ra_uart_recv_enable()`只支持`RA_UART0`。

#####5.5.1 UART属性设置

UART可设置的属性包括波特率、数据位、奇偶校验位、停止位。（注：底层esp8266暂不支持奇偶校验）。默认属性为`9600`、`8`、`无`、`1`。

示例代码：

设置UART0波特率`115200`、数据位`7`，并从UART0重复输出`hello world`，同时注册串口接收回调函数。

	#include <ra_timer.h>
	#include <ra_uart.h>
	
	void timer_callback(ra_timer_t timer, void *arg)
	{
		char *string = "hello world";
		ra_uart_send_data(RA_UART0, string, os_strlen(string));
	}
	
	void uart_recv_cb(char *buf, ra_size_t len)
	{
		os_printf("%s, len = %d\n", buf, len);
	}
	
	void ra_user_main(void)
	{
		//设置UART0属性
		ra_uart_set_rate(RA_UART0, RA_UART_RATE_115200);
		ra_uart_set_data_bits(RA_UART0, RA_UART_DATA_BITS_7);
	
		//定时从UART0发送
		ra_timer_t timer;
		ra_timer_new(&timer);
		ra_timer_set_callback(timer, timer_callback, NULL);
		ra_timer_start(timer, 3000, 1);
	
		//设置UART0的数据接收回调函数
		ra_uart_register_recv_callback(RA_UART0, uart_recv_cb);
		//UART0接收中断使能
		ra_uart_recv_enable(RA_UART0);
	}

#####5.5.2 UART通信配置

UART发送配置：

`ra_uart_set_sending_queue_interval()`：用于设置串口数据发送队列间隔，默认为100ms。当发送数据帧太快，数据帧会缓存在队列中，然后以一定的间隔时间依次发送出去。

UART接收配置：

`ra_uart_set_recv_timeout()`：设置串口接收中断超时时间，默认为100ms。UART中断接收数据，如果超时时间内没有再接收到数据，则进入串口接收回调函数。
	
`ra_uart_set_recv_buffer_size_limit()`：设置串口接收回调触发长度，默认为256Byte。UART中断接收数据，当buffer长度达到设定值，则进入串口接收回调函数。

- 进入串口回调函数的触发条件由上面两个函数设置的参数决定，满足其中一个条件即进入回调函数。


###5.6 设备状态接口	

#####5.6.1 设备状态的读写操作

为了方便开发者对设备状态进行监控及控制，SDK中使用了设备状态机制。开发者可以根据设备当前实际状态，将对应的设备状态项`item`的值置为`ra_true`，待实际状态发生改变时，将该`item`置为`ra_false`。同时，开发者可在任意时候通过读取某`item`的值来判断设备是否处于该状态下。

	typedef enum
	{
		RA_DEVICE_STATE_WLAN_CONNECTED = 0, //路由器连接成功
		RA_DEVICE_STATE_WLAN_CONNECT_FAILED, //路由器连接失败
		RA_DEVICE_STATE_CLOUD_CONNECTED, //服务器连接成功
		RA_DEVICE_STATE_HEKR_CONFIG_RUNNING, //Hekr config模式下
		RA_DEVICE_STATE_AIRKISS_CONFIG_RUNNING, //airkiss config模式下
	} ra_device_state_type_t;

`ra_device_state_type_t`枚举定义中包含了开发者可能用到的设备状态项，可以对它们进行`store`（设置）和`load`（读取）操作。

`ra_device_state_store()`：设置某个设备状态项`item`的值。开发者须根据设备实际状态来设置对应`item`值。

	ra_device_state_store(RA_DEVICE_STATE_WLAN_CONNECTED, ra_true); 
	//设备连接路由器成功，设置item "RA_DEVICE_STATE_WLAN_CONNECTED"为ra_true。

`ra_device_state_load()`：读取某个设备状态项`item`的值。根据该`item`值来判断设备当前状态，以便进行逻辑处理。

	ra_device_state_load(RA_DEVICE_STATE_WLAN_CONNECTED); 
	//根据返回值判断设备是否连接成功路由器，ra_true表示成功，ra_false表示失败。

- 开发者在实际应用中，`ra_device_state_type_t`中已有的设备状态项的`store`操作已经在SDK实现，开发者只需通过`load`操作来获取设备状态。
- 开发者可在`ra_device_state_type_t`枚举定义中添加其他设备状态项，它的`store`和`load`操作则自行实现。<span style="color:red;">注：设备状态项请添加在已有项的后面</span>。
- 所有设备状态项的初始值都是`ra_false`。

#####5.6.2 设备状态改变时的回调函数

为方便开发者快速接入氦氪云端，SDK中提供了wifi连接及服务器登陆的接口，在此过程中，设备状态会出现多种变化（如路由器连接成功、路由器连接失败、服务器登陆成功、服务器登陆失败等）。SDK中提供了设备状态改变的回调函数，当设备状态改变时（如连接上路由器）开发者可以根据`item`的值来判断设备当前状态，进行分支操作。

示例代码：<span style="color:red;">可参考5.8节的相关示例</span>

为了不影响当前设备状态项，在`ra_device_state_type_t`枚举定义中后面添加一条设备状态 `RA_DEVICE_STATE_DO_ACTION`

	#include <ra_device_state.h>
	#include <ra_utils.h>
	
	void device_state_changed_callback(ra_device_state_type_t item, ra_bool current_state)
	{
		if (item == RA_DEVICE_STATE_DO_ACTION)
		{
			os_printf("RA_DEVICE_STATE_DO_ACTION. state = %d\n", current_state);
		}
	}

	void ra_user_main(void)
	{
		ra_register_device_state_changed_callback(device_state_changed_callback);//注册设备状态改变时的回调函数
	
		if (ra_device_state_load(RA_DEVICE_STATE_DO_ACTION) == ra_false)
		{
			ra_device_state_store(RA_DEVICE_STATE_DO_ACTION, ra_true);
		}
	}

uart1输出：
	
	RA_DEVICE_STATE_DO_ACTION. state = 1


###5.7 wifi配网接口

- 配网方式有：`Hekr config`和`airkiss config`（暂不支持）

调用配网接口`ra_start_wifi_config(RA_WIFI_CONFIG_TYPE_HEKR)`即进入`Hekr config`模式，此时使用Hekr APP对设备进行一键配网，设备获取到ssid和password即进入配网处理回调函数中。

示例代码：

	#include <ra_wifi.h>
	
	void wifi_config_finish_callback(char *ssid, char *password)
	{
		if ((ssid != NULL) && (password != NULL))
		{
			os_printf("wifi_config successed\n");
			os_printf("ssid = %s, password = %s\n", ssid, password);
			//配网成功，获取到ssid及password，接着连接路由器
		}
		else
		{
			os_printf("wifi_config failed\n");
			//配网失败
		}
	}
	
	void ra_user_main()
	{
		os_printf("wifi_config start\n");
		ra_register_wifi_config_callback(wifi_config_finish_callback);//注册wifi config配网回调函数
		ra_start_wifi_config(RA_WIFI_CONFIG_TYPE_HEKR);//启动wifi config
	}

- 串口打印`********hekr_config run********`即表示进入配网模式。
- 配网超时时间为5min，超时后也进入配网回调函数，此时ssid和password均为NULL。
- 配网获取到的ssid和password未保存，用户可自行调用`wifi_station_set_config()`进行存储，避免重复配网。
- 进行配网前，用户可以通过`wifi_station_get_config(struct station_config *config)`接口的config参数来判断是否需要进入配网模式。



###5.8 设备连接wifi及登陆服务器

设备须先连接上wifi路由器，然后进行登陆服务器操作。开发者可以使用`hekr config`一键配网获取ssid，也连接一个固定的ssid。以下示例配置固定的ssid。

示例代码：

	#include <ra_wifi.h>
	#include <ra_device_state.h>
	#include <ra_utils.h>
	
	void recv_cloud_msg_callback(char *buf, ra_uint16_t len)
	{
		os_printf("buf = %s, len = %d\n", buf, len);
		//接收到云端数据
	}
	
	void device_state_changed_callback(ra_device_state_type_t item, ra_bool current_state)
	{
		if (item == RA_DEVICE_STATE_WLAN_CONNECTED&&
			current_state == ra_true)
		{
			//路由器连接成功，接着登录服务器
			os_printf("wifi connect successed\n");
			ra_register_cloud_recv_callback(recv_cloud_msg_callback);//注册登录云端后，数据接收的回调函数
			ra_connect_to_cloud();//登录云端服务器
		}
		else if (item == RA_DEVICE_STATE_WLAN_CONNECT_FAILED&&
			current_state == ra_true)
		{
			os_printf("wifi connect failed\n");
		}
	}
	
	void ra_user_main(void)
	{
		ra_register_device_state_changed_callback(device_state_changed_callback);//注册设备状态改变时的回调函数
		ra_connect_wifi("WDD_TEST", "56781234", NULL, 30 * 1000);
	}

- `ra_connect_wifi()`中第四个参数为wifi连接的超时时间，超时后设备停止wifi连接，并进入到设备状态改变的回调函数中，此时的`item == RA_DEVICE_STATE_WLAN_CONNECT_FAILED && current_state == ra_true`。
- 设备登陆服务器后就可以进行开发者自己的业务实现，同时，wifi连接异常及服务器连接异常处理开发者无需自行实现，SDK中集成了wifi重连及服务器重连机制。
- 设备登陆服务器时需要`prodKey`（在HEKR console平台创建产品时生成，长度为32 Bytes），请在`ra_user_main()`中调用`ra_set_parameter_string()`来设置，如`ra_set_parameter_string("prodKey", "12345678123456781234567812345678");`。


###5.9 UDP局域网接口

SDK提供了ra_lan_comm_send接口将数据发送到指定IP的指定端口（port:10000），需要先注册UDP接收回调函数，同时启动UDP server。

示例代码：

	#include <ra_lan_comm.h>

	void lan_recv_cb(ra_remote_info_t *remote, char *data, ra_uint16_t size)
	{
		os_printf("remote ip:%u:%u:%u:%u port:%u\n",
			remote->remote_ip[0],
			remote->remote_ip[1],
			remote->remote_ip[2],
			remote->remote_ip[3],
			remote->remote_port);
	
		os_printf("data = %s\n", data);
		ra_lan_comm_send(remote, data, size); //send data to remote
	}
	
	void ra_user_main()
	{
		ra_lan_comm_register_recv_callback(lan_recv_cb); //注册UDP数据接收回调函数
		ra_lan_comm_server_start(); //启动UDP server
	
		//连接wifi
		ra_connect_wifi("WDD_TEST", "56781234", NULL, 30 * 1000);
	}


###5.10 OTA升级

esp8266的OTA升级方式将flash map分为A/B两个区，固件默认烧录到A区，OTA升级将新固件下载到B区，升级完成后系统重启从B区启动。再次升级时则固件下载至A分区，并从A分区启动，依次更替。

OTA升级指令（action为`devUpgrade`）是由云端主动下发，用户需要将指令中参数URL、MD5和固件类型提取出来，然后调用`ra_start_dev_upgrade()`启动OTA升级。用户在解析`devUpgrade`指令时可以不校验参数，`ra_start_dev_upgrade()`内部会进行校验，如果参数无误，则返回值为0，向云端回应devUpgradeResp时code为200；如果参数有误，则返回值为非0，向云端回应devUpgradeResp时code为非200（自行定义）。

示例代码：

	void OTA_successful_callback(void)
	{
		os_printf("OTA_successful!\n");

		//升级成功后,要执行ra_system_upgrade_reboot()重启完成升级
    	ra_timer_t timer; 
    	ra_timer_new(&timer);
    	ra_timer_set_callback(timer, ra_system_upgrade_reboot, NULL);
    	ra_timer_start(timer, 2*1000, ra_false);
	}
	
	void OTA_fail_callback(int error_code)
	{
		os_printf("OTA_fail!\n");
	}
	
	void ra_user_main()
	{
		int ret; 
		ra_register_dev_upgrade_success_callback(OTA_successful_callback); //注册OTA成功回调函数
		ra_register_dev_upgrade_failed_callback(OTA_fail_callback); //注册OTA失败回调函数
		ret = ra_start_dev_upgrade(dev_TID, URL, MD5, B); //启动模块OTA升级，OTA的固件类型为B
		if(ret == 0)
		{
			//上报devUpgradeResp，code为200
		}
		else
		{
			//上报devUpgradeResp，code为非200
		}
	}

- OTA升级成功后进入升级成功回调函数需调用`ra_system_upgrade_reboot()`，此时系统会重启，方可正常运行新版本固件。
- OTA进度上报在SDK中集成了，无需用户单独编程。

###5.11 浮点打印的解决方案

由于esp8266 SDK并不支持浮点打印（包括`printf`及`sprintf`），氦氪在此基础上移植了一种变通解决方案来支持浮点打印。为了区分原生api，支持浮点的api命名为`c_printf`和`c_sprintf`，在`#include <c_sprintf.h>`中。

- SDK中的cJSON也支持浮点。


## 6. 编码注意事项

---
- `include/ra`目录下提供的api函数均是以`ra_`为前缀命名，是该SDK主推供开发者调用的API接口。由于该SDK本身是基于原生`ESP8266 NON_RTOS SDK`深度开发而来，故ESP8266底层API也对开发者开放可供调用，但我们不推荐开发者使用底层API，以免造成RA接口的失效。`ra_`前缀api使用说明请参考`ra_embedded_api.chm`文档。
- 占用CPU时间超过`500ms`的函数不要在`ra_user_main()`直接调用，推荐在系统初始化完成后。可在入口函数中调用`ra_register_system_init_done_callback()`来注册系统初始化完成的回调函数。然后在回调函数中调用敏感api。
- 敏感API均为网络相关API，有`ra_disconnect_wifi()`。
- 设备接收到云端的数据都是操作指令（如appSend、devUpgrade、devSendResp等）对应的JSON字符串，用户需要自行解析成JSON格式，SDK中已经集成了cJSON接口，请参考头文件cJSON.h。

###### 修饰符说明

- `FUN_ATTRIBUTE`：函数修饰符。函数前不加它，则上电启动时将函数代码从`flash`加载到`iram`；在函数前加上它，则在上电启动后函数被调用时才被加载到`iram`中。由于芯片`iram`空间有限，编码时请在函数前加上此修饰符，但不能给中断回调函数添加（定时器回调函数除外）。
- `RODATA_ATTRIBUTE`：变量修饰符。在变量（包括数组）定义前添加它，它将被写入flash中，减少系统运行期间堆的占用，只在变量使用时从flash中读取到`ram`中。


## Change Log

`V1.0.2`: SDK更新至v1.5.4.1 ,优化心跳机制