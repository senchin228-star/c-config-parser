#ifndef HARDWARE_H
#define HARDWARE_H
#include <stddef.h>

typedef enum {
    CONFIG_ERR = 0,
    CONFIG_OK,
    CONFIG_CREATE,
    CONFIG_MEMORY_ERR,
    CONFIG_ERR_FILE_NOT_FOUND,
    CONFIG_ERR_READ_FAIL,
    CONFIG_ERR_CREATE,
    CONFIG_ERR_GET_MEMORY,
    CONFIG_ERR_GET_MAX_FREQ,
    CONFIG_ERR_GET_CORES,
    CONFIG_ERR_GET_CPU_NAME,
    CONFIG_ERR_GET_GPU_NAME,
    CONFIG_ERR_GET_OS_NAME
} ConfigStatus;

typedef struct {
    int delay;
    int max_memory;
    int cpu_cores;
    float cpu_max_freq;
    char* cpu_name;
    char* gpu_name;
    char* os_name;
} Config;

unsigned long long GetVRAM();
char* GetGpuName();
char* GetOsName();
char* GetCpuName();
int GetCpuCores();
float GetMaxFreq();
int GetMemAvailable();
int GetMemTotal();
ConfigStatus CreateConfig();
ConfigStatus GetConfig(Config* cnf);
ConfigStatus FreeConf(Config* cnf);
void print_hardware_info(Config cnf);
void print_memory_bar();
unsigned long* GetCpuJiffies();
int GetCpuUsage(unsigned long *ticks1, unsigned long *ticks2);
int* GetUpTime();
int GetCpuTemp();

#endif


