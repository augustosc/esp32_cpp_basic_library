# Wifi class
To create singleton wifi object

## Member Functions
init(void) - init wifi

begin(const char* SSID, const char* PASS) - configure and start wifi

wifi_sta_connect(void) - connect esp32 as station

wifi_ap_connect(const char * ssid, const char * pass, wifi_config_t* cfg) - connect esp32 as AP

wifi_disconnect(void) - disconnect station

wifi_stop(void) - stop wifi

netif_destroy(esp_netif_t* netif_object) - destroy netif object

state_e& get_state(void) - get wifi state

get_mac(void) - get mac address string


















