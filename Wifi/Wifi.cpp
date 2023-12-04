#include "Wifi.h"
#include "esp_err.h"
#include <stdio.h>




namespace WIFI
{

EventGroupHandle_t wifi_event_group;
static  esp_netif_t* my_netif{NULL};

// Wifi statics
char                    Wifi::mac_addr_cstr[]{};
Wifi::state_e           Wifi::_wifi_state{state_e::NOT_INITIALISED};
wifi_init_config_t      Wifi::wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
wifi_config_t           Wifi::wifi_config{};


//""""""""""""""""""""""""""""""""Constructor definition
Wifi::Wifi(void)
{

    // Check if the MAC cstring currently begins with a
    //   nullptr, i.e. is default initialised, not set
    if (!get_mac()[0])
    {
        // Get the MAC and if this fails restart
        if (ESP_OK != _get_mac())
            esp_restart();
    }
}


//""""""""""""""""""""""""""""""""_get_mac
// Get the MAC from the API and convert to ASCII HEX

esp_err_t Wifi::_get_mac(void)
{
    uint8_t mac_byte_buffer[6]{};   ///< Buffer to hold MAC as bytes

    // Get the MAC as bytes from the ESP API
    const esp_err_t 
        status{esp_efuse_mac_get_default(mac_byte_buffer)};

    if (ESP_OK == status)
    {
        // Convert the bytes to a cstring and store
        //   in our static buffer as ASCII HEX
        snprintf(mac_addr_cstr, sizeof(mac_addr_cstr), 
                    "%02X%02X%02X%02X%02X%02X",
                    mac_byte_buffer[0],
                    mac_byte_buffer[1],
                    mac_byte_buffer[2],
                    mac_byte_buffer[3],
                    mac_byte_buffer[4],
                    mac_byte_buffer[5]);
                    
        printf("Esp32 mac_address: %s\n",mac_addr_cstr);
    }

    return status;
}


//""""""""""""""""""""""""""""""""_get_disconnection_error

char * Wifi::_get_disconnection_error(uint8_t reason)
{
    switch (reason)
    {
        case WIFI_REASON_UNSPECIFIED:
            return (char*) "WIFI_REASON_UNSPECIFIED";
        case WIFI_REASON_AUTH_EXPIRE:
            return (char*) "WIFI_REASON_AUTH_EXPIRE";
        case WIFI_REASON_AUTH_LEAVE:
            return (char*) "WIFI_REASON_AUTH_LEAVE";
        case WIFI_REASON_ASSOC_EXPIRE:
            return (char*) "WIFI_REASON_ASSOC_EXPIRE";
        case WIFI_REASON_ASSOC_TOOMANY:
            return (char*) "WIFI_REASON_ASSOC_TOOMANY";
        case WIFI_REASON_NOT_AUTHED:
            return (char*) "WIFI_REASON_NOT_AUTHED";
        case WIFI_REASON_NOT_ASSOCED:
            return (char*) "WIFI_REASON_NOT_ASSOCED";
        case WIFI_REASON_ASSOC_LEAVE:
            return (char*) "WIFI_REASON_ASSOC_LEAVE";
        case WIFI_REASON_ASSOC_NOT_AUTHED:
            return (char*) "WIFI_REASON_ASSOC_NOT_AUTHED";
        case WIFI_REASON_DISASSOC_PWRCAP_BAD:
            return (char*) "WIFI_REASON_DISASSOC_PWRCAP_BAD";
        case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
            return (char*) "WIFI_REASON_DISASSOC_SUPCHAN_BAD";
        case WIFI_REASON_BSS_TRANSITION_DISASSOC:
            return (char*) "WIFI_REASON_BSS_TRANSITION_DISASSOC";
        case WIFI_REASON_IE_INVALID:
            return (char*) "WIFI_REASON_IE_INVALID";
        case WIFI_REASON_MIC_FAILURE:
            return (char*) "WIFI_REASON_MIC_FAILURE";
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            return (char*) "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT";
        case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
            return (char*) "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT";
        case WIFI_REASON_IE_IN_4WAY_DIFFERS:
            return (char*) "WIFI_REASON_IE_IN_4WAY_DIFFERS";
        case WIFI_REASON_GROUP_CIPHER_INVALID:
            return (char*) "WIFI_REASON_GROUP_CIPHER_INVALID";
        case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
            return (char*) "WIFI_REASON_PAIRWISE_CIPHER_INVALID";
        case WIFI_REASON_AKMP_INVALID:
            return (char*) "WIFI_REASON_AKMP_INVALID";
        case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
            return (char*) "WIFI_REASON_UNSUPP_RSN_IE_VERSION";
        case WIFI_REASON_INVALID_RSN_IE_CAP:
            return (char*) "WIFI_REASON_INVALID_RSN_IE_CAP";
        case WIFI_REASON_802_1X_AUTH_FAILED:
            return (char*) "WIFI_REASON_802_1X_AUTH_FAILED";
        case WIFI_REASON_CIPHER_SUITE_REJECTED:
            return (char*) "WIFI_REASON_CIPHER_SUITE_REJECTED";
        case WIFI_REASON_INVALID_PMKID:
            return (char*) "WIFI_REASON_INVALID_PMKID";
        case WIFI_REASON_BEACON_TIMEOUT:
            return (char*) "WIFI_REASON_BEACON_TIMEOUT";
        case WIFI_REASON_NO_AP_FOUND:
            return (char*) "WIFI_REASON_NO_AP_FOUND";
        case WIFI_REASON_AUTH_FAIL:
            return (char*) "WIFI_REASON_AUTH_FAIL";
        case WIFI_REASON_ASSOC_FAIL:
            return (char*) "WIFI_REASON_ASSOC_FAIL";
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            return (char*) "WIFI_REASON_HANDSHAKE_TIMEOUT";
        case WIFI_REASON_CONNECTION_FAIL:
            return (char*) "WIFI_REASON_CONNECTION_FAIL";
        case WIFI_REASON_AP_TSF_RESET:
            return (char*) "WIFI_REASON_AP_TSF_RESET";
        case WIFI_REASON_ROAMING:
            return (char*) "WIFI_REASON_ROAMING";
        
        default: return (char*) "UNKNOWN REASON";
    }
    return (char*) "";
}


//""""""""""""""""""""""""""""""""_wifi_event_handler

void Wifi::_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (WIFI_EVENT == event_base)
    {
        const wifi_event_t event_type{static_cast<wifi_event_t>(event_id)};

        switch(event_type)
        {
            case WIFI_EVENT_STA_START:
            {

                ESP_LOGI(_log_tag, "%s:%d WIFI_EVENT_STA_START", __func__, __LINE__);
                

                _wifi_state = state_e::READY_TO_CONNECT;

                xEventGroupSetBits(wifi_event_group, READY_TO_CONNECT_BIT);

            }
            break;

            case WIFI_EVENT_STA_CONNECTED:
            {
                ESP_LOGI(_log_tag, "%s:%d WIFI_EVENT_STA_CONNECTED", __func__, __LINE__);

                _wifi_state = state_e::WAITING_FOR_IP;
                
            }
            break;
            case WIFI_EVENT_STA_DISCONNECTED:
            {
                ESP_LOGI(_log_tag, "%s:%d WIFI_EVENT_STA_DISCONNECTED", __func__, __LINE__);

                xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
                _wifi_state = state_e::DISCONNECTED;
                wifi_event_sta_disconnected_t * disconnect_data = (wifi_event_sta_disconnected_t *)event_data;

                ESP_LOGI(_log_tag, "%s:%d WIFI_EVENT_STA_DISCONNECTED, code %d: %s \n",__func__, __LINE__, disconnect_data->reason,_get_disconnection_error(disconnect_data->reason));
                
                if (disconnect_data->reason != WIFI_REASON_ASSOC_LEAVE)
                {
                    ESP_LOGI(_log_tag, "retry to connect to the AP.");
                    esp_wifi_connect();
                }
            }
            break;
            
            case WIFI_EVENT_AP_STACONNECTED:
            {
                wifi_event_ap_staconnected_t *connected_event = (wifi_event_ap_staconnected_t *)event_data;
                ESP_LOGI(_log_tag, "station " MACSTR " joined, AID=%d", MAC2STR(connected_event->mac), connected_event->aid);
            }
            break;
            case WIFI_EVENT_AP_STADISCONNECTED:
            {
                wifi_event_ap_stadisconnected_t *disconnected_event = (wifi_event_ap_stadisconnected_t *)event_data;
                ESP_LOGI(_log_tag, "station " MACSTR " leave, AID=%d", MAC2STR(disconnected_event->mac), disconnected_event->aid);
            }
            break;
            case WIFI_EVENT_AP_START:
                ESP_LOGI(_log_tag, "%s:%d AP Starting...\n", __func__, __LINE__);
                xEventGroupSetBits(wifi_event_group, AP_CONNECTED_BIT);
                break;

            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(_log_tag, "%s:%d AP Stopping...\n", __func__, __LINE__);
                xEventGroupClearBits(wifi_event_group, AP_CONNECTED_BIT);
                break;

            default:
                ESP_LOGW(_log_tag, "%s:%d OTHER WIFI_EVENT (%ld)", __func__, __LINE__, event_id);
                break;
        }
    }
}


