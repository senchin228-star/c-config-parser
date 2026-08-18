#include <stdio.h>
#include "hardware.h"
#include <unistd.h>
int main()
{
    if (CreateConfig() != CONFIG_CREATE){
        printf("Config not created");
        return 1;
    }
    Config cnf;
    if(GetConfig(&cnf) != CONFIG_OK){
        printf("Something wrong in config");
        return 2;
    }
    print_hardware_info(cnf);
    printf("Delay: %d sec\n", cnf.delay);
    FreeConf(&cnf);
    while (1){
        print_memory_bar();
        printf("\033[1A");
        sleep(cnf.delay);
    } 
    return 0;
}
