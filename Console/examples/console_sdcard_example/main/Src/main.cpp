#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"


#define LOG_TAG "MAIN"

static Main my_main;




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
    #define BASE_PATH "/store"
    const int MAX_FILES = 20;
    esp_err_t status {ESP_OK};
    const char* prompt = "\n>> ";

    if (ESP_OK == status)
    {U0.begin(115200,1,3);}
    if (ESP_OK == status)
    {U0.flush();}

    /////////////////////////////////
    status = sd.mount(BASE_PATH,MAX_FILES);

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error mounting sdcard");
        return 0;
    }
    /////////////////////////////////

    sh.console(prompt);

    ///////////////////////
    status = sd.unmount();

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error unmounting sdcard");
        return 0;
    }
    ///////////////////////



    
    
    return ESP_OK;
}

void Main::loop(void)
{

  vTaskDelay(1);  

}



// //****************************************

// void Main::run_cmd(char** tokens)
// {
//     char data[20]{0};

//     //********* ren
//     if (!strcmp(tokens[0],"ren")){
//         if (tokens[3]!= NULL){
//             strncpy(data,"\ninvalid param!",sizeof(data));
//             U0.print((uint8_t*)data);
//             return;
//         }
//         ren(tokens[1],tokens[2]);
//     }
//         //********* cp
//     else if (!strcmp(tokens[0],"cp")){
//         if (tokens[3]!=NULL){
//             strncpy(data,"\ninvalid param!",sizeof(data));
//             U0.print((uint8_t*)data);
//             return;
//         }
//         cp(tokens[1],tokens[2]);
//     }
//     //********* rm
//     else if (!strcmp(tokens[0],"rm")){
//         if (tokens[2]!=NULL){
//             strncpy(data,"\ninvalid param!",sizeof(data));
//             U0.print((uint8_t*)data);
//             return;
//         }
//         rm(tokens[1]);
//     }
//     // invalid
//     else {
//         strncpy(data,"\ninvalid cmd!",sizeof(data));
//         U0.print((uint8_t*)data);
//     }
// }


// //****************************************

// void Main::ren(char* f1,char* f2){
//     char data[20]{0};
//     if (f1==NULL || f2==NULL){
//         strncpy(data,"\ninvalid param!",sizeof(data));
//         U0.print((uint8_t*)data);
//         return;
//     }
//     sprintf(data,"\nrename %s to %s",f2,f1);
//     U0.print((uint8_t*)data);
   
// }

// //****************************************

// void Main::rm(char* f1){
//     char data[20]{0};
//     if (f1==NULL){
//         strncpy(data,"\ninvalid param!",sizeof(data));
//         U0.print((uint8_t*)data);
//         return;
//     }
//     sprintf(data,"\nremove %s",f1);
//     U0.print((uint8_t*)data);
    
// }


// //****************************************

// void Main::cp(char* f1,char* f2){
//     char data[20]{0};
//     if (f1==NULL || f2==NULL){
//         strncpy(data,"\ninvalid param!",sizeof(data));
//         U0.print((uint8_t*)data);
//         return;
//     }
//     sprintf(data,"\ncopy %s to %s",f2,f1);
//     //ESP_LOGW(_log_tag,"%s",data);
//     U0.print((uint8_t*)data);
// }


