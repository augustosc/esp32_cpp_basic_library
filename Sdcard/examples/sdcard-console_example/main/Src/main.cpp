#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"


#define LOG_TAG "MAIN"

static Main my_main;

/************************************************
 *             Sdcard Console Example:
 * We will use uart console to test file handling
 * functions in "fatfs" and "sdcard" libraries.
 ************************************************
    * try cp file1 file2
    * try cp file1
    * try cp
    * try the same sequence with "ren" and "rm"
    * try ls
*************************************************/


extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(my_main.setup());

    while (true)
    {
        my_main.loop();
    }
}


esp_err_t Main::setup(void)
{
    esp_err_t status {ESP_OK};

    const char *BASE_PATH = "/store";
    const int MAX_FILES = 10;

    vTaskDelay(500/portTICK_PERIOD_MS);

    const char* prompt = "\n>> ";
    char** tokens;


    //////////////////////////////
    status = sd.mount(BASE_PATH,MAX_FILES);
    if (ESP_OK != status){
        ESP_LOGE(LOG_TAG, "Error mounting sdcard");
        return 0;
    }
    //////////////////////////////

    if (ESP_OK == status)
    {U0.begin(115200,1,3);}
    if (ESP_OK == status)
    {U0.flush();}

    while(1){
        tokens = U0.console(prompt);
        
        if (tokens[0] == NULL) {
            continue;
        }


        // terminate shell with "end" string
        if(strstr(tokens[0],"end") != NULL)
        {   
            free(tokens);
            tokens=NULL;
            break;
        }
        if (tokens[0]!=NULL)
        {
            run_cmd(tokens);
        }

        /****************************************************************
         * in strtok(), the first token (tokens[0]) points to the first
         * char of the line being split. The subsequent tokens points to
         * the other parts of the line. 
         * So when we free(line) we lost the content of tokens.
         * So we need free(line) when we no more need tokens.
         * We use tokens[0] to free(line).
        ******************************************************************/

        free(tokens[0]); ///< freeing line
        free(tokens);
        tokens=NULL;

    }

    //////////////////////////////
    status = sd.unmount();

    if(ESP_OK != status)
    {
        ESP_LOGE(LOG_TAG, "Error unmounting sdcard");
        return 0;
    }
    //////////////////////////////
    
    return ESP_OK;
}

void Main::loop(void)
{

  vTaskDelay(1);  

}



//****************************************

void Main::run_cmd(char** tokens)
{
    char data[20]{0};

    //********* ren
    if (!strcmp(tokens[0],"ren")){
        if (tokens[3]!= NULL){
            strncpy(data,"\ninvalid param!",sizeof(data));
            U0.print((uint8_t*)data);
            return;
        }
        ren(tokens[1],tokens[2]);
    }
        //********* cp
    else if (!strcmp(tokens[0],"cp")){
        if (tokens[3]!=NULL){
            strncpy(data,"\ninvalid param!",sizeof(data));
            U0.print((uint8_t*)data);
            return;
        }
        cp(tokens[1],tokens[2]);
    }
    //********* rm
    else if (!strcmp(tokens[0],"rm")){
        if (tokens[2]!=NULL){
            strncpy(data,"\ninvalid param!",sizeof(data));
            U0.print((uint8_t*)data);
            return;
        }
        rm(tokens[1]);
    }
    // invalid
    else {
        strncpy(data,"\ninvalid cmd!",sizeof(data));
        U0.print((uint8_t*)data);
    }
}


//****************************************

void Main::ren(char* f1,char* f2){
    char data[20]{0};
    if (f1==NULL || f2==NULL){
        strncpy(data,"\ninvalid param!",sizeof(data));
        U0.print((uint8_t*)data);
        return;
    }
    sprintf(data,"\nrename %s to %s",f2,f1);
    U0.print((uint8_t*)data);
   
}

//****************************************

void Main::rm(char* f1){
    char data[20]{0};
    if (f1==NULL){
        strncpy(data,"\ninvalid param!",sizeof(data));
        U0.print((uint8_t*)data);
        return;
    }
    sprintf(data,"\nremove %s",f1);
    U0.print((uint8_t*)data);
    
}


//****************************************

void Main::cp(char* f1,char* f2){
    char data[20]{0};
    if (f1==NULL || f2==NULL){
        strncpy(data,"\ninvalid param!",sizeof(data));
        U0.print((uint8_t*)data);
        return;
    }
    sprintf(data,"\ncopy %s to %s",f2,f1);
    //ESP_LOGW(_log_tag,"%s",data);
    U0.print((uint8_t*)data);
}