//""""""""""""""""""""""""""""""""_ip_event_handler

void Wifi::_ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (IP_EVENT == event_base)
    {
        const ip_event_t event_type{static_cast<ip_event_t>(event_id)};

        

        switch(event_type)
        {
        case IP_EVENT_STA_GOT_IP:
        {
            ESP_LOGI(_log_tag, "%s:%d IP_EVENT_STA_GOT_IP %ld", __func__, __LINE__, event_id);

            _wifi_state = state_e::CONNECTED;
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

            ESP_LOGI(_log_tag, "%s:%d WIFI CONNECTED!", __func__, __LINE__);

            break;
        }

        case IP_EVENT_STA_LOST_IP:
        {
            ESP_LOGI(_log_tag, "%s:%d IP_EVENT_STA_LOST_IP %ld", __func__, __LINE__, event_id);

            _wifi_state = state_e::WAITING_FOR_IP;
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

            ESP_LOGI(_log_tag, "%s:%d WAITING_FOR_IP AGAIN", __func__, __LINE__);

            break;
        }

        default:
            // TODO IP6
            ESP_LOGW(_log_tag, "%s:%d OTHER IP_EVENT (%ld)", __func__, __LINE__, event_id);
            break;
        }
    }
}


//""""""""""""""""""""""""""""""""init

