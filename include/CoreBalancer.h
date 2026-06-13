#ifndef COREBALANCER_H
#define COREBALANCER_H

#include "Task.h"
#include <vector>
#include <queue>

using namespace std;


class CoreBalancer {
private:
    int numCores;
    vector<int> coreLoad;  
    vector<queue<Task*>> coreQueues;
    
public:
    CoreBalancer(int cores = 4);
    
    int assignTask(Task* task);
    void forceAssignTask(Task* task, int coreID);
    void clearQueue(int coreID);
    void updateCoreLoad(int coreID, int load);
    int getLeastLoadedCore();
    void balanceLoad();
    void migrateTask(int sourceCore, int destCore, Task* task);
    void displayCoreStatus();
    
    int getNumCores() const { return numCores; }
};

#endif
