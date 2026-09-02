#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "hardware.h"

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

    unsigned long long vram = GetVRAM();
    printf("VRAM: %llu MB\n", vram);

    GpuVendor vendor = GetGpuVendorByPciId();
    char *vendorname = GetGpuVendorName(vendor);
    printf("GPU VENDOR: %s\n", vendorname);
    if (vendorname != NULL) free(vendorname);

    while (1){
        int ProgramTime = 0;
        if (ProgramTime == 0 || ProgramTime % 60 == 0){ 
            int* time = GetUpTime();
            printf("UPTIME: Hours: %d, Minutes: %d\n", time[0], time[1]);
            free(time); 
        }
        unsigned long *ticks1 = GetCpuJiffies();
        sleep(cnf.delay);
        ProgramTime++;
        
        unsigned long *ticks2 = GetCpuJiffies();
        int usage = GetCpuUsage(ticks1, ticks2);

        print_memory_bar();
        int cputemp = GetCpuTemp();
        printf("Usage: %3d%%\nCpu Temp: %d°C\n", usage, cputemp);
        printf("\033[4A");
    } 
    return 0;
}
