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
    stack<Context> contextStack;
    
public:
    void saveContextToStack(const Context& ctx);
    Context restoreContextFromStack();
    bool hasSavedContexts() const;
    void addToProcessingLine(Task* task);
    Task* getNextFromProcessingLine();
    
    void addToPriorityQueue(Task* task);
    Task* getHighestPriorityTask();
    
    void displayQueues();
    
    int getPriorityQueueSize() const { return priorityQueue.size(); }
    int getProcessingLineSize() const { return fifoQueue.size(); }
    
    vector<Task*> getPriorityQueueTasks() const {
        vector<Task*> tasks;
        auto pq_copy = priorityQueue;
        while (!pq_copy.empty()) {
            tasks.push_back(pq_copy.top());
            pq_copy.pop();
        }
        return tasks;
    }
    
    vector<Task*> getProcessingLineTasks() const {
        vector<Task*> tasks;
        auto q_copy = fifoQueue;
        while (!q_copy.empty()) {
            tasks.push_back(q_copy.front());
            q_copy.pop();
        }
        return tasks;
    }
};

#endif
