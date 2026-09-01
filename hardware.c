#include "hardware.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int GetCpuTemp()
{
    char path[256] = "/sys/class/hwmon/hwmon0/name";
    FILE *f;
    int found = 0;
    int i;

    for (i = 0; i < 10; i++){
        snprintf(path, sizeof(path), "/sys/class/hwmon/hwmon%d/name", i);
        f = fopen(path, "r");
        if (f == NULL) continue;

        char string[64];
        if (fgets(string, sizeof(string), f)){
            if (strcmp("k10temp\n", string) == 0 ||
                strcmp("zenpower\n", string) == 0 ||
                strcmp("coretemp\n", string) == 0) {
                found = 1;
                fclose(f);
                break;
            }
        }
        fclose(f);
    }
    
    if (!found) return -1;

    snprintf(path, sizeof(path), "/sys/class/hwmon/hwmon%d/temp1_input", i);
    f = fopen(path, "r");
    if (f == NULL) return -1;
    
    char string[64];
    int temp;
    if (fgets(string, sizeof(string), f) && sscanf(string, "%d", &temp) == 1){
        fclose(f);
        return temp / 1000;
    }
    fclose(f);
    return -1;
}
    
                

int* GetUpTime(){
    FILE *f = fopen("/proc/uptime", "r");
    if (f == NULL) return NULL;
    char string[128];
    if (!fgets(string, sizeof(string), f)){
        fclose(f);
        return NULL;
    }
    fclose(f);

    int seconds;
    if (sscanf(string, "%d ", &seconds) != 1) return NULL;

    int* time = malloc(2 * sizeof(int));
    if (time == NULL) return NULL;

    time[0] = seconds / 3600; // time[0] hours
    time[1] = (seconds % 3600) / 60; // time[1] minutes
    return time; 
}
unsigned long* GetCpuJiffies()
{
    FILE *f = fopen("/proc/stat", "r");
    if (f == NULL) return NULL;
    char string[128];
    if (fgets(string, sizeof(string), f)){
        if (strncmp(string, "cpu ", 4) != 0){
            fclose(f);
            return NULL;
        }
        unsigned long *values = calloc(10, sizeof(unsigned long));
        if (values == NULL){
            fclose(f);
            return NULL;
        }
        int parsed = sscanf(string, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                            &values[0], &values[1], &values[2], &values[3],
                            &values[4], &values[5], &values[6],
                            &values[7], &values[8], &values[9]);
        if (parsed < 4){
            free(values);
            fclose(f);
            return NULL;
        }
        fclose(f);
        return values; 
    }
    fclose(f);
    return NULL;
}

void LongArrSum(const unsigned long *arr, size_t capacity, unsigned long *ptr)
{
    unsigned long res = 0;
    for (size_t i = 0; i < capacity; i++)
    {
        res = res + arr[i];
    }
    *ptr = res;
}

int GetCpuUsage(unsigned long *ticks1, unsigned long *ticks2)
{
    unsigned long *values1 = ticks1;
    if (values1 == NULL) return -1;
    unsigned long *values2 = ticks2;
    if (values2 == NULL){
        free(values1);
        return -1;
    }
    unsigned long total1;
    LongArrSum(values1, 10, &total1); 
    unsigned long total2;
    LongArrSum(values2, 10, &total2); 

    unsigned long idle1 = values1[3];
    unsigned long idle2 = values2[3];
    
    unsigned long total = total2 - total1;
    if (total == 0){
        free(values1);
        free(values2);
        return 0;
    }
    unsigned long idle = idle2 - idle1;

    free(values1);
    free(values2);

    double usage = 100.0 * (1.0 - (double)idle / (double)total);

    return (int)usage;
}

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
        fclose(f);
        return NULL;
    }
    name[127] = '\0';
    fclose(f);
    return name;
}
int GetCpuCores(){
    FILE* f = fopen("/proc/cpuinfo", "r");
    if(f== NULL) return -1;
    char string[128];
    int cores = 0;
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
        fclose(f);
        return freq/1000000.0;
    }
    else{
        fclose(f);
        return -1;
    }
}


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
ConfigStatus CreateConfig()
{
    int totalmem = GetMemTotal();
    if(totalmem == 0) return CONFIG_ERR_GET_MEMORY;
    int cores = GetCpuCores();
    if(cores == -1) return CONFIG_ERR_GET_CORES;
    float maxfreq  = GetMaxFreq();
    if (maxfreq == -1) return CONFIG_ERR_GET_MAX_FREQ;

    char* cpuname = GetCpuName();
    if (cpuname == NULL) return CONFIG_ERR_GET_CPU_NAME;

    char* gpuname = GetGpuName();
    if (gpuname == NULL) {
        free(cpuname);
        return CONFIG_ERR_GET_GPU_NAME;
    }

    char* osname = GetOsName();
    if (osname == NULL) {
        free(cpuname);
        free(gpuname);
        return CONFIG_ERR_GET_OS_NAME;
    }

    FILE* cnf = fopen("config.txt", "w");
    if (cnf == NULL){
        free(cpuname);
        free(gpuname);
        free(osname);
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
    free(cpuname); free(gpuname); free(osname);
    return CONFIG_CREATE;
}
ConfigStatus GetConfig(Config* cnf)
{
    char string[256];
    FILE* f = fopen("config.txt", "r");
    if (f == NULL){
        return CONFIG_ERR_FILE_NOT_FOUND;
    }
    cnf->cpu_name = malloc(128 * sizeof(char));
    if (cnf->cpu_name == NULL) return CONFIG_MEMORY_ERR;

    cnf->gpu_name = malloc(128 * sizeof(char));
    if (cnf->gpu_name == NULL){
        free(cnf->cpu_name);
        return CONFIG_MEMORY_ERR;
    }
    cnf->os_name = malloc(128 * sizeof(char));
    if (cnf->os_name == NULL){
        free(cnf->cpu_name);
        free(cnf->gpu_name);
        return CONFIG_MEMORY_ERR;
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
ConfigStatus FreeConf(Config *cnf)
{
    if (cnf == NULL) return CONFIG_ERR;
    if (cnf->cpu_name) free(cnf->cpu_name);
    if (cnf->gpu_name) free(cnf->gpu_name);
    if (cnf->os_name) free(cnf->os_name);
    return CONFIG_OK;
}

void print_hardware_info(Config cnf) {
    printf("======= SYSTEM INFO =======\n");
    printf("OS:     %s\n", cnf.os_name);
    printf("CPU:  %s FEMBOY EDITION (%d cores)\n", cnf.cpu_name, cnf.cpu_cores);
    printf("Max frequency: %.2f GHz\n", cnf.cpu_max_freq);
    printf("GPU: %s\n", cnf.gpu_name);
    printf("===========================\n\n");
}
void print_memory_bar() {
    unsigned long total = GetMemTotal();
    unsigned long avail = GetMemAvailable();
    if (total == 0 || avail == 0){
        printf("Get memory error");
        return;
    }

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
