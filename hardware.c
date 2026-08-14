#include "hardware.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
char* GetGpuName() {
    FILE *fp = popen("lspci | grep -E 'VGA|3D' | cut -d ':' -f3", "r");
    if (fp == NULL) return NULL;
    char* name = malloc(128 * sizeof(char));
    if (name == NULL) {
        pclose(fp);
        return NULL;
    }
    char buffer[256];

    while (fgets(buffer, 256, fp)) {
        if (sscanf(buffer, " %*[^[][%127[^]]]", name) == 1) break;
    }
    pclose(fp);

    if (name[0] == '\0'){
        free(name);
        return NULL;
    }
    return name;

}

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
    fclose(f);
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
    char* gpuname = GetGpuName();
    if (gpuname == NULL) return CONFIG_ERR_GET_GPU_NAME;
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
    fprintf(cnf,"gpu name = %s\n", gpuname);
    fclose(cnf);
    return CONFIG_CREATE;
}
ConfigStatus GetConfig(Config* cnf)
{
    cnf->cpu_name = malloc(128 * sizeof(char));
    cnf->gpu_name = malloc(128 * sizeof(char));
    cnf->os_name = malloc(128 * sizeof(char));
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
        sscanf(string, "cpu max frequency = %f", &cnf->cpu_max_freq);
        sscanf(string, "cpu name = %127[^\n]", cnf->cpu_name);
        sscanf(string, "gpu name = %127[^\n]", cnf->gpu_name);
        sscanf(string, "os name = %127[^\n]", cnf->os_name);
    }
    fclose(f);
    return CONFIG_OK;
}

void print_hardware_info(Config cnf) {
    printf("======= SYSTEM INFO =======\n");
    printf("OS:     %s\n", cnf.os_name);
    printf("CPU:  %s (%d cores)\n", cnf.cpu_name, cnf.cpu_cores);
    printf("Max frequency: %.2f GHz\n", cnf.cpu_max_freq);
    printf("GPU: %s\n", cnf.gpu_name);
    printf("===========================\n\n");
}
void print_memory_bar() {
    unsigned long total = GetMemTotal();
    unsigned long avail = GetMemAvailable();

    double total_gb = (double)total / (1024 * 1024);
    double used_gb = (double)(total - avail) / (1024 * 1024);
    double percent = (used_gb / total_gb) * 100.0;

    int bar_width = 30;
    int filled = (int)((percent / 100.0) * bar_width);

    printf("RAM: [");
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) printf("#");
        else printf("-");
    }
    printf("] %.1f%% (%.2f / %.2f GB)\n", percent, used_gb, total_gb);
}
