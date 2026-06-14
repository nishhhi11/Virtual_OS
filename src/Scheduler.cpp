#include "../include/Scheduler.h"
#include <iostream>

using namespace std;


void Scheduler::saveContextToStack(const Context& ctx) {
    contextStack.push(ctx);
}

Context Scheduler::restoreContextFromStack() {
    if (contextStack.empty()) return Context();
    Context ctx = contextStack.top();
    contextStack.pop();
    return ctx;
}

bool Scheduler::hasSavedContexts() const {
    return !contextStack.empty();
}

void Scheduler::addToProcessingLine(Task* task) {
    fifoQueue.push(task);
}

Task* Scheduler::getNextFromProcessingLine() {
    if (fifoQueue.empty()) return nullptr;
    Task* task = fifoQueue.front();
    fifoQueue.pop();
    return task;
}


void Scheduler::addToPriorityQueue(Task* task) {
    priorityQueue.push(task);
}

Task* Scheduler::getHighestPriorityTask() {
    if (priorityQueue.empty()) return nullptr;
    Task* task = priorityQueue.top();
    priorityQueue.pop();
    return task;
}


void Scheduler::displayQueues() {
    cout << "\n=== Scheduler Status ===\n";
    cout << "Processing Line size: " << fifoQueue.size() << "\n";
    cout << "Priority Queue size: " << priorityQueue.size() << "\n";
}
