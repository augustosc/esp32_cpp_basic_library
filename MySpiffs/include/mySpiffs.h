#pragma once

#include <stdio.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"



extern "C" {
#include "esp_spiffs.h"
}

namespace SPIFFS
{
    struct Spiffs_info{
        size_t total{};
        size_t used{};
    };

    class Spiffs
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
        /// @param f_name file name
        /// @return file size
        unsigned int get_file_size(const char* f_name);
        
        
        //*********************************************** 
        /// @param f_name file name
        /// @param content data to be written
        /// @param f_size data size
        /// @return bytes written
        unsigned int write_new_file(const char* f_name,
                            const char* content,unsigned int f_size);


        //***********************************************  
        /// @param f_name file name
        /// @param buf buffer to save read bytes
        /// @param size_buf buffer size
        /// @return bytes read
        unsigned int read_file(const char* f_name, char* buf, unsigned int size_buf);


        ///*********************************************** 
        /// @brief writeln to a file stream
        /// @param f pointer to file stream
        /// @param line line to be written
        /// @return bytes written
        esp_err_t writeln(FILE* f, char * line);

        //***********************************************  
        /// @param f pointer to file stream
        /// @param max_line_size max line size
        /// @param line buffer to save read line
        /// @param err returns errno
        /// @return ESP_OK on success
        esp_err_t readln(FILE* f,size_t max_line_size,char* line,int err);


        //***********************************************  
        /// @brief unregister spiffs
        /// @return ESP_OK on success
        esp_err_t unmount()
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
        Spiffs_info get_spiffs_info();

    };

} // namespace SPIFFS