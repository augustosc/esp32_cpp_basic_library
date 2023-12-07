#include "main.h"
#include "string.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"


static Main my_main;


extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(my_main.setup());

    while (true)
    {
        my_main.loop();
    }
}

esp_err_t Main::setup(void)
{

/*************************************************
 *         Read file line by line
**************************************************/
    esp_err_t status = ESP_OK;
    const char* fr_name{"/spiffs/sub/data.txt"};
    const char* fw_name{"/spiffs/sub/write_data.txt"};
    const int n_lines = 100;

    copy_by_line(fw_name,fr_name,n_lines);
    read_by_line(fw_name, n_lines);

    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}


void Main::copy_by_line(const char* fw_name, const char* fr_name, int n_lines)
{
    esp_err_t status = ESP_OK;
    const char* partition_label{"storage"};
    const int MAX_LINE_SIZE=256;
    char* line = (char*)malloc(MAX_LINE_SIZE);
    int err=0;
    FILE* fr = NULL;
    FILE* fw = NULL;

    status = spf.mount(partition_label,"/spiffs",10,"true");
    ESP_LOGI(LOG_TAG, "Spiff Mounted!");

    fr = fopen(fr_name,"r");
    if (fr == NULL) {goto l_unmount;}

    fw = fopen(fw_name,"a+");
    if (fw == NULL) {goto l_unmount;}

    ESP_LOGW(LOG_TAG,"Copying by line: %s to %s",fr_name, fw_name);

    for (int i=0; i < n_lines; i++)
    {
        if(ESP_OK == status)
            spf.readln(fr,MAX_LINE_SIZE,line,err);

        if(ESP_OK == status)
            spf.writeln(fw, line);
    }

    fclose(fr);
    fclose(fw);

l_unmount:
    spf.unmount();
    ESP_LOGI(LOG_TAG, "Spiff Unmount!");

}

void Main::read_by_line(const char* f_name, int n_lines)
{
    
    esp_err_t status = ESP_OK;
    const char* partition_label{"storage"};
    const size_t MAX_LINE_SIZE = 256;
    int err =0;
    FILE* f = NULL;

    char* line = (char*)malloc(MAX_LINE_SIZE);

    // mount spiff
    status = spf.mount(partition_label,"/spiffs",10,"true");

    if (ESP_OK == status)
    {
        ESP_LOGI(LOG_TAG, "Spiff Mounted!");

        // open file
        f = fopen(f_name,"r");
        if (f == NULL)
        {
            ESP_LOGE(LOG_TAG,"Error opening file %s",f_name);
            goto l_unmount;
        }
        
        ESP_LOGW(LOG_TAG,"Reading by line file %s", f_name);

        // read lines
        for(int i=0;i<n_lines;i++)
        {
            status = spf.readln(f,MAX_LINE_SIZE,line,err);

            if(err)
            {
                ESP_LOGE(LOG_TAG,"Error reading line %d",i+1);
                break;
            }

            if(ESP_OK==status)
            {
                printf("[%d] %s",i+1,line);
            } else {
                break;
            }
        }
    }
    
    fclose(f);

l_unmount:
    free(line);
    spf.unmount();

    ESP_LOGI(LOG_TAG, "Spiff Unmount!");
}