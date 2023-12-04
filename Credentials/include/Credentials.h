#pragma once

/**********************************************************
 * Use Credentials class:
 * 1. to get Credentials singleton instance:
 *   CREDENTIALS::Credentials& instance =  
 *            CREDENTIALS::Credentials::get_instance();
 * 
 * 2. to get credentials: 
 *   CREDENTIALS::Wifi_credentials cred = 
 *            credentials.get_credentials()
 * 
 * 3. to set credentials: instance.set_credentials(cred);
 * 4. Nvs is used to write/read credentiasl in flash.
 *   a. as calling set_credentials(), automatically wites
 *   credentials in memory(credentials) and in flash.
 *   b. as calling empty_credentials(), automatically
 *   credentials are read from flash and saved in memory.
 *   c. get_credentials() returns credentials in memory.
 * 
***********************************************************/

#include "esp_log.h"
#include "esp_err.h"
//#include "../../Nvs32/include/Nvs32.h"
//#include "Nvs32.h"


namespace CREDENTIALS {

    struct  Wifi_credentials
    {
        uint8_t ssid[32];
        uint8_t password[64];
    }; // struct  Wifi_credentials

    class Credentials {
    private:
        Wifi_credentials credentials{};
        const char* key{"credentials"};
        const char* nvs_namespace{"Esp32"};
        // NVS::Nvs& nvs {NVS::Nvs::get_instance()};
        const char* _log_tag{"CREDENTIALS"};


    public:

        //***********************************************
        /// @brief get singleton instance
        /// @return instance
        static Credentials& get_instance(void)
        {
            static Credentials instance;
            return instance;
        } // get_instance


        //***********************************************
        /// @brief check empty credentials
        /// @return true true if empty
        bool empty(void);
        

        //***********************************************
        /// @brief set credentials in memory & nvs
        /// @param cred struct with ssid & password
        void set_credentials (Wifi_credentials &cred);
        

        //***********************************************
        /// @brief get credentials from memory
        /// @return struct with ssid & password
        Wifi_credentials get_credentials();
        
        //***********************************************
        /// @brief write credentials on nvs
        /// @param cred struct with ssid & password
        /// @return ESP_OK on success
        esp_err_t write_credentials(Wifi_credentials& cred);
        

        //***********************************************
        /// @brief read credentials from nvs
        /// @param cred struct with ssid & password
        /// @return ESP_OK on success
        esp_err_t read_credentials (Wifi_credentials& cred);
        

    }; // class Credentials


} // namespace CREDENTIALS