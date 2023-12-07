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
 *         Write file line by line
 * 1. copy fr_name to fw_name
 * 2. read and print fw_name
**************************************************/
    esp_err_t status = ESP_OK;
    const char* fr_name{"/spiffs/sub/data.txt"};
    const char* fw_name{"/spiffs/sub/write_data.txt"};
    const int n_lines = 100;

    ESP_LOGW(LOG_TAG,"Copying by line: %s ---> %s",fr_name, fw_name);
    copy_by_line(fw_name,fr_name,n_lines);

    ESP_LOGW(LOG_TAG,"Reading and printing by line file %s", fw_name);
    read_by_line(fw_name, n_lines);

    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}

/*************************************************
 *            Copy files line by line
**************************************************/
void Main::copy_by_line(const char* fw_name, const char* fr_name, const int n_lines)
{
    
    const char* PARTITION_LABEL{"storage"};
    const char* PATH = "/spiffs";
    const int MAX_FILES = 10;
    const int MAX_LINE_SIZE=256;

    esp_err_t status = ESP_OK;
    char* line = (char*)malloc(MAX_LINE_SIZE);
    int err=0;
    FILE* fr = NULL;
    FILE* fw = NULL;

    // mount spiffs with MAX_FILES files
    status = spf.mount(PARTITION_LABEL,PATH,MAX_FILES,"true");
    ESP_LOGI(LOG_TAG, "Spiff Mounted!");

    // open file to read
    fr = fopen(fr_name,"r");
    if (fr == NULL) {
        ESP_LOGE(LOG_TAG, "Error opening %s", fr_name);
        goto l_unmount;
    }
    // open file to write
    fw = fopen(fw_name,"a+");
    if (fw == NULL) {
        ESP_LOGE(LOG_TAG, "Error opening %s", fw_name);
        goto l_unmount;
    }

    // copy n_lines
    for (int i=0; i < n_lines; i++)
    {
        if(ESP_OK == status)
            spf.readln(fr,MAX_LINE_SIZE,line,err);

        if(ESP_OK == status)
            spf.writeln(fw, line);
    }

    // close files
    fclose(fr);
    fclose(fw);

// unmount spiffs
l_unmount:
    spf.unmount();
    ESP_LOGI(LOG_TAG, "Spiff Unmount!");

}


/*************************************************
 *            Read and print file line by line
**************************************************/
void Main::read_by_line(const char* f_name, const int n_lines)
{
    
    
    const char* PARTITION_LABEL{"storage"};
    const size_t MAX_LINE_SIZE = 256;

    esp_err_t status = ESP_OK;
    int err =0;
    FILE* f = NULL;

    // create buffer to receive line read
    char* line = (char*)malloc(MAX_LINE_SIZE);

    // mount spiff
    status = spf.mount(PARTITION_LABEL,"/spiffs",10,"true");

    if (ESP_OK == status)
    {
        ESP_LOGI(LOG_TAG, "Spiff Mounted!");

        // open file to read
        f = fopen(f_name,"r");
        if (f == NULL)
        {
            ESP_LOGE(LOG_TAG,"Error opening file %s",f_name);
            goto l_unmount;
        }
        

        // read n_lines line by line
        for(int i=0;i<n_lines;i++)
        {
            status = spf.readln(f,MAX_LINE_SIZE,line,err);

            if(err)
            {
                ESP_LOGE(LOG_TAG,"Error reading line %d",i+1);
                break;
            }

            if(ESP_OK==status)
            {   // print line
                printf("[%d] %s",i+1,line);
            } else {
                break;
            }
        }
    }
    // close file
    fclose(f);

// unmount spiffs
l_unmount:
    free(line);
    spf.unmount();

    ESP_LOGI(LOG_TAG, "Spiff Unmount!");
}