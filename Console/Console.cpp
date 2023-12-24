#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/dirent.h>
#include <errno.h>
#include "Console.h"


//#define DEBUG



namespace CONSOLE
{
    //****************************************
    // constructor

    Console::Console(UART::Uart* uart, FILESTREAM::FileStream* disk)
    : _uart{uart},_disk{disk}
    {}



    //****************************************

   void Console::console(const char* prompt)
    {
        char* line = NULL;
        int line_size = 40;
        char** tokens;
        const char* initial_msg="Press <delete> to correct and <tab> to repeat previus command";

        char* old_line = (char*)calloc(line_size,sizeof(char));
        
        printuart((uint8_t*)initial_msg);

        while(1){
            // show prompt
            printuart((uint8_t*)prompt);

            // get line
            line = get_line(line_size,old_line);

            if (line == NULL){
                continue;
            }
            if (strlen(line)==0){
                free(line);
                continue;
            }

            // save to old_line
            strncpy(old_line,line,line_size);

            // parse line
            tokens = parse_line(line);

            if (tokens == NULL) {
                continue;
            }
            

        /****************************************************************
         * in strtok(), the first token (tokens[0]) points to the first
         * char of the line being split. The subsequent tokens points to
         * the other parts of the line. 
         * So when we free(line) we lost the content of tokens.
         * So we need free(line) when we no more need tokens.
         * We use tokens[0] to free(line).
        ******************************************************************/
            if (tokens[0] == NULL) {
                free(tokens);
                continue;
            }

            // terminate shell with "exit" string
            if(strstr(tokens[0],"exit") != NULL)
            {   
                ESP_LOGE(_log_tag,"Quiting console ...");
                ESP_LOGE(_log_tag,"press \"^]\" ...");
                free(old_line);
                free(tokens[0]); // equivalent to free(line)
                free(tokens);
                tokens=NULL;
                break;
            }

            if (tokens[0]!=NULL)
            {
                // run command
                run(tokens);
            }

            free(tokens[0]); // equivalent to free(line)
            free(tokens);
            tokens=NULL;

        }
            //return tokens;
        

    }


    //****************************************

    char* Console::get_line(int line_size, char* old_line)
    {
        uint8_t ch ='0';
        int pos =0;
        uint32_t len{0};
        const uint32_t buf_size = 8;
        const uint8_t space = 0x20;
        const uint8_t bkspace = 0x8;
        const uint8_t tab = 0x9;

        uint8_t data[buf_size]={0};

        // allocating line in heap
        char*line = (char*)calloc(line_size,sizeof(char));
        if (line == NULL)
        {
            ESP_LOGE(_log_tag,"Error creating line in heap");
            return NULL;
        }

        
        // loop to get rest of the line from uart
        while(1)
        {
            len=0;
            // receive byte from uart
            while(len==0){
                len = _uart->read(data,buf_size,0);
                vTaskDelay(1);
            }

            if (len <= 0) {
                ESP_LOGE(_log_tag,"Error reading uart");
                free(line);
                return NULL;
            }

            ch = data[0];

            // if first line char is <TAB> repeat previus command
            if ((ch == tab) && (pos == 0)){
                // get size of old_line
                len = strlen(old_line);

                // copy old_line to line
                strncpy(line,old_line,line_size);

                // write line to uart
                len = _uart->write(line,len);

                // update line pos
                pos = len;

                continue;

            }
            // echo byte to uart
            len = _uart->write(data,len);

            // if backspace return 1 position on buffer
            if (ch == bkspace){
                len = _uart->write(&space,len);
                len = _uart->write(&bkspace,len);
                pos--;
                continue;
            }

            if (ch == '\n'|| ch=='\r'){
                // finish line string with \0 and return line
                line[pos]=0;
                return line;
            }

            // insert byte in line[] and points to next line position
            line[pos++]=ch;
        }
    }




    //****************************************

    char** Console::parse_line(char* line)
    {
        char** tokens = (char**)malloc(10*sizeof(char*));


        char *token;
        int pos = 0;

        if(!tokens)
        {
            ESP_LOGE(_log_tag,"Error allocating tokens\n");
            return NULL;
        }

        // get first param
        token = strtok(line, " ");

        // get next param
        while (token != NULL) {
            tokens[pos] = token;
            pos++;

            token = strtok(NULL, " ");
        }
        tokens[pos]=NULL;

        return tokens;

    }



    //****************************************

    int Console::run(char** tokens)
    {
        char data[100]{0};
        int found = -1;

        PMF1 pmf1 = NULL;

        struct fun_table_p1_t fun_table[]{
            {"rm",1,&FILESTREAM::FileStream::rm},
            {"rmtree",1,&FILESTREAM::FileStream::rmtree},
            {"ls",1,&FILESTREAM::FileStream::ls},
            {"lstree",1,&FILESTREAM::FileStream::lstree},
            {"info",1,&FILESTREAM::FileStream::info},
            {"exists",1,&FILESTREAM::FileStream::exists},
            {"cat",1,&FILESTREAM::FileStream::cat},
            {"cp",2,NULL},
            {"cptree",2,NULL},
            {"ren",2,NULL},
            // {"write",3,NULL},
            // {"cat",3,NULL},
            // {"cptree",3,NULL},
            // {"lookup",3,NULL},
        };

         int n_fun = sizeof(fun_table)/sizeof(fun_table[0]);

        // check if is a valid command
        for(int i=0;i<n_fun;i++)
        {
            if(!strcmp(fun_table[i].name,tokens[0]))
            {
                if(fun_table[i].n_par == 1)
                    pmf1 = fun_table[i].fun1;

                found = i;
            }
            else continue;
        }
        // return invalid command if not found
        if (found == -1)
        {
            strncpy(data,"\ninvalid command!",sizeof(data));
            printuart((uint8_t*)data);
            return -1;
        }

        //check if is a valid number of param
        for (int i=1; i <= fun_table[found].n_par; i++)
        {
            if (tokens[i] == NULL) {
                strncpy(data,"\ninvalid param!",sizeof(data));
                printuart((uint8_t*)data);
                return -1;
            }
        }

        // new line
        strncpy(data,"\n",sizeof(data));
        printuart((uint8_t*)data);

        // run functions with n_par ==1
        if ((pmf1 != NULL) && (fun_table[found].n_par == 1))
        return (_disk->*pmf1)(tokens[1]);

        // run functions with n_par ==2 
        if (!strcmp(fun_table[found].name,"ren"))
        {
            return _disk->ren(tokens[1],tokens[2]);
        }
        if (!strcmp(fun_table[found].name,"cp"))
        {
            return _disk->cp(tokens[1],tokens[2]);
        }
        if (!strcmp(fun_table[found].name,"cptree"))
        {
            return _disk->cptree(tokens[1],tokens[2]);
        }
        return -1;
    }


    //****************************************

    void Console::printuart (uint8_t* data)
    {
        _uart->write(data,strlen((char*)data)+1);
    }


    //****************************************

    int Console::flushuart ()
    {
        return _uart->flush();
        
    }



} //namespace SHELL