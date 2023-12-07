# CPP Sntp driver
This folder contains an ESP-IDF driver for Sntp written in C++.

## Member Functions
- init(sync_interval) - init sntp

- get_time_now(void) - get current time

- print_time(t) - print time

- get_time_from_last_update(void) - get duration from last sntp update

- get_instance(void) - get sntp singleton instance


## Using the component
- Creating an instance in main.h inside class Main
```bash
SNTP::Sntp& sntp { SNTP::Sntp::get_instance() };
``````

- Creating wifi instance in main.h inside class Main
```bash
WIFI::Wifi wifi;
``````

- Init the instance in main.cpp inside main::setup
```bash
sntp.init(3*one_minute);
```
- Init wifi instance in main.cpp inside main::setup
```bash
status = wifi.init();
status = wifi.wifi_sta_connect();
status = wifi.begin("ssid","password");
``````
- Use Sntp member functions in your code
```bash
time_t get_time_now(void);
void print_time(t);
time_t get_time_from_last_update(void);
...
```

## Example
Download an example code from Sntp/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally, first download all MySpiffs component from github, so choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```