esp_err_t Wifi::init(void)
{
    wifi_event_group = xEventGroupCreate();

    ESP_LOGI(_log_tag, "%s:%d Init Wifi object",__func__, __LINE__);

    esp_err_t status{ESP_OK};

    if (state_e::NOT_INITIALISED == _wifi_state)
    {
        ESP_LOGI(_log_tag, "%s:%d Calling esp_netif_init", __func__, __LINE__);

        ///////////////////////////
        status = esp_netif_init();
        ///////////////////////////

        ESP_LOGI(_log_tag, "%s:%d esp_netif_init:%s", __func__, __LINE__, esp_err_to_name(status));


        if (ESP_OK == status)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_init", __func__, __LINE__);
            ///////////////////////////
            status = esp_wifi_init(&wifi_init_config);
            ///////////////////////////

            ESP_LOGI(_log_tag, "%s:%d esp_wifi_init:%s", __func__, __LINE__,esp_err_to_name(status));
        }

        if (ESP_OK == status)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling WIFI_event_handler_instance_register", __func__, __LINE__);
            status = esp_event_handler_instance_register
                                           (
                                            WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &_wifi_event_handler,
                                            nullptr,
                                            nullptr
                                           );
            ESP_LOGI(_log_tag, "%s:%d WIFI_event_handler_instance_register:%s", __func__, __LINE__,esp_err_to_name(status));
        }

        if (ESP_OK == status)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling IP_event_handler_instance_register", __func__, __LINE__);
            status = esp_event_handler_instance_register
                                           (
                                            IP_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &_ip_event_handler,
                                            nullptr,
                                            nullptr
                                           );
            ESP_LOGI(_log_tag, "%s:%d IP_event_handler_instance_register:%s", __func__, __LINE__,esp_err_to_name(status));

        }

        if (ESP_OK == status)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_storage", __func__, __LINE__);

            /////////////////////////////////
            status = esp_wifi_set_storage(WIFI_STORAGE_RAM);
            /////////////////////////////////

            ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_storage:%s", __func__, __LINE__,esp_err_to_name(status));
        }

        ESP_LOGI(_log_tag, "%s:%d INITIALISED", __func__, __LINE__);
            _wifi_state = state_e::INITIALISED;
    }
    else if (state_e::ERROR == _wifi_state)
    {
        ESP_LOGE(_log_tag, "%s:%d FAILED", __func__, __LINE__);
        status = ESP_FAIL;
    }

    return status;
}


//""""""""""""""""""""""""""""""""begin

