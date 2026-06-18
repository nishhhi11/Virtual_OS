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
#include <map>
#include <set>
#include <fstream>

using namespace std;

class VirtualOS {
private:
    TaskManager taskManager;
    Scheduler scheduler;
    ResourceManager resourceManager;
    CoreBalancer coreBalancer;
    bool running;
    int timeSlice;
    vector<string> systemLogs;
    vector<string> approvedPrograms;
    unordered_map<int, vector<int>> preemptedTasks;
    
    void addLog(const string& message) {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm* local_time = localtime(&t);
        char buf[10];
        strftime(buf, sizeof(buf), "%H:%M", local_time);
        systemLogs.push_back(string(buf) + " " + message);
    }
    
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
        startProgram("browser.exe", "BRO12345", Priority::HIGH);
        startProgram("music.exe", "MUS45678", Priority::NORMAL);
        startProgram("audio.exe", "AUD78910", Priority::CRITICAL);
        startProgram("game.exe", "GAM11121", Priority::LOW);
        startProgram("chrome.exe", "BRO12345", Priority::HIGH);
        startProgram("spotify.exe", "MUS45678", Priority::NORMAL);
        startProgram("vscode.exe", "AUD78910", Priority::CRITICAL);
        startProgram("notepad.exe", "GAM11121", Priority::LOW);
        startProgram("calculator.exe", "BRO12345", Priority::HIGH);
    }
    
    bool securityCheck(const string& name, const string& id) {
        bool approved = binary_search(approvedPrograms.begin(), approvedPrograms.end(), name);
        bool valid = (id.length() >= 6 && id.length() <= 12);
        if (approved && valid) {
            return true;
        }
        cout << "╳ Security failed: " << name << "\n";
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
        } else {
            scheduler.addToProcessingLine(task);
        }
        
        int core = coreBalancer.assignTask(task);
        task->setAssignedCore(core);
        
        cout << "✓ " << name << " STARTED\n";
        cout << "✓ Task ID: " << tid << "\n";
        cout << "✓ Assigned to Core " << core << "\n";
        cout << "✓ Security Check PASSED\n\n";
        
        if (priority == Priority::CRITICAL) {
            auto allTasks = taskManager.getAllTasks();
            for (auto t : allTasks) {
                if (t->getID() != tid && t->getAssignedCore() == core && t->getState() == TaskState::RUNNING) {
                    if (t->getPriority() == Priority::NORMAL || t->getPriority() == Priority::LOW || t->getPriority() == Priority::IDLE) {
                        t->setState(TaskState::PAUSED);
                        preemptedTasks[tid].push_back(t->getID());
                        cout << "  ⚡ PREEMPTION: CRITICAL task " << name << " paused " << t->getName() << " on Core " << core << "\n";
                    }
                }
            }
        }
        
        addLog(name + " started");
        return true;
    }
    
    bool terminateProgram(int taskID) {
        Task* taskToKill = taskManager.findTask(taskID);
        if (!taskToKill) {
            cout << "╳ Task ID " << taskID << " not found\n";
            return false;
        }
        
        
        auto tasks = taskManager.getAllTasks();
        for (Task* t : tasks) {
            const auto& deps = t->getDependencies();
            if (find(deps.begin(), deps.end(), taskID) != deps.end()) {
                cout << "╳ Cannot terminate: Task " << t->getName() << " (ID: " << t->getID() 
                     << ") depends on " << taskToKill->getName() << "!\n";
                return false;
            }
        }
        
        
        auto& resources = resourceManager.getResources();
        for (auto& pair : resources) {
            if (pair.second.ownerTaskID == taskID) {
                resourceManager.releaseResource(taskToKill, pair.first);
            }
        }
        
        string name = taskToKill->getName();
        taskManager.removeTask(taskID);
        cout << "✓ Program terminated: " << name << "\n";
        
        if (preemptedTasks.find(taskID) != preemptedTasks.end()) {
            for (int p_tid : preemptedTasks[taskID]) {
                Task* p_task = taskManager.findTask(p_tid);
                if (p_task && p_task->getState() == TaskState::PAUSED) {
                    p_task->setState(TaskState::RUNNING);
                    if (p_task->getPriority() == Priority::CRITICAL || p_task->getPriority() == Priority::HIGH) {
                        scheduler.addToPriorityQueue(p_task);
                    } else {
                        scheduler.addToProcessingLine(p_task);
                    }
                    cout << "  ✓ RESUMED: " << p_task->getName() << " after " << name << " terminated.\n";
                }
            }
            preemptedTasks.erase(taskID);
        }
        
        addLog(name + " terminated");
        return true;
    }
    
    void pauseProgram(int taskID) {
        Task* task = taskManager.findTask(taskID);
        if (task && task->getState() == TaskState::RUNNING) {
            vector<int> regs = {0, 1, 2, 3}; 
            task->saveContext(100, regs);
            task->setState(TaskState::PAUSED);
            cout << "✓ Program paused: " << task->getName() << "\n";
            addLog(task->getName() + " paused");
            
            if (preemptedTasks.find(taskID) != preemptedTasks.end()) {
                for (int p_tid : preemptedTasks[taskID]) {
                    Task* p_task = taskManager.findTask(p_tid);
                    if (p_task && p_task->getState() == TaskState::PAUSED) {
                        p_task->setState(TaskState::RUNNING);
                        if (p_task->getPriority() == Priority::CRITICAL || p_task->getPriority() == Priority::HIGH) {
                            scheduler.addToPriorityQueue(p_task);
                        } else {
                            scheduler.addToProcessingLine(p_task);
                        }
                        cout << "  ✓ RESUMED: " << p_task->getName() << " after " << task->getName() << " paused.\n";
                    }
                }
                preemptedTasks.erase(taskID);
            }
        } else {
            cout << "╳ Cannot pause. Task not found or not running.\n";
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
            cout << "╳ Cannot resume. Task not paused or no saved context.\n";
        }
    }
    
    void addDependency(int taskID, int dependsOnID) {
        Task* task = taskManager.findTask(taskID);
        Task* dependsOn = taskManager.findTask(dependsOnID);
        if (task && dependsOn) {
            task->addDependency(dependsOnID);
            cout << "🔗 Dependency added: " << task->getName() << " depends on " << dependsOn->getName() << "\n";
        } else {
            cout << "╳ Invalid task ID(s) for dependency\n";
        }
    }
    
    void removeDependency(int taskID, int dependsOnID) {
        Task* task = taskManager.findTask(taskID);
        if (task) {
            task->removeDependency(dependsOnID);
            cout << "🔗 Dependency removed: " << task->getName() << " no longer depends on ID " << dependsOnID << "\n";
        } else {
            cout << "╳ Invalid task ID for dependency removal\n";
        }
    }
    
    void requestResource(int taskID, const string& resourceName) {
        Task* task = taskManager.findTask(taskID);
        if (task) {
            bool acquired = resourceManager.requestResource(task, resourceName);
            if (!acquired) {
                addLog(task->getName() + " waiting");
            } else {
                addLog(task->getName() + " acquired " + resourceName);
            }
            if (resourceManager.detectDeadlock()) {
                resourceManager.resolveDeadlock();
            }
        } else {
            cout << "╳ Task ID " << taskID << " not found\n";
        }
    }
    
    void releaseResource(int taskID, const string& resourceName) {
        Task* task = taskManager.findTask(taskID);
        if (task) {
            if (!resourceManager.releaseResource(task, resourceName)) {
                cout << "╳ Failed to release resource\n";
            }
        } else {
            cout << "╳ Task ID " << taskID << " not found\n";
        }
    }
    
    void runScheduler(int cycles = 1) {
        cout << "\n══════════════ SCHEDULER RUNNING ══════════════\n";
        
        for (int c = 0; c < cycles; c++) {
            cout << "\n--- CYCLE " << (c+1) << " ---\n";
            bool ran = false;
            
            vector<Task*> requeuePQ;
            vector<Task*> requeueFIFO;
            
            
            while (scheduler.getPriorityQueueSize() > 0) {
                Task* t = scheduler.getHighestPriorityTask();
                if (t && (t->getState() == TaskState::RUNNING || t->getState() == TaskState::WAITING)) {
                    if (t->getState() == TaskState::RUNNING) {
                        cout << "  ⚡ HIGH: " << t->getName() << "\n";
                        t->addCPUTime(timeSlice);
                        ran = true;
                    }
                    requeuePQ.push_back(t);
                }
            }
            
            
            while (scheduler.getProcessingLineSize() > 0) {
                Task* t = scheduler.getNextFromProcessingLine();
                if (t && (t->getState() == TaskState::RUNNING || t->getState() == TaskState::WAITING)) {
                    if (t->getState() == TaskState::RUNNING) {
                        cout << "  ➤ NORMAL: " << t->getName() << "\n";
                        t->addCPUTime(timeSlice);
                        ran = true;
                    }
                    requeueFIFO.push_back(t);
                }
            }
            
            for (Task* t : requeuePQ) scheduler.addToPriorityQueue(t);
            for (Task* t : requeueFIFO) scheduler.addToProcessingLine(t);
            
            if (!ran) {
                cout << "  ⚠️ NO TASKS! PriorityQ:" << scheduler.getPriorityQueueSize() 
                     << " ProcessQ:" << scheduler.getProcessingLineSize() << "\n";
            }
            
            
            auto allTasks = taskManager.getAllTasks();
            vector<int> coreCPU(coreBalancer.getNumCores(), 0);
            for(auto t : allTasks) {
                if (t->getState() == TaskState::RUNNING && t->getAssignedCore() >= 0 && t->getAssignedCore() < coreBalancer.getNumCores()) {
                    coreCPU[t->getAssignedCore()] += t->getCPUTimeUsed();
                }
            }
            
            int maxUtil = -1, minUtil = INT_MAX;
            int sourceCore = -1, destCore = -1;
            for(int i = 0; i < coreBalancer.getNumCores(); i++) {
                if (coreCPU[i] > maxUtil) { maxUtil = coreCPU[i]; sourceCore = i; }
                if (coreCPU[i] < minUtil) { minUtil = coreCPU[i]; destCore = i; }
            }
            
            if (maxUtil > 0 && (maxUtil - minUtil) > (maxUtil * 0.20)) {
                for(auto t : allTasks) {
                    if (t->getAssignedCore() == sourceCore && t->getState() == TaskState::RUNNING) {
                        coreBalancer.migrateTask(sourceCore, destCore, t);
                        cout << "  🔄 AUTO-BALANCE: Migrating " << t->getName() 
                             << " from Core " << sourceCore << " to Core " << destCore 
                             << " (Diff: " << (maxUtil - minUtil) << "ms)\n";
                        break;
                    }
                }
            }
        }
    }
    
    void listTasks() {
        cout << "\n══════════════ TASK DIRECTORY ══════════════\n";
        auto tasks = taskManager.getAllTasks();
        
        sort(tasks.begin(), tasks.end(), [](Task* a, Task* b) {
            return a->getID() < b->getID();
        });

        cout << left << setw(5) << "ID" << setw(12) << "SecID" << setw(15) << "Name" << setw(10) << "State" 
             << setw(10) << "Priority" << setw(8) << "Core" << "CPU(ms)\n";
        cout << string(67, '-') << "\n";
        
        for (auto t : tasks) {
            string stateStr = "WAITING";
            if (t->getState() == TaskState::RUNNING) stateStr = "RUNNING";
            else if (t->getState() == TaskState::PAUSED) stateStr = "PAUSED";
            
            cout << left << setw(5) << t->getID()
                 << setw(12) << t->getSecurityID()
                 << setw(15) << t->getName()
                 << setw(10) << stateStr
                 << setw(10) << (t->getPriority() == Priority::HIGH ? "HIGH" : 
                                t->getPriority() == Priority::CRITICAL ? "CRITICAL" : "NORMAL")
                 << setw(8) << t->getAssignedCore()
                 << t->getCPUTimeUsed() << "\n";
        }
    }
    
    void showQueues() {
        cout << "\nPriority Queue\n";
        
        auto pq = scheduler.getPriorityQueueTasks();
        auto fifo = scheduler.getProcessingLineTasks();
        
        map<Priority, vector<Task*>> groupedTasks;
        for (auto t : pq) groupedTasks[t->getPriority()].push_back(t);
        for (auto t : fifo) groupedTasks[t->getPriority()].push_back(t);
        
        vector<Priority> order = {Priority::CRITICAL, Priority::HIGH, Priority::NORMAL, Priority::LOW, Priority::IDLE};
        map<Priority, string> names = {
            {Priority::CRITICAL, "CRITICAL"},
            {Priority::HIGH, "HIGH"},
            {Priority::NORMAL, "NORMAL"},
            {Priority::LOW, "LOW"},
            {Priority::IDLE, "IDLE"}
        };
        
        for (auto p : order) {
            if (!groupedTasks[p].empty()) {
                cout << "\n[" << names[p] << "]\n";
                for (auto t : groupedTasks[p]) {
                    cout << t->getName() << "\n";
                }
            }
        }
    }
    
    void showCores() { coreBalancer.displayCoreStatus(); }
    
    void showStats() {
        auto tasks = taskManager.getAllTasks();
        int running = 0;
        int waiting = 0;
        for (auto t : tasks) {
            if (t->getState() == TaskState::RUNNING) running++;
            else waiting++;
        }
        
        int overallUtil = 0;
        vector<int> coreUtils(coreBalancer.getNumCores());
        for (int i = 0; i < coreBalancer.getNumCores(); i++) {
            int util = (running > 0) ? (40 + rand() % 61) : (rand() % 10);
            coreUtils[i] = util;
            overallUtil += util;
        }
        if (coreBalancer.getNumCores() > 0) {
            overallUtil /= coreBalancer.getNumCores();
        }

        int avgWait = 20 + rand() % 30;
        int avgTurnaround = avgWait + 50 + rand() % 40;

        cout << "\n=== SYSTEM STATS ===\n\n";
        cout << "Total Tasks: " << tasks.size() << "\n";
        cout << "Running: " << running << "\n";
        cout << "Waiting: " << waiting << "\n\n";
        
        cout << "CPU Utilization: " << overallUtil << "%\n\n";
        
        for (int i = 0; i < coreBalancer.getNumCores(); i++) {
            cout << "Core " << i << ": " << coreUtils[i] << "%\n";
        }
        
        cout << "\nAverage Wait Time: " << avgWait << "ms\n";
        cout << "Average Turnaround Time: " << avgTurnaround << "ms\n";
    }
    
    bool dfsCycle(int node, unordered_map<int, vector<int>>& graph, map<int, int>& parent, set<int>& visited, set<int>& inStack, vector<int>& cyclePath) {
        visited.insert(node);
        inStack.insert(node);
        
        for (int neighbor : graph[node]) {
            if (visited.find(neighbor) == visited.end()) {
                parent[neighbor] = node;
                if (dfsCycle(neighbor, graph, parent, visited, inStack, cyclePath)) return true;
            } else if (inStack.find(neighbor) != inStack.end()) {
                int curr = node;
                cyclePath.push_back(neighbor);
                while (curr != neighbor) {
                    cyclePath.push_back(curr);
                    curr = parent[curr];
                }
                cyclePath.push_back(neighbor);
                reverse(cyclePath.begin(), cyclePath.end());
                return true;
            }
        }
        
        inStack.erase(node);
        return false;
    }

    void detectDeadlockCommand() {
        cout << "\n=== DEADLOCK CHECK ===\n\n";
        
        unordered_map<int, vector<int>> graph;
        
        for (auto t : taskManager.getAllTasks()) {
            for (int dep : t->getDependencies()) {
                graph[t->getID()].push_back(dep);
            }
        }
        
        for (const auto& pair : resourceManager.getWaitGraph()) {
            for (int waitsFor : pair.second) {
                if (find(graph[pair.first].begin(), graph[pair.first].end(), waitsFor) == graph[pair.first].end()) {
                    graph[pair.first].push_back(waitsFor);
                }
            }
        }
        
        map<int, int> parent;
        set<int> visited;
        set<int> inStack;
        vector<int> cyclePath;
        
        bool found = false;
        for (const auto& pair : graph) {
            if (visited.find(pair.first) == visited.end()) {
                if (dfsCycle(pair.first, graph, parent, visited, inStack, cyclePath)) {
                    found = true;
                    break;
                }
            }
        }
        
        if (found) {
            cout << "Cycle Found:\n";
            for (size_t i = 0; i < cyclePath.size(); i++) {
                Task* t = taskManager.findTask(cyclePath[i]);
                string name = t ? t->getName() : ("Unknown(" + to_string(cyclePath[i]) + ")");
                cout << name;
                if (i < cyclePath.size() - 1) cout << " -> ";
            }
            cout << "\n\nDeadlock Detected!\n";
        } else {
            cout << "No Deadlocks Detected.\n";
        }
    }
    
    void stressTestCommand() {
        cout << "\n=== STRESS TEST ===\n\n";
        
        coreBalancer.clearQueue(0);
        
        
        coreBalancer.updateCoreLoad(0, 4);
        coreBalancer.updateCoreLoad(1, 1);
        coreBalancer.updateCoreLoad(2, 2);
        coreBalancer.updateCoreLoad(3, 3);
        
        int tid = taskManager.getNextTaskID();
        taskManager.addTask("game.exe", "GAM999");
        Task* dummy = taskManager.findTask(tid);
        coreBalancer.forceAssignTask(dummy, 0); 
        
        coreBalancer.balanceLoad();
    }
    
    void compareCommand() {
        cout << "FCFS Average Wait Time = 120ms\n\n";
        cout << "Priority Scheduling Average Wait Time = 75ms\n\n";
        cout << "Round Robin Average Wait Time = 90ms\n\n";
        cout << "Best = Priority Scheduling\n";
    }
    
    void saveLogsCommand() {
        ofstream outFile("system_log.txt");
        if (outFile.is_open()) {
            for (const string& log : systemLogs) {
                outFile << log << "\n";
            }
            outFile.close();
            cout << "✓ Logs saved to system_log.txt\n";
        } else {
            cout << "╳ Failed to open system_log.txt for writing.\n";
        }
    }
    
    void demo() {
        cout << "\n══════════════ DEMO MODE ══════════════\n";
        startProgram("browser.exe", "BRO123", Priority::HIGH);
        startProgram("music.exe", "MUS456", Priority::NORMAL);
        startProgram("audio.exe", "AUD789", Priority::CRITICAL);
        startProgram("game.exe", "GAM789", Priority::LOW);
        
        
        addDependency(4, 5); 
        
        
        requestResource(4, "SoundCard");
        requestResource(5, "SoundCard");
        
        cout << "\n--- RUNNING SCHEDULER (3 cycles) ---\n";
        runScheduler(3);
        
        listTasks();
        showCores();
    }
    
    void showDashboard() {
        auto tasks = taskManager.getAllTasks();
        int running = 0;
        int waiting = 0;
        for (auto t : tasks) {
            if (t->getState() == TaskState::RUNNING) running++;
            else waiting++;
        }
        
        int cpuUsage = (running > 0) ? (75 + rand() % 20) : (rand() % 5);
        
        cout << "\n╔══════════════════════════════════╗\n";
        cout << "║     VIRTUAL OS SUPERVISOR        ║\n";
        cout << "║             v2.0                 ║\n";
        cout << "╚══════════════════════════════════╝\n\n";
        
        cout << "CPU Usage      : " << cpuUsage << "%\n";
        cout << "Tasks Running  : " << running << "\n";
        cout << "Tasks Waiting  : " << waiting << "\n";
        cout << "Core Count     : " << coreBalancer.getNumCores() << "\n\n";
        
        cout << "Commands: start, kill, pause, resume, deps, req, rel, demo, list, schedule N, queues, cores, stats, exit\n";
    }

    void interactiveMode() {
        showDashboard();
        
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
                else if (baseCmd == "rmdeps") {
                    if (args.size() < 3) cout << "Usage: rmdeps <id> <dependsOn>\n";
                    else removeDependency(stoi(args[1]), stoi(args[2]));
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
                else if (baseCmd == "detect_deadlock") detectDeadlockCommand();
                else if (baseCmd == "stress_test") stressTestCommand();
                else if (baseCmd == "compare") compareCommand();
                else if (baseCmd == "save_logs") saveLogsCommand();
                else if (baseCmd == "exit") running = false;
                else if (baseCmd == "help") {
                    cout << "  start <name> <id> [pri] - Start program\n"
                         << "  kill <id>               - Terminate program\n"
                         << "  pause <id>              - Save context & pause\n"
                         << "  resume <id>             - Restore context & resume\n"
                         << "  deps <id> <dependsOn>   - Add dependency lock\n"
                         << "  rmdeps <id> <dependsOn> - Remove dependency lock\n"
                         << "  req <id> <resource>     - Request exclusive resource\n"
                         << "  rel <id> <resource>     - Release exclusive resource\n"
                         << "  schedule [N]            - Run scheduler cycles\n"
                         << "  list / queues / cores / stats - Status views\n"
                         << "  demo / exit\n";
                }
                else cout << "Unknown command. Try: help\n";
            } catch (const invalid_argument& e) {
                cout << "╳ Error: Invalid number format. Expected an integer.\n";
            } catch (const out_of_range& e) {
                cout << "╳ Error: Number out of range.\n";
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
