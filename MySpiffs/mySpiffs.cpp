#include "mySpiffs.h"

#include <string.h>
#include <errno.h>

#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace SPIFFS {

    //***********************************************

    esp_err_t Spiffs::mount(const char* p_label,
                        const char* path,
                        size_t max,
                        bool fmt_on_fail)
    {
        esp_err_t status;

        esp_vfs_spiffs_conf_t conf = {
            .base_path = path,
            .partition_label = p_label,
            .max_files = max,
            .format_if_mount_failed = fmt_on_fail
        };
        _conf=conf;
        status = esp_vfs_spiffs_register(&conf);
        return status;
    }



    //***********************************************

    esp_err_t Spiffs::spiffs_check(const char* p_label)
    {
        esp_err_t ret = ESP_OK;

        ESP_LOGI(_log_tag, "esp_spiffs_check init");
        ret = esp_spiffs_check(p_label);
        ESP_LOGI(_log_tag, "esp_spiffs_check end");

        if (ret != ESP_OK) {
            ESP_LOGE(_log_tag, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        } else {
            ESP_LOGI(_log_tag, "SPIFFS_check() successful");
        }

        return ret;
    }


    

    //***********************************************
    
    Spiffs_info Spiffs::get_spiffs_info(void)
    {
        int ret = esp_spiffs_info(_conf.partition_label, &info.total, &info.used);

        if (ret != ESP_OK) {
            ESP_LOGE(_log_tag, "ERROR: Failed to get SPIFFS partition information (%s).", esp_err_to_name(ret));
        } else {
            //ESP_LOGI(_log_tag, "Partition size: total: %d, used: %d", info.total, info.used);
            if (info.used > info.total) {
                ESP_LOGE(_log_tag, "ERROR: used > total");
        
            }
        }
        return info;
    }

}

