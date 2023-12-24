# CPP Smartconfig driver
This folder contains an ESP-IDF driver for Smartconfig written in C++.

## Member Functions
- init(void) - init smartconfig
- get_credentials(void) - get smartconfig credentials

## Using the component
- Creating an instance in main.h inside class Main
```bash
SMARTCFG::Smartcfg smartconfig;
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

- Get credentials from smartconfig
```bash
status = smartconfig.init();
``````

- Init the instance in main.cpp inside main::setup
```bash
sntp.init(3*one_minute);
```

- After Smartconfig get credentials from ESPTOUCH APP, set wifi credentials:
```bash
SMARTCFG::Credentials credential{0};

credential = smartconfig.get_credentials();

wifi.begin((char*)credential.ssid,(char*)credential.password);
```


## Example
Download an example code from Smartconfig/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally, first download all Smartcfg component from github, so choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```


