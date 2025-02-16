#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

unsigned long long old_total = 0;
unsigned long long old_idle = 0;

void *monitor_cpu(void *arg)
{
    while (1)
    {
        FILE *file = fopen("/proc/stat", "r");
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
        fclose(file);

        unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
        unsigned long long diff_total = total - old_total;
        unsigned long long diff_idle = idle - old_idle;
        float cpu_usage = (1.0 - (float)diff_idle / (float)diff_total) * 100.0;

        printf("CPU Usage: %.2f%%\n", cpu_usage);

        old_total = total;
        old_idle = idle;

        sleep(1);
    }
    return NULL;
}

int mainn()
{
    pthread_t cpu_thread;
    pthread_create(&cpu_thread, NULL, monitor_cpu, NULL);

    pthread_join(cpu_thread, NULL);

    return 0;
}
