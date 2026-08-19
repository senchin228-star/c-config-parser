#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "hardware.h"
int main()
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

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

    int* time = GetUpTime();
    printf("UPTIME: Hours: %d, Minutes: %d\n", time[0], time[1]);
    free(time); 
    while (1){
        unsigned long *ticks1 = GetCpuJiffies();
        sleep(cnf.delay);
        
        unsigned long *ticks2 = GetCpuJiffies();
        int usage = GetCpuUsage(ticks1, ticks2);

        print_memory_bar();
        int cputemp = GetCpuTemp();
        printf("Usage: %3d%%\nCpu Temp: %d°C\n", usage, cputemp);
        printf("\033[3A");
    } 
    return 0;
}
