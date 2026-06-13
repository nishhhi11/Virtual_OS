#include "../include/TaskManager.h"
#include "../include/Scheduler.h"
#include "../include/ResourceManager.h"
#include "../include/CoreBalancer.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <cctype>
#include <stdexcept>

using namespace std;

class VirtualOS {
private:
    TaskManager taskManager;
    Scheduler scheduler;
    ResourceManager resourceManager;
    CoreBalancer coreBalancer;
    vector<string> approvedPrograms;
    bool running;
    int timeSlice;
    
public:
    VirtualOS(int cores = 4, int timeSliceMs = 100) 
        : coreBalancer(cores), running(true), timeSlice(timeSliceMs) {
        
        approvedPrograms = {
            "system.exe", "memory.exe", "browser.exe", "music.exe", 
            "antivirus.exe", "game.exe", "chrome.exe", "spotify.exe", 
            "vscode.exe", "notepad.exe", "calculator.exe", "audio.exe"
        };
        sort(approvedPrograms.begin(), approvedPrograms.end());
        
        startProgram("system.exe", "SYS12345", Priority::CRITICAL);
        startProgram("memory.exe", "MEM12345", Priority::CRITICAL);
    }
    
    bool securityCheck(const string& name, const string& id) {
        bool approved = binary_search(approvedPrograms.begin(), approvedPrograms.end(), name);
        bool valid = (id.length() >= 6 && id.length() <= 12);
        if (approved && valid) {
            cout << "✅ Security passed: " << name << "\n";
            return true;
        }
        cout << "❌ Security failed: " << name << "\n";
        return false;
    }
    
    bool startProgram(const string& name, const string& id, Priority priority = Priority::NORMAL) {
        if (!securityCheck(name, id)) return false;
        
        int tid = taskManager.getNextTaskID();
        taskManager.addTask(name, id);
        Task* task = taskManager.findTask(tid);
        if (!task) return false;
        
        task->setPriority(priority);
        task->setState(TaskState::RUNNING);
        task->setMemoryUsage(50 + rand() % 200);
        
        if (priority == Priority::CRITICAL || priority == Priority::HIGH) {
            scheduler.addToPriorityQueue(task);
            cout << "  → PRIORITY QUEUE (size: " << scheduler.getPriorityQueueSize() << ")\n";
        } else {
            scheduler.addToProcessingLine(task);
            cout << "  → PROCESSING LINE (size: " << scheduler.getProcessingLineSize() << ")\n";
        }
        
        int core = coreBalancer.assignTask(task);
        task->setAssignedCore(core);
        
        cout << "✅ Started: " << name << " (ID:" << tid << ", Core:" << core 
             << ", Mem:" << task->getMemoryUsage() << "MB)\n";
        return true;
    }
    
    bool terminateProgram(int taskID) {
        Task* taskToKill = taskManager.findTask(taskID);
        if (!taskToKill) {
            cout << "❌ Task ID " << taskID << " not found\n";
            return false;
        }
        
        // CASE STUDY: Dependency Grid Safe-Shutdown
        auto tasks = taskManager.getAllTasks();
        for (Task* t : tasks) {
            const auto& deps = t->getDependencies();
            if (find(deps.begin(), deps.end(), taskID) != deps.end()) {
                cout << "❌ Cannot terminate: Task " << t->getName() << " (ID: " << t->getID() 
                     << ") depends on " << taskToKill->getName() << "!\n";
                return false;
            }
        }
        
        // Clean up resources if owned
        auto& resources = resourceManager.getResources();
        for (auto& pair : resources) {
            if (pair.second.ownerTaskID == taskID) {
                resourceManager.releaseResource(taskToKill, pair.first);
            }
        }
        
        string name = taskToKill->getName();
        taskManager.removeTask(taskID);
        cout << "✅ Program terminated: " << name << "\n";
        return true;
    }
    
    void pauseProgram(int taskID) {
        Task* task = taskManager.findTask(taskID);
        if (task && task->getState() == TaskState::RUNNING) {
            vector<int> regs = {0, 1, 2, 3}; // Dummy registers
            task->saveContext(100, regs);
            task->setState(TaskState::PAUSED);
            cout << "⏸️ Paused: " << task->getName() << "\n";
        } else {
            cout << "❌ Cannot pause. Task not found or not running.\n";
        }
    }
    
    void resumeProgram(int taskID) {
        Task* task = taskManager.findTask(taskID);
        if (task && task->getState() == TaskState::PAUSED) {
            int pc;
            vector<int> regs;
            task->restoreContext(pc, regs);
            task->setState(TaskState::RUNNING);
            if (task->getPriority() == Priority::CRITICAL || task->getPriority() == Priority::HIGH) {
                scheduler.addToPriorityQueue(task);
            } else {
                scheduler.addToProcessingLine(task);
            }
            cout << "▶️ Resumed: " << task->getName() << "\n";
        } else {
            cout << "❌ Cannot resume. Task not paused or no saved context.\n";
        }
    }
    