esp_err_t Wifi::begin(const char* SSID, const char* PASS)
{
    esp_err_t status{ESP_OK};
    
    ESP_LOGW(_log_tag, "ssid = %s\tpassword = %s",SSID,PASS);

    ESP_LOGI(_log_tag, "%s:%d _state = %d", __func__, __LINE__, (int)_wifi_state);

    if (_wifi_state == state_e::READY_TO_CONNECT)
    {
        memset(&wifi_config.sta.ssid,0,sizeof(wifi_config.sta.ssid));
        memset(&wifi_config.sta.password,0,sizeof(wifi_config.sta.password));
        memcpy(wifi_config.sta.ssid, SSID,sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, PASS,sizeof(wifi_config.sta.password));

        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_config", __func__, __LINE__);

        /////////////////////////////////////////
        status = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        /////////////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_config:%s", __func__, __LINE__,esp_err_to_name(status));


        if(ESP_OK == status)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_connect", __func__, __LINE__);

            /////////////////////////////////////////
            status = esp_wifi_connect();
            /////////////////////////////////////////

            ESP_LOGI(_log_tag, "%s:%d esp_wifi_connect:%s", __func__, __LINE__,esp_err_to_name(status));
        }
        
        if (ESP_OK == status)
            _wifi_state = state_e::CONNECTING;
    }

    /***************************************
     * Wait for CONNECTED_BIT before return.
    ****************************************/
    ESP_LOGI(_log_tag, "%s:%d Waiting for CONNECTED_BIT", __func__, __LINE__);

    EventBits_t result = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
    
    status = (result & CONNECTED_BIT)? ESP_OK : ESP_FAIL;

    ESP_LOGI(_log_tag, "%s:%d Waiting for CONNECTED_BIT:%s", __func__, __LINE__,esp_err_to_name(status));

    return status;
}


//""""""""""""""""""""""""""""""""wifi_sta_connect

esp_err_t Wifi::wifi_sta_connect()
{
    esp_err_t status{ESP_OK};

    ESP_LOGI(_log_tag, "%s:%d Init wifi_sta_connect", __func__, __LINE__);

    if (ESP_OK == status)
        {

            /*******************************
             * Assure new esp_netif object
            *******************************/
            /////////////////////////////////////////
            if (my_netif)
            {
                esp_netif_destroy(my_netif);
                my_netif=NULL;
            }

            ESP_LOGI(_log_tag, "%s:%d Calling esp_netif_create_default_wifi_sta", __func__, __LINE__);

            /////////////////////////////////////////
            my_netif = esp_netif_create_default_wifi_sta();
            /////////////////////////////////////////

            ESP_LOGI(_log_tag, "%s:%d esp_netif_create_default_wifi_sta:%p", __func__, __LINE__,my_netif);

            if (!my_netif) status = ESP_FAIL;
        }

    // for static ip...
    // esp_netif_dhcpc_stop(esp_netif);
    // esp_netif_ip_info_t ip_info;
    // ip_info.ip.addr = ipaddr_addr("192.168.0.222");
    // ip_info.gw.addr = ipaddr_addr("192.168.0.1");
    // ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
    // esp_netif_set_ip_info(esp_netif, &ip_info);

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_mode", __func__, __LINE__);

        /////////////////////////////////
        status = esp_wifi_set_mode(WIFI_MODE_STA); 
        /////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_mode:%s", __func__, __LINE__,esp_err_to_name(status));
    }

    if (ESP_OK == status)
    {
        wifi_config.sta.threshold.authmode  = WIFI_AUTH_WPA2_PSK;
        wifi_config.sta.pmf_cfg.capable     = true;
        wifi_config.sta.pmf_cfg.required    = false;

        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_config", __func__, __LINE__);

        /////////////////////////////////
        status = esp_wifi_set_config(WIFI_IF_STA, &wifi_config); 
        /////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_config:%s", __func__, __LINE__,esp_err_to_name(status));
    }

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_start", __func__, __LINE__);

        /////////////////////////////////
        status = esp_wifi_start();
        /////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d esp_wifi_start:%s", __func__, __LINE__,esp_err_to_name(status));
    }

    /****************************************************
     * wait for READY_TO_CONNECT_BIT before call _begin()
    *****************************************************/
    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Waiting bit READY_TO_CONNECT_BIT", __func__, __LINE__);

        EventBits_t result = xEventGroupWaitBits(wifi_event_group, READY_TO_CONNECT_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(5000));

        /////////////////////////////////////
        status = (result & READY_TO_CONNECT_BIT)? ESP_OK : ESP_FAIL;
        /////////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d Waiting bit READY_TO_CONNECT_BIT:%s", __func__, __LINE__,esp_err_to_name(status));

    }
    return status;

}


