
idf_component_register(SRCS 
                    Wifi.cpp

                    INCLUDE_DIRS
                         include

                    REQUIRES 
                        esp_wifi nvs_flash esp_event esp_netif lwip)