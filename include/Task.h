#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>
#include <ctime>

using namespace std;


enum class TaskState {
    RUNNING,
    PAUSED,
    WAITING,
    TERMINATED
};

enum class Priority {
    CRITICAL = 1,    
    HIGH = 2,        
    NORMAL = 3,      
    LOW = 4,         
    IDLE = 5         
};

struct Context {
    int programCounter;     
    vector<int> registers;  
    vector<void*> stack;     
    time_t lastSavedTime;
    
    Context() : programCounter(0), lastSavedTime(0) {}
};

class Task {
private:
    int taskID;
    string name;
    size_t memoryUsage;
    TaskState state;
    Priority priority;
    Context savedContext;
    string securityID;
    vector<int> dependencies;  
    int assignedCore;
    time_t creationTime;
    size_t cpuTimeUsed;

public:
    Task(int id, const string& name, const string& secID);
    
    
    int getID() const { return taskID; }
    string getName() const { return name; }
    size_t getMemoryUsage() const { return memoryUsage; }
    TaskState getState() const { return state; }
    Priority getPriority() const { return priority; }
    Context getContext() const { return savedContext; }
    string getSecurityID() const { return securityID; }
    vector<int> getDependencies() const { return dependencies; }
    int getAssignedCore() const { return assignedCore; }
    size_t getCPUTimeUsed() const { return cpuTimeUsed; }
    
    
    void setState(TaskState newState) { state = newState; }
    void setPriority(Priority newPriority) { priority = newPriority; }
    void setMemoryUsage(size_t mem) { 
        memoryUsage = mem; 
        
        if (mem > 0) {
            
        }
    }
    void setAssignedCore(int core) { assignedCore = core; }
    void addDependency(int depID);
    void removeDependency(int depID);
    
    
    void saveContext(int pc, const vector<int>& regs);
    void restoreContext(int& pc, vector<int>& regs);
    
    
    void addCPUTime(size_t time) { cpuTimeUsed += time; }
    
    
    bool operator<(const Task& other) const {
        return priority > other.priority;  
    }
    
    void display() const;
};

#endif
