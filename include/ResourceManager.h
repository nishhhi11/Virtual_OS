#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Task.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <set>

using namespace std;


struct Resource {
    string name;
    int ownerTaskID;
    vector<int> waitingTasks;
    
    Resource(const string& n) : name(n), ownerTaskID(-1) {}
    Resource() : name(""), ownerTaskID(-1) {}
};

class ResourceManager {
private:
    unordered_map<string, Resource> resources;
    unordered_map<int, vector<int>> waitGraph;
    
    
    bool detectCycleDFS(int currentTask, set<int>& visited, set<int>& recursionStack);
    
public:
    bool requestResource(Task* task, const string& resourceName);
    bool releaseResource(Task* task, const string& resourceName);
    bool detectDeadlock();
    void resolveDeadlock();
    void displayResources();
    
    
    unordered_map<string, Resource>& getResources() { return resources; }
    
    
    const unordered_map<int, vector<int>>& getWaitGraph() const { return waitGraph; }
};

#endif
