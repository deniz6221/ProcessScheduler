#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Insturction typedef to store instruction information.
typedef struct Instruction{
    char name[256];
    int duration;
}Instruction;

//Process typedef to store instruction information.
typedef struct Process{
    char name[256];
    int priority;
    int type;
    int initArrival;
    int arrival;
    int arrived;
    int quantumCompleted;
    Instruction instructions[256];
    int instructionCount;
    int completedInstructions;
    int burstTime;
    int prChangeTime;
    int completionTime;
    int queueTurn;
}Process;

/*This function compares two processes to determine which one should execute
priorly. It checks their type, priority, queue turn, arrival and alphabetical order.*/
int compareProcesses(Process* x, Process* y){
    if(x->type == 2 || y->type == 2){
        if(x->type == 2 && y->type == 2){
            if(x->priority == y->priority){
                if(x->arrival == y->arrival){
                    if(strcmp(x->name, y->name) < 0 ){
                        return 0;
                    }
                    return 1;
                }
                else{
                    return x->arrival > y->arrival;
                }
            }
            else{
                return x->priority < y->priority;
            }
        }
        else{
            return x->type < y->type;
        }
    }
    else{
        if(x->priority == y->priority){
            if(x->queueTurn != y-> queueTurn){
                return x->queueTurn < y->queueTurn;
            }
            if(x->arrival == y->arrival){
                    if(strcmp(x->name, y->name) < 0 ){
                        return 0;
                    }
                    return 1;
                }
                else{
                    return x->arrival > y->arrival;
                }
        }
        else{
            return x->priority < y->priority;
        }
    }
}
/*This function finds the process to be executed in a process array. It
uses the compareProcesses function to compare each pair of process.*/
int findProcess(Process** queue, int queueCount){
    if(queueCount == 0){
        return -1;
    }
    Process* ret = queue[0];
    for (int i = 1; i< queueCount; i++){
        if(compareProcesses(ret, queue[i]) == 1){
            ret = queue[i];
        }
    }
    for(int j = 0; j <queueCount; j++){
        if(strcmp(ret->name, queue[j]->name) == 0){
            return j;
        }
    }
    return -1;
}
/*This function returns 1 if time quantum has passed 0 if not.*/
int calculateRR(Process* proc, int burst, int prevBurst){
    if(proc->type ==2){
        return 0;
    }
    if(proc->type == 1 && burst >= 120){
        return 1;
    }
    if(proc->type == 0 && burst >= 80){
        return 1;
    }
    return 0;
}

//This function checks if the priority number of a process needs to be increased or not.
int calculatePR(Process* proc, int burst, int prevBurst){
    if(proc->type ==2){
        return 0;
    }
    for(int i = 1; i<6; i++){
        if(proc->type == 1 && burst >= 120*i && prevBurst<120*i){
        return 1;
    }
    }
    
    for(int i = 1; i<4; i++){
        if(proc->type == 0 && burst >= 80*i && prevBurst<80*i){
        return 1;
    }
    }
    
    
    return 0;
}

/*This funtion checks if a process should become a GOLD/PLAT process due to
time quantums. If it is required, then it changes its type.*/
void changePriority(Process* proc){
    if(proc->type == 1 && proc->prChangeTime >= 5){
        proc->prChangeTime = 0;
        proc ->type = 2;
    }
    else if(proc->type == 0 && proc->prChangeTime >= 3){
        proc->prChangeTime = 0;
        proc ->type = 1;
    }
}

/*This function was used for debugging purposes.*/
void printStringArray(char** ary){
    int i = 0;
    while(ary[i] != NULL){
        printf("%s\n", ary[i]);
        i++;
    }
}

/*This function frees a Process pointer array. It is used in each clock cycle.*/
void freePointerArray(Process** ary, int count){
    for(int i=0; i<count; i++){
        ary[i] = NULL;
    }
}

