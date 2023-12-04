#include <string.h>
#include <algorithm>
#include "esp_log.h"
#include "esp_event.h"
#include "Smartcfg.h"



namespace SMARTCFG
{
    /*****************************************************
     * o compilador nao aceita que _smartconfig_state 
     * seja um membro da classe
    ******************************************************/
    smartconfig_state_e          _smartconfig_state{smartconfig_state_e::NOT_STARTED};

    constexpr static const char* _log_tag{"SMARTCONFIG"};




//***********************************************_sc_event_handler

void Smartcfg::_sc_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    Credentials* mycred = (Credentials*)arg;
    if (SC_EVENT == event_base)
    {
        const smartconfig_event_t event_type{static_cast<smartconfig_event_t>(event_id)};

        ESP_LOGI(_log_tag, "%s:%d Event ID %ld", __func__, __LINE__, event_id);

        switch(event_type)
        {
        case SC_EVENT_GOT_SSID_PSWD:
        {
            ESP_LOGI(_log_tag, "%s:%d Got Smartconfig SC_EVENT_GOT_SSID_PSWD", __func__, __LINE__);

            ////////////////////////////////////////////////////////
            smartconfig_event_got_ssid_pswd_t* const
                data{static_cast<smartconfig_event_got_ssid_pswd_t*>(event_data)};
            ////////////////////////////////////////////////////////

            ESP_LOGI(_log_tag, "%s:%d Calling sc_send_ack_start", __func__, __LINE__);

            ////////////////////////////////////////////////////////
            const esp_err_t sc_ack_status = sc_send_ack_start(data->type, data->token, data->cellphone_ip);
            ////////////////////////////////////////////////////////

            ESP_LOGI(_log_tag, "%s:%d sc_send_ack_start:%s", __func__, __LINE__,esp_err_to_name(sc_ack_status));

            ////////////////////////////////////////////////////////
            if (sc_ack_status != ESP_OK) 
            {
                ESP_LOGE(_log_tag, "Send smartconfig ACK error: %s", esp_err_to_name(sc_ack_status));
                break;
            }
            ////////////////////////////////////////////////////////

            
            size_t ssid_size = sizeof(smartconfig_event_got_ssid_pswd_t::ssid);
            size_t password_size = sizeof(smartconfig_event_got_ssid_pswd_t::password);

            // save internal credentials
            memset(mycred,0,sizeof(*mycred));
            memcpy(mycred->ssid,data->ssid,ssid_size);
            memcpy(mycred->password,data->password,password_size);
            
            // save crednetials in credentials instance
            // CREDENTIALS::Wifi_credentials cred;
            // memset(&cred,0,sizeof(cred));
            // memcpy(cred.ssid,data->ssid,ssid_size);
            // memcpy(cred.password,data->password,password_size);

            // CREDENTIALS::Credentials& cred_obj = CREDENTIALS::Credentials::get_instance();

            /***********************************************
             * Copy recvd credentials to Credentials object.
             * cred_obj is singleton instance.
            ************************************************/
            //cred_obj.set_credentials(cred);

            _smartconfig_state = smartconfig_state_e::GOT_SSID_PASSWORD;

        break;
        }
        case SC_EVENT_SCAN_DONE:
        {
            ESP_LOGI(_log_tag, "Scan done");

            break;
        }
        case SC_EVENT_FOUND_CHANNEL:
        {
            ESP_LOGI(_log_tag, "Found channel");
            break;
        }
        case SC_EVENT_SEND_ACK_DONE:
        {
            ESP_LOGI(_log_tag, "Send ACK done");
            break;
        }
        default: 
            break;
        }
    }
}



//***********************************************init

esp_err_t Smartcfg::init()
{
    esp_err_t status{ESP_OK};

    ESP_LOGI(_log_tag, "%s:%d Calling esp_event_handler_instance_register", __func__, __LINE__);

    ////////////////////////////////////////////////////////
    status = esp_event_handler_instance_register
                                        (
                                        SC_EVENT,
                                        ESP_EVENT_ANY_ID,&
                                        _sc_event_handler,
                                        &my_credentials,
                                        nullptr
                                        );
    ////////////////////////////////////////////////////////

    ESP_LOGI(_log_tag, "%s:%d esp_event_handler_instance_register:%s", __func__, __LINE__,esp_err_to_name(status));

    if (ESP_OK == status)
    {

    ESP_LOGI(_log_tag, "%s:%d Calling esp_smartconfig_set_type", __func__, __LINE__);

    ////////////////////////////////////////////////////////
    status = esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);
    ////////////////////////////////////////////////////////

    ESP_LOGI(_log_tag, "%s:%d esp_smartconfig_set_type:%s", __func__, __LINE__,esp_err_to_name(status));
    }

    if (ESP_OK == status)
    {
        ESP_LOGI(_log_tag, "%s:%d Calling esp_smartconfig_start", __func__, __LINE__);

        ////////////////////////////////////////////////////////
        status = esp_smartconfig_start(&smartconfig_config);
        ////////////////////////////////////////////////////////

        ESP_LOGI(_log_tag, "%s:%d esp_smartconfig_start:%s", __func__, __LINE__,esp_err_to_name(status));
    }

    if (ESP_OK == status)
    {
        SMARTCFG::_smartconfig_state = SMARTCFG::smartconfig_state_e::STARTED;
    }

    if (ESP_OK == status)
    {
        while (SMARTCFG::smartconfig_state_e::GOT_SSID_PASSWORD != SMARTCFG::_smartconfig_state)
        {
            ////////////////////////////////////////////////////////
            ESP_LOGI(_log_tag, "%s:%d Waiting for espTouch ssid & password:", __func__, __LINE__);
            ////////////////////////////////////////////////////////

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    return ESP_OK;
}   

}
