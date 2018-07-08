#include <c_types.h>

typedef void(*scan_done_cb_t)(void *arg, STATUS status);
struct scan_config {
	uint8 *ssid;	// Note: ssid == NULL, don't filter ssid.
	uint8 *bssid;	// Note: bssid == NULL, don't filter bssid.
	uint8 channel;	// Note: channel == 0, scan all channels, otherwise scan set channel.
	uint8 show_hidden;	// Note: show_hidden == 1, can get hidden ssid routers' info.
};

bool wifi_station_scan(struct scan_config *config, scan_done_cb_t cb);


