#pragma once

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "Uart.h"
#include "FileStream.h"



namespace CONSOLE
{
 
    class Console 
    {
private:
   UART::Uart* _uart;
   FILESTREAM::FileStream* _disk;

   const char* _log_tag = {"Console"};

public:
    Console(UART::Uart* uart, FILESTREAM::FileStream* disk);
   void console(const char* prompt);

private:
    char* get_line(int line_size, char* old_line);

    char** parse_line(char* line);

    void printuart (uint8_t* data);

    int flushuart(void);

    int run(char** tokens);



    };

typedef int (FILESTREAM::FileStream::*PMF1)(const char*);




struct fun_table_p1_t {
		char name [20];
        int n_par;
        PMF1 fun1;
};



} // namespace CONSOLE