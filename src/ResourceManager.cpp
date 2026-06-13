#include "../include/ResourceManager.h"
#include <iostream>
#include <algorithm>

using namespace std;


bool ResourceManager::requestResource(Task* task, const string& resourceName) {
    auto it = resources.find(resourceName);
    if (it == resources.end()) {
        resources[resourceName] = Resource(resourceName);
        it = resources.find(resourceName);
    }
    
    if (it->second.ownerTaskID == -1) {
        it->second.ownerTaskID = task->getID();
        cout << "  [Resource] Task " << task->getName() << " acquired " << resourceName << "\n";
        return true;
    } else if (it->second.ownerTaskID == task->getID()) {
        return true;
    } else {
        if (find(it->second.waitingTasks.begin(), it->second.waitingTasks.end(), task->getID()) == it->second.waitingTasks.end()) {
            it->second.waitingTasks.push_back(task->getID());
            // Add wait edge for the graph
            waitGraph[task->getID()].push_back(it->second.ownerTaskID);
        }
        task->setState(TaskState::WAITING);
        cout << "  [Resource] Task " << task->getName() << " waiting for " << resourceName << " (owned by ID " << it->second.ownerTaskID << ")\n";
        return false;
    }
}

bool ResourceManager::releaseResource(Task* task, const string& resourceName) {
    auto it = resources.find(resourceName);
    if (it == resources.end()) return false;
    
    if (it->second.ownerTaskID == task->getID()) {
        if (!it->second.waitingTasks.empty()) {
            int nextTaskID = it->second.waitingTasks.front();
            it->second.waitingTasks.erase(it->second.waitingTasks.begin());
            it->second.ownerTaskID = nextTaskID;
            
            // Remove the edge for the task that just acquired it
            auto& edges = waitGraph[nextTaskID];
            edges.erase(remove(edges.begin(), edges.end(), task->getID()), edges.end());
            
            // Update all remaining waiting tasks to point to the new owner
            for (int waitTaskID : it->second.waitingTasks) {
                auto& waitEdges = waitGraph[waitTaskID];
                replace(waitEdges.begin(), waitEdges.end(), task->getID(), nextTaskID);
            }
            
            cout << "  [Resource] " << resourceName << " transferred to waiting task ID " << nextTaskID << "\n";
        } else {
            it->second.ownerTaskID = -1;
            cout << "  [Resource] " << resourceName << " is now fully released.\n";
        }
        return true;
    }
    return false;
}

bool ResourceManager::detectCycleDFS(int currentTask, set<int>& visited, set<int>& recursionStack) {
    visited.insert(currentTask);
    recursionStack.insert(currentTask);
    
    for (int neighbor : waitGraph[currentTask]) {
        if (visited.find(neighbor) == visited.end()) {
            if (detectCycleDFS(neighbor, visited, recursionStack)) {
                return true;
            }
        } else if (recursionStack.find(neighbor) != recursionStack.end()) {
            // Cycle detected!
            return true;
        }
    }
    
    recursionStack.erase(currentTask);
    return false;
}

bool ResourceManager::detectDeadlock() {
    set<int> visited;
    set<int> recursionStack;
    
    for (const auto& node : waitGraph) {
        if (visited.find(node.first) == visited.end()) {
            if (detectCycleDFS(node.first, visited, recursionStack)) {
                cout << "  [WARNING] DEADLOCK DETECTED via Cycle in Wait-For Graph!\n";
                return true;
            }
        }
    }
    
    return false;
}

void ResourceManager::resolveDeadlock() {
    cout << "  [Resolver] Attempting to break deadlock by releasing locked resources...\n";
    for (auto& pair : resources) {
        if (!pair.second.waitingTasks.empty() && pair.second.ownerTaskID != -1) {
            cout << "  [Resolver] Breaking wait queue on resource: " << pair.first << "\n";
            int owner = pair.second.ownerTaskID;
            pair.second.waitingTasks.clear();
            pair.second.ownerTaskID = -1;
            
            // Clean up the wait graph for all waiting tasks on this resource
            for (auto& graphPair : waitGraph) {
                auto& edges = graphPair.second;
                edges.erase(remove(edges.begin(), edges.end(), owner), edges.end());
            }
            return; // Break one lock at a time
        }
    }
}

void ResourceManager::displayResources() {
    cout << "\n══════════════ RESOURCE MANAGER ══════════════\n";
    if (resources.empty()) {
        cout << "No resources actively requested.\n";
        return;
    }
    for (auto& pair : resources) {
        cout << "Resource: " << pair.first 
                  << "\n  Owner Task ID: " << (pair.second.ownerTaskID == -1 ? "None" : to_string(pair.second.ownerTaskID))
                  << "\n  Waiting Tasks: " << pair.second.waitingTasks.size() << "\n";
    }
}
