#include "main.h"


#define LOG_TAG "MAIN"

static Main my_main;


extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(my_main.setup());

    while (true)
    {
        my_main.loop();
    }
}

esp_err_t Main::setup(void)
{
    esp_err_t status {ESP_OK};

    ESP_LOGI(LOG_TAG, "Setup!");
    

    /*******************************************************
     * TEST NVS Storage to save/read nvs_credentials in flash:
     * 1. erase flash: idf.py erase-flash
     * 2. So, get_credentials() returns NOT_FOUND.
     * 3. Smart config is called and set_credentials();
     * 4 Now, nvs_credentials are in flash.
     * 5. Reboot ESP32 and credentias are read from flash.
     * 6. Wifi connects with the saved nvs_credentials.
     * 7. Sntp updates time very 3 seconds.
    *******************************************************/

    if(ESP_OK == status)
    {status = wifi.init();}

    if(ESP_OK == status)
    {status = wifi.wifi_sta_connect();}


    if(ESP_OK == status)
    {check_credentials();}


    if(ESP_OK == status)
    {   
        ESP_LOGI(LOG_TAG, "Get nvs credentials.");
        CREDENTIALS::Wifi_credentials nvs_cred{0};

        nvs_cred = nvs_credentials.get_credentials();

        ESP_LOGI(LOG_TAG, "CRED SSID: %s",(char*)nvs_cred.ssid);
        ESP_LOGI(LOG_TAG, "CRED PASS: %s",((char*)nvs_cred.password));

        ESP_LOGI(LOG_TAG, "Call wifi begin().");
        status = wifi.begin((char*)nvs_cred.ssid,(char*)nvs_cred.password);
    }

    if(ESP_OK == status)
    {
        ESP_LOGI(LOG_TAG, "Time before SNTP init:");
        sntp.print_time(sntp.get_time_now());

        sntp.init(3*one_minute);
    }
    
    return status;
}

void Main::loop(void)
{
    ESP_LOGI(LOG_TAG, "Hello World!");
    
    sntp.print_time_from_last_update();

    vTaskDelay(pdSECOND);

}

esp_err_t Main::check_credentials(void)
{
    CREDENTIALS::Wifi_credentials nvs_cred{0};
    SMARTCFG::Credentials smart_cred{0};
    esp_err_t status{ESP_OK};

    ESP_LOGI(LOG_TAG, "Checking Credentials.");
    if (nvs_credentials.empty())
    {
        ESP_LOGI(LOG_TAG, "Credentials is empty!");
        ESP_LOGI(LOG_TAG, "Calling smartconfig!");

        status = smartconfig.init();

        if (ESP_OK == status)
        {
            ESP_LOGI(LOG_TAG, "Geting smartconfig credentials.");
            smart_cred = smartconfig.get_credentials();

            //copy smart_credentials to nvs_credentials in nvs
            size_t ssid_size = sizeof(smartconfig_event_got_ssid_pswd_t::ssid);
            size_t password_size = sizeof(smartconfig_event_got_ssid_pswd_t::password);
            memset(&nvs_cred,0,sizeof(nvs_cred));
            memcpy(nvs_cred.ssid,smart_cred.ssid,ssid_size);
            memcpy(nvs_cred.password,smart_cred.password,password_size);

            //save nvs_credentials in nvs
            CREDENTIALS::Credentials& cred_obj = CREDENTIALS::Credentials::get_instance();
            ESP_LOGI(LOG_TAG, "Saving smartconfig credentials in nvs credentials");
            cred_obj.set_credentials(nvs_cred);

        }
    }
    return status;
}
