#include <stdio.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS
#define MAX 10

//PCB structure
typedef struct {
    int pid;
    int priority;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int response_time;
    int turnaround_time;
} PCB;

//Function to calculate average waiting time
float calculateAvgWaitingTime(PCB processes[], int n) {
    int total_waitingTime = 0;
    for (int i = 0; i < n; i++) {
        total_waitingTime = total_waitingTime + processes[i].waiting_time;
    }
    return (float)total_waitingTime / n;
}
//Function to calculate average response time
float calculateAvgResponseTime(PCB processes[], int n) {
    int total_responseTime = 0;
    for (int i = 0; i < n; i++) {
        total_responseTime = total_responseTime + processes[i].response_time;
    }
    return (float)total_responseTime / n;
}
//Function to calculate average turnaround time
float calculateAvgTurnaroundTime(PCB processes[], int n) {
    int total_turnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        total_turnaroundTime = total_turnaroundTime + processes[i].turnaround_time;
    }
    return (float)total_turnaroundTime / n;
}

int main() {
    //Read input from file
    FILE* file = fopen("input.dat", "r");
    if (file == NULL) {
        printf("Error : Cannot open file.\n");
        return 1;
    }

    //Create array of PCB
    PCB processes[10];
    int numProcesses = 0;

    //Read process information from the file
    while (fscanf(file, "%d %d %d %d", &processes[numProcesses].pid, &processes[numProcesses].priority,&processes[numProcesses].arrival_time, &processes[numProcesses].burst_time) == 4) {
        numProcesses++;
    }
    fclose(file);

    //Sort process based on arrival time
    for (int i = 0; i < numProcesses - 1; i++) {
        for (int j = 0; j < numProcesses - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                PCB temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    //Calculate waiting time, response time, turnaround time
    int currentTime = 0;
    for (int i = 0; i < numProcesses; i++) {
        if (currentTime < processes[i].arrival_time) {
            currentTime = processes[i].arrival_time;
        }

        processes[i].waiting_time = currentTime - processes[i].arrival_time;
        processes[i].response_time = processes[i].waiting_time;
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;

        currentTime = currentTime + processes[i].burst_time;
    }

    //Print chart
    printf("Scheduling : FCFS\n");
    printf("==========================================\n");

    currentTime = 0;
    for (int i = 0; i < numProcesses; i++) {
        if (currentTime < processes[i].arrival_time) {
            printf("<time %d> ---- system is idle ----\n", currentTime);
            currentTime = processes[i].arrival_time;
            printf("<time %d> [new arrival] process %d\n", currentTime, processes[i].pid);
        }

        printf("<time %d> process %d is running\n", currentTime, processes[i].pid);
        currentTime++;

        for (int i = 0; i < numProcesses; i++) {
            printf("<time %d> process %d is finished\n", currentTime, processes[i].pid);
            if (i != numProcesses - 1) {
                printf("------------------------------- (Context-Switch)\n");
                printf("<time %d> process %d is running\n", currentTime + 1, processes[i + 1].pid);
            }
            currentTime++;
        }

        //Calculate statistics
        float cpuUsage = (float)currentTime / currentTime * 100;
        float avg_waitingTime = calculateAvgWaitingTime(processes, numProcesses);
        float avg_responseTime = calculateAvgResponseTime(processes, numProcesses);
        float avg_turnaroundTime = calculateAvgTurnaroundTime(processes, numProcesses);

        //Print performance result
        printf("==========================================\n");
        printf("Average CPU usage: %.2f %%\n", cpuUsage);
        printf("Average waiting time: %.1f\n", avg_waitingTime);
        printf("Average response time: %.1f\n", avg_responseTime);
        printf("Average turnaround time: %.1f\n", avg_turnaroundTime);

        return 0;
    }
}
