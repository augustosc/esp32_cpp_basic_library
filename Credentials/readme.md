# CPP Credentials library
This folder contains an ESP-IDF library to handling credentials in NVS written in C++.

## Member Functions
-  Credentials& get_instance(void) - get singleton instance

- empty(void) - check empty credentials

- set_credentials (&cred) - write credentials in memory & nvs

- get_credentials(void) - get credentials from memory

- write_credentials(Wifi_credentials& cred) - write credentials on nvs

- read_credentials (Wifi_credentials& cred) - read credentials from nvs

## Using the component
- Creating an instance in main.h inside class Main
```bash
CREDENTIALS::Credentials& nvs_credentials { CREDENTIALS::Credentials::get_instance() };
``````

- Creating wifi instance in main.h inside class Main
```bash
WIFI::Wifi wifi;
``````


- Init wifi instance in main.cpp inside main::setup
```bash
status = wifi.init();
status = wifi.wifi_sta_connect();
``````

- Check for credentials
```bash
if (nvs_credentials.empty()){
    //
}
```
- If no credentials on nvs, get credentials from smartconfig or others
```bash
status = smartconfig.init();
```
- After Smartconfig get credentials from ESPTOUCH APP, copy to NVS credentials
```bash
smart_cred = smartconfig.get_credentials();
memcpy(nvs_cred.ssid,smart_cred.ssid,ssid_size);
memcpy(nvs_cred.password,smart_cred.password,password_size);
CREDENTIALS::Credentials& cred_obj = CREDENTIALS::Credentials::get_instance();
cred_obj.set_credentials(nvs_cred);
```

- Begin Wifi with the nvs credentials
```bash
CREDENTIALS::Wifi_credentials nvs_cred{0};
nvs_cred = nvs_credentials.get_credentials();
status = wifi.begin((char*)nvs_cred.ssid,(char*)nvs_cred.password);
```

## Example
Download an example code from Credentials/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally, first download all Credentials component from github, so choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```




