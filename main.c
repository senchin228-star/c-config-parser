#include <stdio.h>
#include "hardware.h"
#include <unistd.h>
int main()
{
    CreateConfig();
    Config cnf;
    GetConfig(&cnf);
    print_hardware_info(cnf);
    printf("Delay: %d sec\n", cnf.delay);
    while (1){
        print_memory_bar();
        printf("\033[1A");
        sleep(cnf.delay);
    }
    return 0;
}
