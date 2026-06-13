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
    cout << "Task " << task->getName() << " assigned to Core " << coreID 
              << " (Load: " << coreLoad[coreID] << ")\n";
    return coreID;
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
    cout << "\n=== Load Balancing ===\n";
    
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
        if (!coreQueues[sourceCore].empty()) {
            Task* taskToMigrate = coreQueues[sourceCore].front();
            coreQueues[sourceCore].pop();
            
            coreQueues[destCore].push(taskToMigrate);
            coreLoad[sourceCore]--;
            coreLoad[destCore]++;
            taskToMigrate->setAssignedCore(destCore);
            
            cout << "  [Migration] Moved " << taskToMigrate->getName() 
                 << " from Core " << sourceCore << " to Core " << destCore << "\n";
        }
    } else {
        cout << "  Cores are balanced. No migration needed.\n";
    }
}

void CoreBalancer::displayCoreStatus() {
    cout << "\n=== Core Status ===\n";
    for (int i = 0; i < numCores; i++) {
        cout << "Core " << i << ": " << coreLoad[i] << " tasks\n";
    }
}
