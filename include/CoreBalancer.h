#ifndef COREBALANCER_H
#define COREBALANCER_H

#include "Task.h"
#include <vector>
#include <queue>

using namespace std;


class CoreBalancer {
private:
    int numCores;
    vector<int> coreLoad;  // Load per core (number of tasks)
    vector<queue<Task*>> coreQueues;
    
public:
    CoreBalancer(int cores = 4);
    
    int assignTask(Task* task);
    void updateCoreLoad(int coreID, int load);
    int getLeastLoadedCore();
    void balanceLoad();
    void displayCoreStatus();
    
    int getNumCores() const { return numCores; }
};

#endif
