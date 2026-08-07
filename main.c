#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    CONFIG_OK = 0,
    CONFIG_CREATE = 0,
    CONFIG_ERR_FILE_NOT_FOUND,
    CONFIG_ERR_READ_FAIL,
    CONFIG_ERR_CREATE
} ConfigStatus;

typedef struct {
    int delay;
    int max_memory;
    int core_numbers;
    int core_frequency;
} Config;

int CreateConfig()
{
    FILE* meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL){
        close(meminfo);
        return CONFIG_ERR_CREATE;
    }
    FILE* cnf = fopen("config.txt", "w");
    if (cnf == NULL){
        close(cnf);
        return CONFIG_ERR_CREATE;
    }
    char string[30];
    int totalmem = 0;
    while((fgets(string, 30, meminfo){
                sscanf(string, "MemTotal:  %d", &totalmem);
    }
    if (totalmem == 0){
        return CONFIG_ERR_CREATE;
    }
    fprintf(cnf ,"max_memory = %d\n", totalmem);
    fclose(cnf);
    fclose(meminfo);
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
    while (fgets(string, 51, f){
            sscanf(string, "delay = %d", &cnf->delay);
            sscanf(string, "max_memory = %d", &cnf->max_memory);
            sscanf(string, "core_numbers = %d", &cnf->core_numbers);
            sscanf(string, "core_frequency = %d", &cnf->core_frequency);
    }
    fclose(f);
    return CONFIG_OK;
}