    void addDependency(int taskID, int dependsOnID) {
        Task* task = taskManager.findTask(taskID);
        Task* dependsOn = taskManager.findTask(dependsOnID);
        if (task && dependsOn) {
            task->addDependency(dependsOnID);
            cout << "🔗 Dependency added: " << task->getName() << " depends on " << dependsOn->getName() << "\n";
        } else {
            cout << "❌ Invalid task ID(s) for dependency\n";
        }
    }
    
    void requestResource(int taskID, const string& resourceName) {
        Task* task = taskManager.findTask(taskID);
        if (task) {
            resourceManager.requestResource(task, resourceName);
            if (resourceManager.detectDeadlock()) {
                resourceManager.resolveDeadlock();
            }
        } else {
            cout << "❌ Task ID " << taskID << " not found\n";
        }
    }
    
    void releaseResource(int taskID, const string& resourceName) {
        Task* task = taskManager.findTask(taskID);
        if (task) {
            if (!resourceManager.releaseResource(task, resourceName)) {
                cout << "❌ Failed to release resource\n";
            }
        } else {
            cout << "❌ Task ID " << taskID << " not found\n";
        }
    }
    
    void runScheduler(int cycles = 1) {
        cout << "\n══════════════ SCHEDULER RUNNING ══════════════\n";
        
        for (int c = 0; c < cycles; c++) {
            cout << "\n--- CYCLE " << (c+1) << " ---\n";
            bool ran = false;
            
            // Run priority queue
            while (scheduler.getPriorityQueueSize() > 0) {
                Task* t = scheduler.getHighestPriorityTask();
                if (t && t->getState() == TaskState::RUNNING) {
                    cout << "  ⚡ HIGH: " << t->getName() << "\n";
                    t->addCPUTime(timeSlice);
                    ran = true;
                }
            }
            
            // Run normal queue
            while (scheduler.getProcessingLineSize() > 0) {
                Task* t = scheduler.getNextFromProcessingLine();
                if (t && t->getState() == TaskState::RUNNING) {
                    cout << "  ➤ NORMAL: " << t->getName() << "\n";
                    t->addCPUTime(timeSlice);
                    ran = true;
                }
            }
            
            if (!ran) {
                cout << "  ⚠️ NO TASKS! PriorityQ:" << scheduler.getPriorityQueueSize() 
                     << " ProcessQ:" << scheduler.getProcessingLineSize() << "\n";
            }
            
            // Core load balancing occurs implicitly in assignTask, but we simulate tick here
            coreBalancer.balanceLoad();
        }
    }
    
    void listTasks() {
        cout << "\n══════════════ TASK DIRECTORY ══════════════\n";
        auto tasks = taskManager.getAllTasks();
        cout << left << setw(5) << "ID" << setw(15) << "Name" << setw(10) << "State" 
             << setw(10) << "Priority" << setw(8) << "Core" << "CPU(ms)\n";
        cout << string(55, '-') << "\n";
        
        for (auto t : tasks) {
            string stateStr = "WAITING";
            if (t->getState() == TaskState::RUNNING) stateStr = "RUNNING";
            else if (t->getState() == TaskState::PAUSED) stateStr = "PAUSED";
            
            cout << left << setw(5) << t->getID()
                 << setw(15) << t->getName()
                 << setw(10) << stateStr
                 << setw(10) << (t->getPriority() == Priority::HIGH ? "HIGH" : 
                                t->getPriority() == Priority::CRITICAL ? "CRITICAL" : "NORMAL")
                 << setw(8) << t->getAssignedCore()
                 << t->getCPUTimeUsed() << "\n";
        }
    }
    
    void showQueues() {
        cout << "\n══════════════ QUEUE STATUS ══════════════\n";
        cout << "Priority Queue (High Priority): " << scheduler.getPriorityQueueSize() << " tasks\n";
        cout << "Processing Line (Normal): " << scheduler.getProcessingLineSize() << " tasks\n";
    }
    
    void showCores() { coreBalancer.displayCoreStatus(); }
    
    void showStats() {
        auto tasks = taskManager.getAllTasks();
        size_t totalMem = 0, totalCPU = 0;
        for (auto t : tasks) {
            totalMem += t->getMemoryUsage();
            totalCPU += t->getCPUTimeUsed();
        }
        cout << "\n══════════════ STATISTICS ══════════════\n";
        cout << "Total Tasks: " << tasks.size() << "\n";
        cout << "Total Memory: " << totalMem << " MB\n";
        cout << "Total CPU Time: " << totalCPU << " ms\n";
        cout << "Priority Queue: " << scheduler.getPriorityQueueSize() << "\n";
        cout << "Process Line: " << scheduler.getProcessingLineSize() << "\n";
    }
    