/*Main function reads all necessary files and stores the given information
in data structures. It also calls the previously defined functions to operate.*/
int main(int argc, char** argv){
    char instFileLine[256];
    FILE* instFile = fopen("instructions.txt", "r");
    Instruction instructionInfo[256];
    int instructionCount;
    while(fgets(instFileLine, 256, instFile) != NULL){
        char* newLineI = strchr(instFileLine, '\n');
        if(newLineI != NULL){
            *newLineI = '\0';
        }
        char* arg = strtok(instFileLine, " ");
        strcpy(instructionInfo[instructionCount].name, arg);
        arg = strtok(NULL, " ");
        instructionInfo[instructionCount].duration = atoi(arg);
        instructionCount++;
    }
    fclose(instFile);


    FILE* def = fopen("definition.txt", "r");
    char line[256];
    Process processes[10];

    

    int processCount = 0;
    while(fgets(line, 256, def) != NULL){
        char* newLine = strchr(line, '\n');
        if(newLine != NULL){
            *newLine = '\0';
        }
        char* info = strtok(line, " ");
        strcpy(processes[processCount].name,info);
        char* procF = (char*)malloc(256*sizeof(char));
        sprintf(procF, "%s.txt", info);
        FILE* procFile = fopen(procF, "r");
        free(procF);
        char* instLine = (char*)malloc(256*sizeof(char));
        int instCount= 0;
        while(fgets(instLine, 256, procFile) != NULL){
            if(strcmp(instLine, "exit") == 0 || strcmp(instLine, "exit\n") == 0){
                break;
            }
            char* instNewL = strchr(instLine, '\n');
            if(instNewL != NULL){
                *instNewL = '\0';
            }
            strcpy(processes[processCount].instructions[instCount].name,instLine);
            for(int i = 0; i<instructionCount; i++){
                if(strcmp(instructionInfo[i].name,instLine) == 0){
                    processes[processCount].instructions[instCount].duration = instructionInfo[i].duration;
                }
            }
            instCount++;
        }
        processes[processCount].instructionCount = instCount;
        
        info = strtok(NULL, " ");
        processes[processCount].priority = atoi(info);
        info = strtok(NULL, " ");
        processes[processCount].arrival = atoi(info);
        processes[processCount].initArrival = atoi(info);
        info = strtok(NULL, " ");
        if(strcmp("SILVER", info) == 0){
            processes[processCount].type = 0;
        }
        else if(strcmp("GOLD", info) == 0){
            processes[processCount].type = 1;
        }
        else{
            processes[processCount].type = 2;
        }
        processes[processCount].quantumCompleted = 0;
        processes[processCount].burstTime = 0;
        processes[processCount].prChangeTime = 0;
        processes[processCount].arrived = 0;
        processes[processCount].completedInstructions = 0;
        processes[processCount].queueTurn= 0;
        processes[processCount].completionTime = -1;
        processCount++;
        fclose(procFile);
    }
    fclose(def);

    int time = 0;
    int prevBurst = 0;
    int completedProcesses = 0;
    int queueCount = 0;
    Process* current = NULL;
    int currentBurst = 0;
    int queueTurn = 0;
    int RRHappened = 0;
    int PRHappened = 0;
    Process* lastProccess;

    int debug = 0;
    while(completedProcesses != processCount){ //Loop until all processes complete.
    //  if(++debug >= 10){
    //     printf("10\n");
    //  }  
    //    if(debug == 19){
    //        printf("");
    //    }
//   if(debug % 3 == 0){
//        printf("\n");
//        }
        int newArrived = 0;
        int newArrivals[10];
        int arrivalTimes[10];
        Process* readyQ[10];
        for(int i = 0; i< processCount; i++){
            if(processes[i].arrival <= time && processes[i].completionTime== -1){
                  readyQ[queueCount] = &processes[i];
                  if(!processes[i].arrived){
                    processes[i].arrived = 1;
                    newArrivals[newArrived] = queueCount;
                    arrivalTimes[newArrived] = processes[i].arrival;
                    newArrived++;
                  }
                  queueCount++;
            }
        }
        for(int i = 1; i<newArrived; i++){
            int j = i-1;
            int key = arrivalTimes[i];
            int otherKey = newArrivals[i];
            while(j >= 0 && arrivalTimes[j] > key){
                arrivalTimes[j+1] = arrivalTimes[j];
                newArrivals[j+1] = newArrivals[j];
                j = j-1;
            }
            arrivalTimes[j+1] = key;
            newArrivals[j+1] = otherKey;
        }
        for(int i=0; i<newArrived; i++){
            int sameOrder = 0;
            for(int k = i+1; k < newArrived; k++){
                if(arrivalTimes[i] != arrivalTimes[k]){
                    break;
                }
                sameOrder++;
            }
            queueTurn--;
            for(int m = 0; m <=sameOrder; m++){
                readyQ[newArrivals[i+m]]->queueTurn = queueTurn;
            }
            i += sameOrder;
            
        }
        int index;
        if(lastProccess != NULL){
            if(lastProccess->type == 2){
                for(int i=0; i<queueCount; i++){
                    if(strcmp(readyQ[i]->name, lastProccess->name) == 0){
                        index = i;
                        break;
                    }
                }
            }
            else{
                index = findProcess(readyQ, queueCount);
            }
        }
        else{
            index = findProcess(readyQ, queueCount);
        }

        if(index == -1){
            time++;
            freePointerArray(readyQ, queueCount);
            queueCount= 0;
            continue;
        }
        if(current == NULL){
            time += 10;
        }
        
        current = readyQ[index];
    //     if(current != NULL){
    //     if(strcmp(current->name, "P6") == 0){
    //         printf("here\n");
    //     }
    //    }
        if(lastProccess != NULL){
            if(strcmp(current->name, lastProccess->name) != 0){
                if(!RRHappened){
                    queueTurn--;
                    lastProccess->queueTurn = queueTurn;
                }
                if(!PRHappened){
                    lastProccess->prChangeTime++;
                }
                changePriority(lastProccess);
                if(lastProccess->type == 2){
                    if(current->type == 2){
                        currentBurst = 0;
                        time += 10;
                    }
                    else{
                        switch (compareProcesses(lastProccess, current))
                        {
                        case 0:
                            current = lastProccess;
                            break;
                        
                        case 1:
                            currentBurst = 0;
                            time += 10;
                            break;
                        default:
                            break;
                        }
                    }
                }
                else{   
                    currentBurst = 0;
                    time += 10;
                }
            }
        }
        
        RRHappened = 0;
        PRHappened = 0;
        
        if(current->completedInstructions == current->instructionCount){
            queueCount--;
            completedProcesses++;
            time += 10;
            current-> completionTime = time;
            currentBurst = 0;
            current->burstTime += 10;
            current = NULL;
            freePointerArray(readyQ, queueCount);
            queueCount =0;
            lastProccess = NULL;
            continue;
        }

        
        int burst = current->instructions[current->completedInstructions].duration;
        prevBurst = currentBurst;
        time += burst;
        currentBurst += burst;
        current->completedInstructions += 1;
        current->burstTime  += burst;
        // current->prChangeTime += burst;


        
        if(calculateRR(current, currentBurst, prevBurst)){
            RRHappened = 1;
            current->arrived = 0;
            current->arrival = time;

            if(calculatePR(current, currentBurst, prevBurst)){
                current->prChangeTime++;
                PRHappened = 1;
            }
            
            // queueTurn--;
            // current->queueTurn = queueTurn;
        }
        lastProccess = current;

        freePointerArray(readyQ, queueCount);
        queueCount = 0;
        


    }

    float avgTurn = 0;
    float avgWait = 0;
    for(int i =0; i< processCount; i++){
        int turn = processes[i].completionTime-processes[i].initArrival;
        avgTurn += turn;
        avgWait += turn-processes[i].burstTime;
    }
    avgTurn = avgTurn/processCount;
    avgWait = avgWait/processCount;

    if(avgWait- (int)avgWait> 0){
        printf("%.5f\n", avgWait);
    }
    else{
        printf("%.0f\n", avgWait);
    }

    if(avgTurn- (int)avgTurn> 0){
        printf("%.5f\n", avgTurn);
    }
    else{
        printf("%.0f\n", avgTurn);
    }
    
    return 0;
}

