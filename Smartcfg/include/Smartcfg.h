#pragma once

/****************************************************
 * Use Smartconfig class:
 * Must be used in conjuntion with Credentials class. 
 * 1. call init(): status = smartconfig.init();
 * 2. enter ssid & password in Esptouch App
 * 3. Smartconfig will write ssid & password received
 *    in CREDENTIALS::Credentials credential.
 * 4. See Credentials info to know how to get credentials.
*****************************************************/

#include "esp_smartconfig.h"
#include "smartconfig_ack.h"
#include "esp_err.h"


namespace SMARTCFG
{
    enum class smartconfig_state_e
    {
        NOT_STARTED,
        STARTED,
        GOT_SSID_PASSWORD
    }; ///< SmartConfig states

    struct Credentials{
        char ssid[32];
        char password[64];
    };

    class Smartcfg
    {
    private:
        smartconfig_start_config_t   smartconfig_config{true, false, nullptr};

        static void _sc_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);

        Credentials my_credentials;
        

    public:

        //***********************************************
        /// @brief init smartconfig
        /// @return ESP_OK on success
        [[nodiscard]] esp_err_t init();


        //***********************************************
        /// @brief get smartconfig credentials
        /// @return smartconfig credentials
        Credentials& get_credentials(void)
        {
            return my_credentials;
        }

    };  // class Smartcfg



} // namespace SMARTCFG