    void demo() {
        cout << "\n══════════════ DEMO MODE ══════════════\n";
        startProgram("browser.exe", "BRO123", Priority::HIGH);
        startProgram("music.exe", "MUS456", Priority::NORMAL);
        startProgram("audio.exe", "AUD789", Priority::CRITICAL);
        startProgram("game.exe", "GAM789", Priority::LOW);
        
        // Establish dependency
        addDependency(4, 5); // music depends on audio
        
        // Trigger deadlock test
        requestResource(4, "SoundCard");
        requestResource(5, "SoundCard");
        
        cout << "\n--- RUNNING SCHEDULER (3 cycles) ---\n";
        runScheduler(3);
        
        listTasks();
        showCores();
    }
    
    void interactiveMode() {
        cout << "\n╔══════════════════════════════════════════════════╗\n";
        cout << "║     VIRTUAL OS TASK SUPERVISOR v2.0              ║\n";
        cout << "╚══════════════════════════════════════════════════╝\n";
        cout << "\nCommands: start, kill, pause, resume, deps, req, rel, demo, list, schedule N, queues, cores, stats, exit\n";
        
        string cmd;
        while (running) {
            auto now = chrono::system_clock::now();
            auto t = chrono::system_clock::to_time_t(now);
            string ts = ctime(&t);
            ts.pop_back();
            
            cout << "[" << ts << "] OS@virtual:~$ ";
            getline(cin, cmd);
            
            if (cmd.empty()) continue;
            
            vector<string> args;
            stringstream ss(cmd);
            string token;
            while (ss >> token) args.push_back(token);
            
            string baseCmd = args[0];
            transform(baseCmd.begin(), baseCmd.end(), baseCmd.begin(), ::tolower);
            
            try {
                if (baseCmd == "start") {
                    if (args.size() < 3) {
                        cout << "Usage: start <name> <id> [pri]\n";
                    } else {
                        Priority p = Priority::NORMAL;
                        if (args.size() >= 4) {
                            string pri = args[3];
                            transform(pri.begin(), pri.end(), pri.begin(), ::tolower);
                            if (pri == "high") p = Priority::HIGH;
                            else if (pri == "critical") p = Priority::CRITICAL;
                            else if (pri == "low") p = Priority::LOW;
                        }
                        startProgram(args[1], args[2], p);
                    }
                }
                else if (baseCmd == "kill") {
                    if (args.size() < 2) cout << "Usage: kill <id>\n";
                    else terminateProgram(stoi(args[1]));
                }
                else if (baseCmd == "pause") {
                    if (args.size() < 2) cout << "Usage: pause <id>\n";
                    else pauseProgram(stoi(args[1]));
                }
                else if (baseCmd == "resume") {
                    if (args.size() < 2) cout << "Usage: resume <id>\n";
                    else resumeProgram(stoi(args[1]));
                }
                else if (baseCmd == "deps") {
                    if (args.size() < 3) cout << "Usage: deps <id> <dependsOn>\n";
                    else addDependency(stoi(args[1]), stoi(args[2]));
                }
                else if (baseCmd == "req") {
                    if (args.size() < 3) cout << "Usage: req <id> <resource>\n";
                    else requestResource(stoi(args[1]), args[2]);
                }
                else if (baseCmd == "rel") {
                    if (args.size() < 3) cout << "Usage: rel <id> <resource>\n";
                    else releaseResource(stoi(args[1]), args[2]);
                }
                else if (baseCmd == "demo") demo();
                else if (baseCmd == "list") listTasks();
                else if (baseCmd == "schedule") {
                    int cycles = (args.size() >= 2) ? stoi(args[1]) : 1;
                    runScheduler(cycles);
                }
                else if (baseCmd == "queues") showQueues();
                else if (baseCmd == "cores") showCores();
                else if (baseCmd == "stats") showStats();
                else if (baseCmd == "exit") running = false;
                else if (baseCmd == "help") {
                    cout << "  start <name> <id> [pri] - Start program\n"
                         << "  kill <id>               - Terminate program\n"
                         << "  pause <id>              - Save context & pause\n"
                         << "  resume <id>             - Restore context & resume\n"
                         << "  deps <id> <dependsOn>   - Add dependency lock\n"
                         << "  req <id> <resource>     - Request exclusive resource\n"
                         << "  rel <id> <resource>     - Release exclusive resource\n"
                         << "  schedule [N]            - Run scheduler cycles\n"
                         << "  list / queues / cores / stats - Status views\n"
                         << "  demo / exit\n";
                }
                else cout << "Unknown command. Try: help\n";
            } catch (const invalid_argument& e) {
                cout << "❌ Error: Invalid number format. Expected an integer.\n";
            } catch (const out_of_range& e) {
                cout << "❌ Error: Number out of range.\n";
            }
        }
    }
};

int main() {
    srand(time(nullptr));
    VirtualOS os(4, 100);
    os.interactiveMode();
    return 0;
}
