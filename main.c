#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    CONFIG_OK = 0,
    CONFIG_CREATE = 0,
    CONFIG_ERR_FILE_NOT_FOUND,
    CONFIG_ERR_READ_FAIL,
    CONFIG_ERR_CREATE,
    CONFIG_ERR_GET_MEMORY,
    CONFIG_ERR_GET_MAX_FREQ
} ConfigStatus;

typedef struct {
    int delay;
    int max_memory;
    int core_numbers;
    float cpu_max_frequency;
} Config;

float GetMaxFreq(){
    FILE* f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r");
    if (f == NULL) return -1;
    float freq;
    char string[128];
    fgets(string, 128, f);
    if (sscanf(string, "%f", &freq) == 1){
        return freq/1000000.0;
    }
    else{
        return -1;
    }
}


int GetMemAvailable(){
    char string[128];
    int available_mem = 0;
    FILE* meminfo = fopen("/proc/meminfo","r");
    if (meminfo == NULL){
        return -1;
    }
    while (fgets(string, 128, meminfo)){
        if(sscanf(string, "MemAvailable: %d", &available_mem) == 1){
            break;
        }
    }
    fclose(meminfo);
    return available_mem;
}
int GetMemTotal(){
    char string[128];
    FILE* meminfo = fopen("/proc/meminfo","r");
    int totalmem = -1;
    if (meminfo == NULL) return -1;
    while (fgets(string, 128, meminfo)){
        if (sscanf(string, "MemTotal: %d", &totalmem) == 1) break;
    }
    fclose(meminfo);
    return totalmem;
}
ConfigStatus CreateConfig()
{
    int totalmem = GetMemTotal();
    if (totalmem == -1) return CONFIG_ERR_GET_MEMORY;
    float maxfreq = GetMaxFreq();
    if (maxfreq == -1) return CONFIG_ERR_GET_MAX_FREQ;
    FILE* cnf = fopen("config.txt", "w");
    if (cnf == NULL){
        return CONFIG_ERR_CREATE;
    }
    fprintf(cnf ,"delay = 1\n");
    fprintf(cnf ,"max_memory = %d\n", totalmem);
    fprintf(cnf,"cpu_max_frequency = %.2f\n", maxfreq);
    fclose(cnf);
    return CONFIG_CREATE;
}
ConfigStatus GetConfig(Config* cnf)
{
    char string[51];
    FILE* f = fopen("config.txt", "r");
    if (f == NULL){
        if (CreateConfig()) {
            fputs("Config created", stdout);
        }
        return CONFIG_ERR_FILE_NOT_FOUND;
    }
    while (fgets(string, 51, f)){
        sscanf(string, "delay = %d", &cnf->delay);
        sscanf(string, "max_memory = %d", &cnf->max_memory);
        sscanf(string, "core_numbers = %d", &cnf->core_numbers);
        sscanf(string, "cpu_max_frequency = %f", &cnf->cpu_max_frequency);
    }
    fclose(f);
    return CONFIG_OK;
}
int main()
{
    int memory_avail = GetMemAvailable();
    Config cnf;
    CreateConfig();
    GetConfig(&cnf);
    printf("Delay: %d \n", cnf.delay);
    printf("Available memory: %d MB\n", memory_avail / 1024);
    printf("Max memory: %d MB\n", cnf.max_memory / 1024);
    printf("Max frequency: %.2f GHz\n", cnf.cpu_max_frequency);
    return 0;
}
