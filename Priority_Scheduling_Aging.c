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

//Linkedlist structure
typedef struct Node {
    PCB process;
    struct Node* next;
} Node;

//Function to create a new Node
Node* createNode(PCB process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;
    return newNode;
}

//Function to insert a Node at the end of the list
void insertNode(Node** head, PCB process) {
    Node* newNode = createNode(process);
    if (*head == NULL) {
        *head = newNode;
    }
    else {
        Node* currNode = *head;
        while (currNode->next != NULL) {
            currNode = currNode->next;
        }
        currNode->next = newNode;
    }
}

//Function to remove and return the Node with the highest priority
Node* removeHighestPriorityNode(Node** head) {
    if (*head == NULL) {
        return NULL;
    }
    Node* currNode = *head;
    Node* prevNode = NULL;
    Node* highestPriorityNode = *head;
    Node* prevHighestPriorityNode = NULL;

    while (currNode != NULL) {
        if (currNode->process.priority > highestPriorityNode->process.priority) {
            highestPriorityNode = currNode;
            prevHighestPriorityNode = prevNode;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
    if (prevHighestPriorityNode == NULL) {
        *head = highestPriorityNode->next;
    }
    else {
        prevHighestPriorityNode->next = highestPriorityNode->next;
    }
    return highestPriorityNode;
}

//Function to calculate the average waiting time
float calculateAverageWaitingTime(PCB* processes, int n) {
    float totalWaitingTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime=totalWaitingTime + processes[i].waiting_time;
    }
    return totalWaitingTime / n;
}
//Function to calculate the average response time
float calculateAverageResponseTime(PCB* processes, int n) {
    float totalResponseTime = 0;
    for (int i = 0; i < n; i++) {
        totalResponseTime=totalResponseTime + processes[i].response_time;
    }
    return totalResponseTime / n;
}
//Function to calculate the average turnaround time
float calculateAverageTurnaroundTime(PCB* processes, int n) {
    float totalTurnaroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalTurnaroundTime=totalTurnaroundTime + processes[i].turnaround_time;
    }
    return totalTurnaroundTime / n;
}

//Function to simulate Preemptive Priority Scheduling with Aging
void simulatePriorityScheduling(PCB* processes, int numProcesses, float alpha) {
    Node* readyQueue = NULL;
    int currentTime = 0;
    int completedProcesses = 0;
    
    printf("Scheduling : Preemptive Priority Scheduling with Aging\n");
    printf("==========================================\n");

    while (completedProcesses < numProcesses) {
        //Check for new arrivals and insert them into the ready queue
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrival_time == currentTime) {
                insertNode(&readyQueue, processes[i]);
                printf("<time %d> [new arrival] process %d\n", currentTime, processes[i].pid);
            }
        }
        if (readyQueue == NULL) {
            printf("<time %d> ---- system is idle ----\n", currentTime);
        }
        else {
            Node* runningProcessNode = removeHighestPriorityNode(&readyQueue);
            PCB* runningProcess = &(runningProcessNode->process);
            runningProcess->response_time = currentTime - runningProcess->arrival_time;

            printf("<time %d> process %d is running\n", currentTime, runningProcess->pid);

            runningProcess->burst_time--;

            if (runningProcess->burst_time == 0) {
                completedProcesses++;
                printf("<time %d> process %d is finished\n", currentTime + 1, runningProcess->pid);
                runningProcess->turnaround_time = currentTime + 1 - runningProcess->arrival_time;
            }
            else {
                insertNode(&readyQueue, *runningProcess);
            }
            Node* temp = readyQueue;
            while (temp != NULL) {
                temp->process.waiting_time++;
                temp = temp->next;
            }
        }
        //Aging
        Node* temp = readyQueue;
        while (temp != NULL) {
            temp->process.priority += alpha * temp->process.waiting_time;
            temp = temp->next;
        }
        currentTime++;
    }

    printf("==========================================\n");
    printf("Average CPU usage: %.2f%%\n", ((float)(currentTime - 1) / currentTime) * 100);
    printf("Average waiting time: %.2f\n", calculateAverageWaitingTime(processes, numProcesses));
    printf("Average response time: %.2f\n", calculateAverageResponseTime(processes, numProcesses));
    printf("Average turnaround time: %.2f\n", calculateAverageTurnaroundTime(processes, numProcesses));
}

int main() {
    FILE* inputFile = fopen("input.dat", "r");
    if (inputFile == NULL) {
        printf("Error : Cannot open file.\n");
        return 1;
    }
    PCB processes[MAX];
    int numProcesses = 0;

    while (fscanf(inputFile, "%d %d %d %d", &processes[numProcesses].pid, &processes[numProcesses].priority,&processes[numProcesses].arrival_time, &processes[numProcesses].burst_time) == 4) {
        numProcesses++;
    }
    fclose(inputFile);

    //Simulate Preemptive Priority Scheduling with Aging
    float alpha = 0.2; //my aging parameter

    simulatePriorityScheduling(processes, numProcesses, alpha);

    return 0;
}