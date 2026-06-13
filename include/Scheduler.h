#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Task.h"
#include <queue>
#include <stack>
#include <vector>

using namespace std;


struct ComparePriority {
    bool operator()(Task* a, Task* b) {
        return a->getPriority() > b->getPriority();
    }
};

class Scheduler {
private:
    priority_queue<Task*, vector<Task*>, ComparePriority> priorityQueue;
    queue<Task*> fifoQueue;
    
public:
    void addToProcessingLine(Task* task);
    Task* getNextFromProcessingLine();
    
    void addToPriorityQueue(Task* task);
    Task* getHighestPriorityTask();
    
    void displayQueues();
    
    int getPriorityQueueSize() const { return priorityQueue.size(); }
    int getProcessingLineSize() const { return fifoQueue.size(); }
};

#endif
