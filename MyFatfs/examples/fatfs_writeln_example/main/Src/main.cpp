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
    const char *BASE_PATH = "/myfatfs";

    const char *READ_FOLDER = "sub/data.txt";
    const char *WRITE_FOLDER = "sub/write_data.txt";

    char fr_name[128]{};
    char fw_name[128]{};

    sprintf(fr_name,"%s/%s",BASE_PATH,READ_FOLDER);
    sprintf(fw_name,"%s/%s",BASE_PATH,WRITE_FOLDER);

    const int n_lines = 100;

    ESP_LOGW(LOG_TAG,"Copying by line: %s ---> %s",fr_name, fw_name);
    copy_by_line(BASE_PATH,fw_name,fr_name,n_lines);

    ESP_LOGW(LOG_TAG,"Reading and printing by line file %s", fw_name);
    read_by_line(BASE_PATH,fw_name, n_lines);

    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}

/*************************************************
 *            Copy files line by line
**************************************************/
void Main::copy_by_line(const char* BASE_PATH,const char* fw_name, const char* fr_name, const int n_lines)
{
    
    const char* PARTITION_LABEL{"storage"};
    const int MAX_LINE_SIZE=256;

    esp_err_t status = ESP_OK;
    char* line = (char*)malloc(MAX_LINE_SIZE);
    int err=0;
    FILE* fr = NULL;
    FILE* fw = NULL;

    // mount fatfs with MAX_FILES files
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,NULL);
    ESP_LOGI(LOG_TAG, "Fatfs Mount!");

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
            mydisk.readln(fr,MAX_LINE_SIZE,line,err);

        if(ESP_OK == status)
            mydisk.writeln(fw, line);
    }

    // close files
    fclose(fr);
    fclose(fw);

// unmount fatfs
l_unmount:
    mydisk.unmount();
    ESP_LOGI(LOG_TAG, "Fatfs Unmount!");

}


/*************************************************
 *            Read and print file line by line
**************************************************/
void Main::read_by_line(const char* BASE_PATH,const char* f_name, const int n_lines)
{
    
    
    const char* PARTITION_LABEL{"storage"};
    const size_t MAX_LINE_SIZE = 256;

    esp_err_t status = ESP_OK;
    int err =0;
    FILE* f = NULL;

    // create buffer to receive line read
    char* line = (char*)malloc(MAX_LINE_SIZE);

    // mount spiff
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,NULL);

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
            status = mydisk.readln(f,MAX_LINE_SIZE,line,err);

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

// unmount fatfs
l_unmount:
    free(line);
    mydisk.unmount();

    ESP_LOGI(LOG_TAG, "Spiff Unmount!");
}