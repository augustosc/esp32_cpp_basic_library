#pragma once

/***********************************************************
 * Use Wifi class:
 * 1. create wifi object: WIFI::Wifi wifi;
 * 2. STATION:
 *   a. call wifi.init();
 *   b. call wifi_sta_connect(); 
 *   c. call wifi.begin("ssid","password");
 * 2. AP:
 *   a. call wifi.init();
 *   b. call wifi_ap_connect():
 * 3. AP: TO STA:
 * 
***********************************************************/

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/event_groups.h"
#include <cstring>

namespace WIFI
{

class Wifi
{
    
private:
    // private member variables
    constexpr static const char* _log_tag{"WIFI"};
    static const EventBits_t READY_TO_CONNECT_BIT {BIT0};
    static const EventBits_t WIFI_FAIL_BIT {BIT1};
    static const EventBits_t CONNECTED_BIT {BIT2};
    static const EventBits_t AP_CONNECTED_BIT {BIT3};
    static wifi_init_config_t wifi_init_config;
    static wifi_config_t wifi_config;
    static char mac_addr_cstr[13];  ///< Buffer to hold MAC as cstring
    

    // private member functions

    //***********************************************
    /// @brief get esp32 mac address
    /// @return ESP_OK on success
    static esp_err_t _get_mac(void);


    //***********************************************
    /// @brief get string of disconnection code 
    /// @param reason code for the disconnection
    /// @return string of disconnection code
    static char * _get_disconnection_error(uint8_t reason);



    //***********************************************
    /// @brief wifi event handler
    static void _wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);



    //***********************************************
    /// @brief ip event handler
    static void _ip_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
    


public:
    // Strongly typed enum to define our state
    enum class state_e
    {
        NOT_INITIALISED,
        INITIALISED,
        READY_TO_CONNECT,
        CONNECTING,
        WAITING_FOR_IP,
        CONNECTED,
        DISCONNECTED,
        ERROR
    };

    

    //constructors
    // "Rule of 5" Constructors and assignment operators
    // Ref: https://en.cppreference.com/w/cpp/language/rule_of_three
    Wifi(void);
    ~Wifi(void)                     = default;
    Wifi(const Wifi&)               = default;
    Wifi(Wifi&&)                    = default;
    Wifi& operator=(const Wifi&)    = default;
    Wifi& operator=(Wifi&&)         = default;

    // public member variables
    static state_e _wifi_state;
    
    // public static member functions
    static void set_wificonfig(wifi_config_t& cfg){
        wifi_config = cfg;
    }
    
    static wifi_config_t get_wificonfig(){
        return wifi_config;
    }
    

    // public member functions

    //***********************************************
    /// @brief init wifi
    /// @return ESP_OK on success
    esp_err_t init(void);


    //***********************************************
    /// @brief configure and start wifi
    /// @param SSID ssid
    /// @param PASS password
    /// @return ESP_OK on success
    esp_err_t begin(const char* SSID, const char* PASS);


    //***********************************************
    /// @brief connect esp32 as station
    /// @return ESP_OK on success
    esp_err_t wifi_sta_connect();


    //***********************************************
    /// @brief connect esp32 as AP
    /// @param ssid ssid
    /// @param pass password
    /// @param cfg config struct (NULL for default)
    /// @return ESP_OK on success
    esp_err_t wifi_ap_connect(const char *ssid, const char *pass, wifi_config_t* cfg = NULL);


    //***********************************************
    /// @brief disconnect station
    /// @return ESP_OK on success
    esp_err_t wifi_disconnect();

    //***********************************************
    /// @brief stop wifi
    /// @return ESP_OK on success
    esp_err_t wifi_stop();

    //***********************************************
    /// @brief destroy netif object
    /// @param netif_object object
    void netif_destroy(esp_netif_t* netif_object);


    esp_err_t wifi_deinit();
    esp_err_t netif_deinit();

    //***********************************************
    /// @brief get wifi state
    /// @return wifi state
    constexpr const state_e& get_state(void) { return _wifi_state; }

    //***********************************************
    /// @brief get mac address string
    /// @return mac address string
    constexpr static const char* get_mac(void) 
        { return mac_addr_cstr; }



    
};


} // namespace WIFI