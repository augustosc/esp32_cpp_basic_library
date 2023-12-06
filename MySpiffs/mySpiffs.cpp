#include "mySpiffs.h"

#include <string.h>

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

    FILE* Spiffs::open_file(const char* f_name,
                    const char* mode)
    {
        FILE* f = fopen(f_name, mode);
        if (f == NULL) {
        ESP_LOGE(_log_tag,"FILE OPEN ERROR");
        }
        return f;
    }


    //***********************************************

    unsigned int Spiffs::write_new_file(const char* f_name,
                            const char* content, unsigned int f_size)
    {
        FILE* f = fopen(f_name, "wb");
        if (!f)
        {
            ESP_LOGE(_log_tag,"FILE OPEN ERROR");
            return ESP_FAIL;
        }

        unsigned int n = fwrite(content,sizeof(char),f_size,f);

        fclose(f);
        
        return n;
    }



    //***********************************************

    unsigned int Spiffs::read_file(const char* f_name, char* buf, unsigned int size_buf)
    {
        //esp_err_t status{ESP_OK};
        unsigned int f_size{};
        const unsigned int chunk_size{1024};
        unsigned int total_read{};
        unsigned int count{};

        f_size = get_file_size(f_name);
        //ESP_LOGI(_log_tag,"FILE SIZE = %u",f_size);
        //ESP_LOGI(_log_tag,"BUFFER SIZE = %u",size_buf);

        // check buffer size sufficiency
        if(size_buf < f_size )
        {
            ESP_LOGE(_log_tag,"INSUFFICIENT BUFFER SIZE: file size = %u bytes",f_size);
            return ESP_FAIL;
        }

        // file open in binary
        FILE* f = fopen(f_name, "rb");
        if (!f)
        {
            ESP_LOGE(_log_tag,"FILE OPEN ERROR");
            return ESP_FAIL;
        }

        ////////////////////////////////////
        // read long files in chunks to prevent WDT trigger
        if (f_size > chunk_size)
        {
            ESP_LOGI(_log_tag,"Running chunk read");
            
            while(total_read < f_size)
            {
                count = fread(&buf[total_read],1,chunk_size,f);
                total_read += count;
                //ESP_LOGI(_log_tag,"COUNT = %ld", count);
                //ESP_LOGI(_log_tag,"TOTAL_READ = %ld",total_read );
                vTaskDelay(1);
            }
        ////////////////////////////////////

        }else {
        total_read = fread(buf,1,f_size,f);
        }

        ESP_LOGI(_log_tag,"read bytes = %u",total_read);

        fclose(f);

        return total_read;

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

    unsigned int Spiffs::get_file_size(const char* f_name)
    {
        struct stat entryStat;

        // stat returns 0 on succsess
        int res = stat(f_name, &entryStat);

        if (res!=0)
            ESP_LOGE(_log_tag,"FILE SIZE ERROR");

        return entryStat.st_size;

    }



    //***********************************************
    
    Spiffs_info Spiffs::get_spiffs_info()
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

