#pragma once

#include <stdio.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "FileStream.h"



extern "C" {
#include "esp_spiffs.h"
}

namespace SPIFFS
{
    struct Spiffs_info{
        size_t total{};
        size_t used{};
    };

    class Spiffs : public FILESTREAM::FileStream
    {
    private:
        esp_vfs_spiffs_conf_t _conf{};
        const char* _log_tag{"SPIFFS"};
        struct Spiffs_info info;
        

    public:
        //***********************************************
        /// @brief register spiffs
        /// @param p_label partition label
        /// @param path spiffspath
        /// @param max max files
        /// @param format format on failure
        /// @return ESP_OK on success
        esp_err_t mount(const char* p_label,
                        const char* path,
                        size_t max,
                        bool format);
        

        //***********************************************  
        /// @brief unregister spiffs
        /// @return ESP_OK on success
        esp_err_t unmount(void)
        {
            return esp_vfs_spiffs_unregister(_conf.partition_label);
        }

        /*************************************************
         * Before running spiffs_check:
         * Disable Watch CPU0 Idle Task in MENUCONFIG to
         * prevent WDT trigger.
        **************************************************/
        /// @brief check spifss consistency
        /// @param p_label partition label
        /// @return ESP_OK on success
        esp_err_t spiffs_check(const char* p_label);


        //*************************************************
        /// @return info.used & info.total
        Spiffs_info get_spiffs_info(void);

    };

} // namespace SPIFFS