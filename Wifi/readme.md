# CPP Wifi driver
This folder contains an ESP-IDF driver for Wigi written in C++.

## Member Functions
- init(void) - init wifi

- begin(SSID, PASS) - configure and start wifi

- wifi_sta_connect(void) - connect esp32 as station

- wifi_ap_connect(ssid, pass, cfg) - connect esp32 as AP

- wifi_disconnect(void) - disconnect station

- wifi_stop(void) - stop wifi

- netif_destroy(esp_netif_t* netif_object) - destroy netif object

- esp_err_t wifi_deinit() - deinit wifi

- state_e& get_state(void) - get wifi state

- get_mac(void) - get mac address string

- set_wificonfig(wifi_config_t& cfg) - set wifi config

- get_wificonfig(wifi_config_t& cfg) - get wifi config



## Using the component
- Creating an instance in main.h inside class Main
```bash
WIFI::Wifi wifi;
``````

- Init the instance in main.cpp inside main::setup
```bash
esp_err_t status = wifi.init();
``````
- For Wifi Station:
```bash
status = wifi.wifi_sta_connect();
status = wifi.begin("ssid","password");
```

- For Wifi AP:
```bash
status = wifi.wifi_ap_connect("Esp32","password")
```

## Example
Download an example code from Wifi/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally, first download all MySpiffs component from github, so choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```
















