/* ESP8266 API Emulator Library : WIFI
 * Copyright(c) 2016 Hangzhou District Nine Technology Co., Ltd.  */

#include "libesp8266emu.h"

extern emu_task_queue_t *g_task_queue;

typedef enum 
{
    EMU_WIFI_OPMODE_STATION = 0x01,
    EMU_WIFI_OPMODE_SOFTAP = 0x02,
    EMU_WIFI_OPMODE_STATION_SOFTAP = 0x03,
} emu_wifi_opmode_t;

struct emu_wifi
{
    /* Current Mode */
    emu_wifi_opmode_t opmode;
    /* Station configurations */
    struct station_config station_config;
};

/* Settings includes a live version (the setting now working) and
 * a flash version, everytime we change the setting in the flash, 
 * we write all settings back into file system */
static struct emu_wifi g_wifi_live;
static wifi_event_handler_cb_t g_wifi_event_handler = NULL;

static struct emu_wifi g_wifi_flash;

uint8 wifi_get_opmode(void)
{
	return (uint8)g_wifi_live.opmode;
}

uint8 wifi_get_opmode_default(void)
{
	return (uint8)g_wifi_flash.opmode;
}

bool wifi_set_opmode(uint8 opmode)
{
    g_wifi_live.opmode = opmode;
    g_wifi_flash.opmode = opmode;
    /* TODO: Write the setting back to file system */
	return true;
}

bool wifi_set_opmode_current(uint8 opmode)
{
    g_wifi_live.opmode = opmode;
	return true;
}

bool wifi_station_get_config(struct station_config * config)
{
    memcpy(config, &g_wifi_live.station_config, sizeof(struct station_config));
	return true;
}

bool wifi_station_get_config_default(struct station_config * config)
{
    memcpy(config, &g_wifi_flash.station_config, sizeof(struct station_config));
	return true;
}

bool wifi_station_set_config(struct station_config * config)
{
    memcpy(&g_wifi_live.station_config, config, sizeof(struct station_config));
    memcpy(&g_wifi_flash.station_config, config, sizeof(struct station_config));
    /* TODO: Apply Settings */
	return true;
}

bool wifi_station_set_config_current(struct station_config * config)
{
    memcpy(&g_wifi_live.station_config, config, sizeof(struct station_config));
    /* TODO: Apply Settings */
	return true;
}

bool wifi_station_connect(void)
{
    emu_task_t *new_task = NULL;

    if ((new_task = emu_task_new_wifi_station_connect()) == NULL)
    { return false; }
    emu_task_queue_push_back(g_task_queue, new_task);

	return true;
}

bool wifi_station_disconnect(void)
{
    emu_task_t *new_task = NULL;

    if ((new_task = emu_task_new_wifi_station_connect()) == NULL)
    { return false; }
    emu_task_queue_push_back(g_task_queue, new_task);

	return true;
}

sint8 wifi_station_get_rssi(void)
{
	return 0;
}

bool wifi_station_scan(struct scan_config * config, scan_done_cb_t cb)
{
	return false;
}

uint8 wifi_station_get_auto_connect(void)
{
	return 0;
}

bool wifi_station_set_auto_connect(uint8 set)
{
	return false;
}

bool wifi_station_set_reconnect_policy(bool set)
{
	return false;
}

uint8 wifi_station_get_connect_status(void)
{
	return 0;
}

uint8 wifi_station_get_current_ap_id(void)
{
	return 0;
}

bool wifi_station_ap_change(uint8 current_ap_id)
{
	return false;
}

bool wifi_station_ap_number_set(uint8 ap_number)
{
	return false;
}

uint8 wifi_station_get_ap_info(struct station_config config[])
{
	return 0;
}

bool wifi_station_dhcpc_start(void)
{
	return false;
}

bool wifi_station_dhcpc_stop(void)
{
	return false;
}

enum dhcp_status wifi_station_dhcpc_status(void)
{
	return DHCP_STOPPED;
}

