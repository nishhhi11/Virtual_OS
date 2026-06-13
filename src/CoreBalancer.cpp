#include "../include/CoreBalancer.h"
#include <iostream>
#include <climits>

using namespace std;


CoreBalancer::CoreBalancer(int cores) : numCores(cores) {
    coreLoad.resize(numCores, 0);
    coreQueues.resize(numCores);
}

int CoreBalancer::assignTask(Task* task) {
    int coreID = getLeastLoadedCore();
    coreQueues[coreID].push(task);
    coreLoad[coreID]++;
    task->setAssignedCore(coreID);
    return coreID;
}

void CoreBalancer::forceAssignTask(Task* task, int coreID) {
    if (coreID >= 0 && coreID < numCores) {
        coreQueues[coreID].push(task);
        coreLoad[coreID]++;
        task->setAssignedCore(coreID);
    }
}

void CoreBalancer::clearQueue(int coreID) {
    if (coreID >= 0 && coreID < numCores) {
        queue<Task*> empty;
        swap(coreQueues[coreID], empty);
    }
}

int CoreBalancer::getLeastLoadedCore() {
    int minLoad = INT_MAX;
    int selectedCore = 0;
    
    for (int i = 0; i < numCores; i++) {
        if (coreLoad[i] < minLoad) {
            minLoad = coreLoad[i];
            selectedCore = i;
        }
    }
    return selectedCore;
}

void CoreBalancer::updateCoreLoad(int coreID, int load) {
    if (coreID >= 0 && coreID < numCores) {
        coreLoad[coreID] = load;
    }
}

void CoreBalancer::balanceLoad() {
    int maxLoad = -1;
    int minLoad = INT_MAX;
    int sourceCore = -1;
    int destCore = -1;
    
    for (int i = 0; i < numCores; i++) {
        if (coreLoad[i] > maxLoad) {
            maxLoad = coreLoad[i];
            sourceCore = i;
        }
        if (coreLoad[i] < minLoad) {
            minLoad = coreLoad[i];
            destCore = i;
        }
    }
    
    if (sourceCore != -1 && destCore != -1 && (maxLoad - minLoad > 1)) {
        cout << "Core " << sourceCore << " Load = " << maxLoad << "\n";
        cout << "Core " << destCore << " Load = " << minLoad << "\n\n";

        if (!coreQueues[sourceCore].empty()) {
            Task* taskToMigrate = coreQueues[sourceCore].front();
            coreQueues[sourceCore].pop();
            
            coreQueues[destCore].push(taskToMigrate);
            coreLoad[sourceCore]--;
            coreLoad[destCore]++;
            taskToMigrate->setAssignedCore(destCore);
            
            cout << "Migrating " << taskToMigrate->getName() << "\n";
            cout << "Core " << sourceCore << " -> Core " << destCore << "\n\n";
            cout << "Load Balanced\n";
        }
    }
}

void CoreBalancer::migrateTask(int sourceCore, int destCore, Task* task) {
    if (sourceCore >= 0 && sourceCore < numCores && destCore >= 0 && destCore < numCores) {
        coreLoad[sourceCore]--;
        coreLoad[destCore]++;
        task->setAssignedCore(destCore);
        coreQueues[destCore].push(task);
    }
}

void CoreBalancer::displayCoreStatus() {
    cout << "\n=== Core Status ===\n";
    for (int i = 0; i < numCores; i++) {
        cout << "Core " << i << ": " << coreLoad[i] << " tasks\n";
    }
}
