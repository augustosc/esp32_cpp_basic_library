#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include  "FileStream.h"

namespace FILESTREAM
{
    //***********************************************

    unsigned int FileStream::write_new_file(const char* f_name,
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

    unsigned int FileStream::read_file(const char* f_name, char* buf, unsigned int size_buf)
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

        buf[f_size] = '\0';

        ESP_LOGI(_log_tag,"read bytes = %u",total_read);


        fclose(f);

        return total_read;

    }


    //***********************************************

    esp_err_t FileStream::writeln(FILE* f, char * line)
    {
        errno=0;

        clearerr(f);

        int result = fputs(line,f);

        if(result<0) ESP_LOGE(_log_tag, "Error writing file %s", strerror(errno));

        return (result>0)? ESP_OK : ESP_FAIL;
    }


    //***********************************************
    
    esp_err_t FileStream::readln(FILE* f,size_t max_line_size,char* line, int err)
    {
        errno=0;

        clearerr(f);

        line = fgets(line,max_line_size,f);

        err = errno;

        if(err) ESP_LOGE(_log_tag, "Error reading file : %s", strerror(errno));

        return ((line != NULL))? ESP_OK : ESP_FAIL;

    }

    //***********************************************

    unsigned int FileStream::get_file_size(const char* f_name)
    {
        struct stat entryStat;

        // stat returns 0 on succsess
        int res = stat(f_name, &entryStat);

        if (res!=0)
            ESP_LOGE(_log_tag,"FILE SIZE ERROR");

        return entryStat.st_size;

    }



}
