#include "../include/TaskManager.h"
#include <iostream>

using namespace std;


TaskManager::TaskManager() : nextTaskID(1) {}

TaskManager::~TaskManager() {
    for (auto& pair : taskDirectory) {
        delete pair.second;
    }
    taskDirectory.clear();
}

bool TaskManager::addTask(const string& name, const string& securityID) {
    Task* newTask = new Task(nextTaskID, name, securityID);
    taskDirectory[nextTaskID] = newTask;
    nextTaskID++;
    return true;
}

bool TaskManager::removeTask(int taskID) {
    auto it = taskDirectory.find(taskID);
    if (it != taskDirectory.end()) {
        delete it->second;
        taskDirectory.erase(it);
        return true;
    }
    return false;
}

Task* TaskManager::findTask(int taskID) {
    auto it = taskDirectory.find(taskID);
    return (it != taskDirectory.end()) ? it->second : nullptr;
}

vector<Task*> TaskManager::getAllTasks() {
    vector<Task*> tasks;
    for (auto& pair : taskDirectory) {
        tasks.push_back(pair.second);
    }
    return tasks;
}

void TaskManager::displayAllTasks() {
    cout << "\n=== Task Directory ===\n";
    for (auto& pair : taskDirectory) {
        pair.second->display();
        cout << "------------------------\n";
    }
}