esp_err_t Wifi::wifi_ap_connect(const char *ssid, const char *pass, wifi_config_t* cfg)
{
    esp_err_t status = ESP_OK;

    wifi_config_t wifi_config;
    if (cfg == NULL)
    {
        ESP_LOGI(_log_tag, "%s:%d setting wifi_config.ap", __func__, __LINE__);
        memset(&wifi_config, 0, sizeof(wifi_config_t));
        strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
        strncpy((char *)wifi_config.ap.password, pass, sizeof(wifi_config.sta.password) - 1);
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        wifi_config.ap.max_connection = 4;
        wifi_config.ap.beacon_interval = 100;
    } else {
        wifi_config = *cfg;
    }


    /*******************************
    * Assure new esp_netif object
    *******************************/

    if (my_netif)
    {
        netif_destroy(my_netif); 
        my_netif=NULL;
    }

    ESP_LOGI(_log_tag, "%s:%d Calling esp_netif_create_default_wifi_ap", __func__, __LINE__);

    /////////////////////////////////
    my_netif = esp_netif_create_default_wifi_ap();
    /////////////////////////////////

    ESP_LOGI(_log_tag, "%s:%d esp_netif_create_default_wifi_ap:%p", __func__, __LINE__,my_netif);

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_mode", __func__, __LINE__);
        
        /////////////////////////////////
        status = esp_wifi_set_mode(WIFI_MODE_AP);
        /////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_mode:%s", __func__, __LINE__,esp_err_to_name(status));
    }


    if(ESP_OK == status)
    {
        /*************************************************************
         * esp_wifi_set_config MUST BE CALLED after esp_wifi_set_mode
        **************************************************************/
        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_config", __func__, __LINE__);

        /////////////////////////////////
        status = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
        /////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_config:%s", __func__, __LINE__,esp_err_to_name(status));
    }

    

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_start", __func__, __LINE__);

        /////////////////////////////////
        status = esp_wifi_start();
        /////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_start:%s", __func__, __LINE__,esp_err_to_name(status));
    }

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Waiting bit AP_CONNECTED_BIT", __func__, __LINE__);

        EventBits_t result = xEventGroupWaitBits(wifi_event_group, AP_CONNECTED_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(5000));

        /////////////////////////////////////
        status = (result & AP_CONNECTED_BIT)? ESP_OK : ESP_FAIL;
        /////////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d Waiting bit AP_CONNECTED_BIT:%s", __func__, __LINE__,esp_err_to_name(status));
    } 
 
    return status;

}


//""""""""""""""""""""""""""""""""wifi_disconnect

// disconnect wifi station
esp_err_t Wifi::wifi_disconnect()
{
    ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_disconnect", __func__, __LINE__);

        esp_err_t status = esp_wifi_disconnect();

    ESP_LOGI(_log_tag, "%s:%d esp_wifi_disconnect:%s", __func__, __LINE__,esp_err_to_name(ESP_OK));

    return status;
}


//""""""""""""""""""""""""""""""""wifi_stop

// stop wifi station
esp_err_t Wifi::wifi_stop()
{
    return esp_wifi_stop();
}


void wifi_shutdown(void)
{
    esp_err_t err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        return;
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(esp_wifi_deinit());

    // call esp_wifi_clear_default_wifi_driver_and_handlers
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(my_netif));

    // esp_netif_destroy
    esp_netif_destroy(my_netif);
    my_netif = NULL;
}


//""""""""""""""""""""""""""""""""wifi_deinit

esp_err_t Wifi::wifi_deinit (void) {

    esp_err_t status = ESP_OK;

    ESP_LOGI(_log_tag, "%s:%d Unregister IP_EVENT", __func__, __LINE__);

    status = esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, (esp_event_handler_instance_t)_ip_event_handler);

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Unregister WIFI_EVENT", __func__, __LINE__);

        status = esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, (esp_event_handler_instance_t)_wifi_event_handler);
    }

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_deinit", __func__, __LINE__);

        status = esp_wifi_deinit();

        ESP_LOGI(_log_tag, "%s:%d esp_wifi_deinit:%s", __func__, __LINE__,esp_err_to_name(ESP_OK));
    }

    return status;
}



//""""""""""""""""""""""""""""""""netif_destroy

// destroy netif object
void Wifi::netif_destroy(esp_netif_t* netif_obj)
{
    esp_netif_destroy(netif_obj);
}




} // namespace WIFI