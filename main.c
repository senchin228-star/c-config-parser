#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    CONFIG_OK = 0,
    CONFIG_CREATE = 0,
    CONFIG_ERR_FILE_NOT_FOUND,
    CONFIG_ERR_READ_FAIL,
    CONFIG_ERR_CREATE,
    CONFIG_ERR_GET_MEMORY
} ConfigStatus;

typedef struct {
    int delay;
    int max_memory;
    int core_numbers;
    int core_frequency;
} Config;

int GetMemAvailable(){
    char string[128];
    int available_mem = 0;
    FILE* meminfo = fopen("/proc/meminfo","r");
    if (meminfo == NULL){
        return 0;
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
    int totalmem = 0;
    if (meminfo == NULL) return 0;
    while (fgets(string, 128, meminfo)){
        if (sscanf(string, "MemTotal: %d", &totalmem) == 1) break;
    }
    fclose(meminfo);
    return totalmem;
}
int CreateConfig()
{
    int totalmem = GetMemTotal();
    if (totalmem == 0){
        return CONFIG_ERR_GET_MEMORY;
    }
    FILE* cnf = fopen("config.txt", "w");
    if (cnf == NULL){
        return CONFIG_ERR_CREATE;
    }
    fprintf(cnf ,"max_memory = %d\n", totalmem);
    fprintf(cnf ,"delay = 1\n");
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
        sscanf(string, "core_frequency = %d", &cnf->core_frequency);
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
    printf("Available memory: %d MB\n", memory_avail / 1024);
    printf("Max memory: %d MB", cnf.max_memory / 1024);
    return 0;
}