bool wifi_station_dhcpc_set_maxtry(uint8 num)
{
	return false;
}

char * wifi_station_get_hostname(void)
{
	return NULL;
}

bool wifi_station_set_hostname(char * name)
{
	return false;
}

bool wifi_softap_get_config(struct softap_config * config)
{
	return false;
}

bool wifi_softap_get_config_default(struct softap_config * config)
{
	return false;
}

bool wifi_softap_set_config(struct softap_config * config)
{
	return false;
}

bool wifi_softap_set_config_current(struct softap_config * config)
{
	return false;
}

uint8 wifi_softap_get_station_num(void)
{
	return 0;
}

struct station_info * wifi_softap_get_station_info(void)
{
	return NULL;
}

void wifi_softap_free_station_info(void)
{
}

bool wifi_softap_dhcps_start(void)
{
	return false;
}

bool wifi_softap_dhcps_stop(void)
{
	return false;
}

bool wifi_softap_set_dhcps_lease(struct dhcps_lease * please)
{
	return false;
}

bool wifi_softap_get_dhcps_lease(struct dhcps_lease * please)
{
	return false;
}

uint32 wifi_softap_get_dhcps_lease_time(void)
{
	return 0;
}

bool wifi_softap_set_dhcps_lease_time(uint32 minute)
{
	return false;
}

bool wifi_softap_reset_dhcps_lease_time(void)
{
	return false;
}

enum dhcp_status wifi_softap_dhcps_status(void)
{
	return DHCP_STOPPED;
}

bool wifi_softap_set_dhcps_offer_option(uint8 level, void * optarg)
{
	return false;
}

bool wifi_get_ip_info(uint8 if_index, struct ip_info * info)
{
	return false;
}

bool wifi_set_ip_info(uint8 if_index, struct ip_info * info)
{
	return false;
}

bool wifi_get_macaddr(uint8 if_index, uint8 * macaddr)
{
	return false;
}

bool wifi_set_macaddr(uint8 if_index, uint8 * macaddr)
{
	return false;
}

uint8 wifi_get_channel(void)
{
	return 0;
}

bool wifi_set_channel(uint8 channel)
{
	return false;
}

void wifi_status_led_install(uint8 gpio_id, uint32 gpio_name, uint8 gpio_func)
{
}

void wifi_status_led_uninstall()
{
}

void wifi_promiscuous_enable(uint8 promiscuous)
{
}

void wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb)
{
}

void wifi_promiscuous_set_mac(const uint8_t * address)
{
}

enum phy_mode wifi_get_phy_mode(void)
{
	return PHY_MODE_11B;
}

bool wifi_set_phy_mode(enum phy_mode mode)
{
	return false;
}

bool wifi_set_sleep_type(enum sleep_type type)
{
	return false;
}

enum sleep_type wifi_get_sleep_type(void)
{
	return NONE_SLEEP_T;
}

void wifi_fpm_open(void)
{
}

void wifi_fpm_close(void)
{
}

void wifi_fpm_do_wakeup(void)
{
}

sint8 wifi_fpm_do_sleep(uint32 sleep_time_in_us)
{
	return 0;
}

void wifi_fpm_set_sleep_type(enum sleep_type type)
{
}

enum sleep_type wifi_fpm_get_sleep_type(void)
{
	return NONE_SLEEP_T;
}

void wifi_set_event_handler_cb(wifi_event_handler_cb_t cb)
{
    g_wifi_event_handler = cb;
}

/* Event loop WIFI Station connect */
emu_task_next_t libesp8266emu_enter_event_loop_wifi_station_connect(emu_task_t *task)
{
    (void)task;

    return EMU_TASK_NEXT_DESTROY;
}

/* Event loop WIFI Station disconnect */
emu_task_next_t libesp8266emu_enter_event_loop_wifi_station_disconnect(emu_task_t *task)
{
    (void)task;

    return EMU_TASK_NEXT_DESTROY;
}

