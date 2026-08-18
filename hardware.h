#ifndef HARDWARE_H
#define HARDWARE_H
#include <stddef.h>
typedef enum {
    CONFIG_ERR = 0,
    CONFIG_OK,
    CONFIG_CREATE,
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
void LongArrSum(const unsigned long *arr, size_t capacity, unsigned long *ptr);
int GetCpuUsage();

#endif


