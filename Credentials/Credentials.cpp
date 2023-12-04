#include "Credentials.h"
#include "Nvs32.h"


namespace CREDENTIALS
{
    bool Credentials::empty(void)
    {
        esp_err_t status = read_credentials(credentials);

        if (ESP_OK == status)
        {
            if ('\0' == credentials.ssid[0] ||
            '\0' == credentials.password[0])
            return true;
        return false;
        
        }
        else
        {
            return true;
        }
    } // empty_credentials



    void Credentials::set_credentials (Wifi_credentials &cred)
    {
        credentials = cred;

        write_credentials(cred);
    } // set_credentials


    Wifi_credentials Credentials::get_credentials()
    {
        return credentials;
    } // get_credentials

    
    esp_err_t Credentials::write_credentials(Wifi_credentials& cred)
    {
        esp_err_t status = ESP_OK;

        NVS::Nvs& nvs {NVS::Nvs::get_instance()};

        status = nvs.init(nvs_namespace);

        if (ESP_OK==status)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling write_credentials", __func__, __LINE__);

            esp_err_t status = nvs.write(key,&cred,1);

            ESP_LOGI(_log_tag, "%s:%d write_credentials:%s", __func__, __LINE__,esp_err_to_name(status));
        }
        
        nvs.close();

        return status;
    } // write_credentials



    esp_err_t Credentials::read_credentials (Wifi_credentials& cred)
    {
        size_t n_itens = 1;
        esp_err_t status = ESP_OK;

        NVS::Nvs& nvs {NVS::Nvs::get_instance()};

        status = nvs.init(nvs_namespace);

        if(ESP_OK == status)

        {
            ESP_LOGI(_log_tag, "%s:%d Calling read_credentials", __func__, __LINE__);

            status = nvs.read(key,&cred,n_itens);

            ESP_LOGI(_log_tag, "%s:%d read_credentials:%s", __func__, __LINE__,esp_err_to_name(status));
        }

        nvs.close();

        return status;
    } // read_credentials




}