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
    CRITICAL = 1,    // System processes
    HIGH = 2,        // Important user processes
    NORMAL = 3,      // Regular applications
    LOW = 4,         // Background tasks
    IDLE = 5         // Optional tasks
};

struct Context {
    int programCounter;     // Current line of code
    vector<int> registers;  // CPU register values
    vector<void*> stack;     // Stack pointer values
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
    vector<int> dependencies;  // IDs of tasks this task depends on
    int assignedCore;
    time_t creationTime;
    size_t cpuTimeUsed;

public:
    Task(int id, const string& name, const string& secID);
    
    // Getters
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
    
    // Setters
    void setState(TaskState newState) { state = newState; }
    void setPriority(Priority newPriority) { priority = newPriority; }
    void setMemoryUsage(size_t mem) { 
        memoryUsage = mem; 
        // Simulate memory allocation for demo
        if (mem > 0) {
            // In real OS, this would allocate actual memory
        }
    }
    void setAssignedCore(int core) { assignedCore = core; }
    void addDependency(int depID);
    void removeDependency(int depID);
    
    // Context management
    void saveContext(int pc, const vector<int>& regs);
    void restoreContext(int& pc, vector<int>& regs);
    
    // CPU time tracking
    void addCPUTime(size_t time) { cpuTimeUsed += time; }
    
    // Comparison operators for priority queue
    bool operator<(const Task& other) const {
        return priority > other.priority;  // Higher priority first
    }
    
    void display() const;
};

#endif
