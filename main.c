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
    CONFIG_ERR_GET_MAX_FREQ,
    CONFIG_ERR_GET_CORES,
    CONFIG_ERR_GET_CPU_NAME,
    CONFIG_ERR_GET_OS_NAME
} ConfigStatus;

typedef struct {
    int delay;
    int max_memory;
    int cpu_cores;
    float cpu_max_frequency;
    char* cpu_name;
    char* os_name;
} Config;

char* GetOsName(){
    char* name = malloc(128 * sizeof(char));
    name[0] = '\0';
    char string[128];
    FILE* f = fopen("/etc/os-release", "r");
    if (f == NULL) return NULL;

    while(fgets(string, 128, f)){
        if (sscanf(string, "PRETTY_NAME=%127[^\n]", name) == 1) break;
    }
    if (name[0] == '\0'){
        free(name);
        return NULL;
    }
    name[127] = '\0';
    return name;
}

char* GetCpuName(){

    char* name = malloc(128 * sizeof(char));
    name[0] = '\0';
    char string[128];

    FILE* f = fopen("/proc/cpuinfo","r");
    if (f == NULL) return NULL;

    while(fgets(string, 128, f)){
        if (sscanf(string, "model name :  %127[^\n]", name) == 1) break;
    }
    if (name[0] == '\0'){
        free(name);
        return NULL;
    }
    name[127] = '\0';
    return name;
}
int GetCpuCores(){
    FILE* f = fopen("/proc/cpuinfo", "r");
    if(f== NULL) return -1;
    char string[128];
    int cores;
    while (fgets(string, 128, f)){
        if (sscanf(string, "cpu cores : %d", &cores) == 1) break;
    }
    if (!cores) return -1;
    return cores;
}

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
    fclose(f);
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
    int cores = GetCpuCores();
    if (cores == -1) return CONFIG_ERR_GET_CORES;
    float maxfreq = GetMaxFreq();
    if (maxfreq == -1) return CONFIG_ERR_GET_MAX_FREQ;
    char* cpuname = GetCpuName();
    if (cpuname == NULL) return CONFIG_ERR_GET_CPU_NAME;
    char* osname = GetOsName();
    if (osname == NULL) return CONFIG_ERR_GET_OS_NAME;

    FILE* cnf = fopen("config.txt", "w");
    if (cnf == NULL){
        return CONFIG_ERR_CREATE;
    }
    fprintf(cnf ,"delay = 1\n");
    fprintf(cnf ,"max memory = %d\n", totalmem);
    fprintf(cnf,"cpu max frequency = %.2f\n", maxfreq);
    fprintf(cnf,"cpu cores = %d\n", cores);
    fprintf(cnf,"cpu name = %s\n", cpuname);
    fprintf(cnf,"os name = %s\n", osname);
    fclose(cnf);
    return CONFIG_CREATE;
}
ConfigStatus GetConfig(Config* cnf)
{
    cnf->cpu_name = malloc(128 * sizeof(char));
    char string[256];
    FILE* f = fopen("config.txt", "r");
    if (f == NULL){
        if (CreateConfig()) {
            fputs("Config created", stdout);
        }
        return CONFIG_ERR_FILE_NOT_FOUND;
    }
    while (fgets(string, 256, f)){
        sscanf(string, "delay = %d", &cnf->delay);
        sscanf(string, "max memory = %d", &cnf->max_memory);
        sscanf(string, "cpu cores = %d", &cnf->cpu_cores);
        sscanf(string, "cpu max frequency = %f", &cnf->cpu_max_frequency);
        sscanf(string, "cpu name = %127[^\n]", cnf->cpu_name);
        sscanf(string, "os name = %127[^\n]", cnf->os_name);
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
    printf("Cores: %d\n", cnf.cpu_cores); 
    printf("CPU Name: %s\n", cnf.cpu_name); 
    printf("Os Name: %s\n", cnf.os_name); 
    return 0;
}
