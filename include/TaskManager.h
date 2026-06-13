#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;


class TaskManager {
private:
    unordered_map<int, Task*> taskDirectory;  // O(1) lookups
    int nextTaskID;
    
public:
    TaskManager();
    ~TaskManager();
    
    // Task Directory operations
    bool addTask(const string& name, const string& securityID);
    bool removeTask(int taskID);
    Task* findTask(int taskID);
    vector<Task*> getAllTasks();
    void displayAllTasks();
    
    int getNextTaskID() const { return nextTaskID; }
    size_t getTaskCount() const { return taskDirectory.size(); }
};

#endif
