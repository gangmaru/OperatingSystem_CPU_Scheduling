#include <stdio.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS
#define MAX 10
#define TIMEQUANTUM 1

//PCB structure
struct Process {
    int pid;
    int priority;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
};

struct Node {
    struct Process data;
    struct Node* next;
};

struct Queue {
    struct Node* front;
    struct Node* rear;
};

void enqueue(struct Queue* queue, struct Process process) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = process;
    newNode->next = NULL;
    if (queue->rear == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

struct Process dequeue(struct Queue* queue) {
    if (queue->front == NULL) {
        struct Process emptyProcess = {0};
        return emptyProcess;
    }
    struct Node* frontNode = queue->front;
    struct Process frontProcess = frontNode->data;
    if (queue->front == queue->rear) {
        queue->front = NULL;
        queue->rear = NULL;
    }
    else {
        queue->front = queue->front->next;
    }
    free(frontNode);
    return frontProcess;
}

int queueEmpty(struct Queue* queue) {
    return (queue->front == NULL);
}

void initProcesses(struct Process processes[], char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error : Cannot open file.\n");
        exit(1);
    }

    for (int i = 0; i < MAX; i++) {
        fscanf(file, "%d %d %d %d", &processes[i].pid, &processes[i].priority, &processes[i].arrival_time, &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = -1;
    }
    fclose(file);
}

void print(int time, struct Process* process) {
    printf("<time %d>", time);
    if (process->pid != 0) {
        printf(" process %d is running\n", process->pid);
    }
    else {
        printf(" ---- system is idle ----\n");
    }
}

void printPerformance(struct Process processes[], int n) {
    float cpuUsage = 0, avgWaitingTime = 0, avgResponseTime = 0, avgTurnaroundTime = 0;

    for (int i = 0; i < n; i++) {
        cpuUsage = cpuUsage + processes[i].burst_time;
        avgWaitingTime = avgWaitingTime + processes[i].waiting_time;
        avgResponseTime = avgResponseTime + processes[i].response_time;
        avgTurnaroundTime = avgTurnaroundTime + processes[i].turnaround_time;
    }

    cpuUsage = (cpuUsage / TIMEQUANTUM) * 100;
    avgWaitingTime = avgWaitingTime / n;
    avgResponseTime = avgResponseTime / n;
    avgTurnaroundTime = avgTurnaroundTime / n;

    printf("==========================================\n");
    printf("Average CPU usage: %.2f %%\n", cpuUsage);
    printf("Average waiting time: %.2f\n", avgWaitingTime);
    printf("Average response time: %.2f\n", avgResponseTime);
    printf("Average turnaround time: %.2f\n", avgTurnaroundTime);
    printf("==========================================\n");
}

void RR(struct Process processes[], int totalProcesses) {
    struct Queue readyQueue;
    readyQueue.front = NULL;
    readyQueue.rear = NULL;

    int currentTime = 0;
    int completedProcesses = 0;
    int currentProcessIndex = -1;
    int timeQuantumCounter = 0;

    while (completedProcesses < totalProcesses) {
        //Check new arrivals
        for (int i = 0; i < totalProcesses; i++) {
            if (processes[i].arrival_time == currentTime) {
                processes[i].response_time = currentTime;
                enqueue(&readyQueue, processes[i]);
            }
        }

        if (currentProcessIndex == -1) {
            //If no process is currently running, dequeue the next process from the ready queue
            struct Process newProcess = dequeue(&readyQueue);
            if (newProcess.pid != 0) {
                currentProcessIndex = newProcess.pid - 1;
                timeQuantumCounter = 0;
                print(currentTime, &processes[currentProcessIndex]);
            }
            else {
                print(currentTime, &newProcess);
            }
        }
        else {
            //If process is currently running, continue executing it
            struct Process* currentProcess = &processes[currentProcessIndex];

            if (currentProcess->remaining_time > TIMEQUANTUM) {
                //Process still needs more time, so reduce its remaining time and check if time quantum has expired
                currentProcess->remaining_time = currentProcess->remaining_time - TIMEQUANTUM;
                timeQuantumCounter = timeQuantumCounter + TIMEQUANTUM;

                if (timeQuantumCounter >= TIMEQUANTUM) {
                    //Time quantum expired, context switch
                    enqueue(&readyQueue, *currentProcess);
                    currentProcessIndex = -1;
                    timeQuantumCounter = 0;
                }
            }
            else {
                //Process will complete execution
                timeQuantumCounter = timeQuantumCounter + currentProcess->remaining_time;
                currentProcess->remaining_time = 0;
                currentProcess->turnaround_time = currentTime - currentProcess->arrival_time + 1;
                currentProcess->waiting_time = currentProcess->turnaround_time - currentProcess->burst_time;
                completedProcesses++;
                currentProcessIndex = -1;
                timeQuantumCounter = 0;
            }
            print(currentTime, currentProcess);
        }
        currentTime++;
    }
    printPerformance(processes, totalProcesses);
}

int main() {
    struct Process processes[MAX];

    char* filename = "input.dat";

    initProcesses(processes, filename);

    printf("Scheduling: RR\n");
    RR(processes, MAX);

    return 0;
}