# OS Virtual Core Task Thread Supervisor

## 1. Project Title

**OS Virtual Core Task Thread Supervisor**

---

## 2. Problem Statement

Modern operating systems execute multiple programs simultaneously, including web browsers, media players, system services, downloads, and background applications. Since processor cores can execute only one instruction stream at a time, the operating system must efficiently manage CPU time, resources, priorities, and dependencies among tasks.

The objective of this project is to simulate a kernel-level operating system scheduler capable of managing multiple concurrent processes while ensuring fairness, efficiency, security, and optimal resource utilization. The system demonstrates core operating system concepts such as task scheduling, context switching, dependency management, load balancing, and resource allocation using advanced Data Structures and Algorithms.

---

## 3. Objectives

The primary objectives of this project are:

* Design and implement a virtual operating system scheduler.
* Simulate multitasking through process scheduling and context switching.
* Manage process priorities using efficient scheduling algorithms.
* Maintain a centralized task directory for fast task lookup and management.
* Implement dependency tracking between processes and resources.
* Prevent resource conflicts and deadlocks through lock management.
* Distribute workloads dynamically across multiple CPU cores.
* Demonstrate real-world operating system concepts using Data Structures and Algorithms.

---

## 4. System Overview / Architecture

The Virtual OS Supervisor is divided into multiple modules that work together to simulate the behavior of an operating system kernel.

### System Components

#### 1. Task Directory

Stores information about all active tasks including:

* Task ID
* Security ID
* Process Name
* State (Running, Waiting, Paused)
* Priority Level
* Assigned CPU Core
* CPU Usage Statistics

#### 2. Security Manager

Verifies process authorization before execution by checking process security identifiers against approved records.

#### 3. Scheduler Engine

Responsible for:

* Task Scheduling
* Priority Handling
* Context Switching
* Time Slice Management
* Process Preemption

#### 4. Dependency Manager

Tracks relationships between processes and resources using a graph-based structure.

Example:

music.exe → audio.exe

#### 5. Resource Lock Manager

Controls access to shared resources and ensures that processes wait safely when resources are already in use.

#### 6. Core Balancer

Distributes processes across multiple CPU cores to maintain balanced workloads and maximize system efficiency.

---

## 5. System Flow

```text
User Command
      │
      ▼
Task Registration
      │
      ▼
Security Verification
      │
      ▼
Task Directory
      │
      ▼
Priority Queue
      │
      ▼
Scheduler Engine
      │
      ▼
Context Switching
      │
      ▼
Core Assignment
      │
      ▼
Dependency & Resource Checks
      │
      ▼
Task Execution
      │
      ▼
Load Balancing
```

---

## 6. Data Structures and Algorithms Used

### Hash Map (Task Directory)

Used for:

* Fast task lookup
* Task insertion
* Task deletion
* Process state updates

Average Complexity:

* Insert → O(1)
* Search → O(1)
* Delete → O(1)

---

### Priority Queue

Used for:

* Scheduling critical processes first
* Managing execution order

Priority Levels:

1. CRITICAL
2. HIGH
3. NORMAL

Complexity:

* Insert → O(log N)
* Remove Highest Priority → O(log N)

---

### Queue (Processing Line)

Used to implement:

* First Come First Serve behavior
* Round Robin scheduling

Complexity:

* Enqueue → O(1)
* Dequeue → O(1)

---

### Stack (Context Undo)

Used for:

* Saving process state
* Restoring process state

Functions:

* saveContextToStack()
* restoreContextFromStack()

Complexity:

* Push → O(1)
* Pop → O(1)

---

### Graph (Dependency Grid)

Represents process dependencies.

Example:

Browser → Network Service

Music Player → Audio Driver

Used to:

* Track process relationships
* Prevent invalid process termination
* Support resource allocation decisions

Complexity:

* Add Dependency → O(1)
* Traverse Dependencies → O(V + E)

---

### Round Robin Scheduling Algorithm

Ensures fair CPU time allocation among tasks.

Benefits:

* Prevents starvation
* Improves responsiveness
* Simulates real operating system scheduling

---

## 7. Features Implemented

### Task Directory

Maintains a master record of all active processes.

### Context Switching

Saves and restores process states during task preemption.

### Processing Line

Manages waiting processes using queue-based scheduling.

### Security Verification

Validates every task before execution.

### Priority Scheduling

Executes critical processes before normal tasks.

### Dependency Grid

Tracks relationships between processes and services.

### Resource Lock Management

Handles shared resources and prevents conflicts.

### Core Balancing

Distributes tasks dynamically across 4 processor cores.

### Process Preemption

Allows critical tasks to interrupt lower-priority tasks when necessary.

---

## 8. Implementation Approach

The project follows a modular C++ architecture.

### Major Modules

* TaskManager
* Scheduler
* CoreBalancer
* ResourceManager
* DependencyManager

Each module performs a dedicated operating system function while interacting with the scheduler to simulate real kernel behavior.

The simulation operates in scheduling cycles where:

1. Tasks are evaluated.
2. Priorities are checked.
3. Contexts are saved/restored.
4. Resources are allocated.
5. Load balancing is performed.

---

## 9. Time and Space Complexity Analysis

| Operation                  | Complexity |
| -------------------------- | ---------- |
| Add Task                   | O(1)       |
| Remove Task                | O(1)       |
| Search Task                | O(1)       |
| Save Context               | O(1)       |
| Restore Context            | O(1)       |
| Insert into Priority Queue | O(log N)   |
| Remove from Priority Queue | O(log N)   |
| Dependency Traversal       | O(V + E)   |
| Scheduler Cycle            | O(N log N) |
| Load Balancing             | O(N)       |

### Space Complexity

Overall Space Complexity:

O(N)

where N is the number of active tasks stored in the Task Directory and Scheduler Queues.

---

## 10. Project Structure

```text
DSA-Project/
│
├── bin/
├── docs/
├── include/
├── obj/
├── src/
├── tests/
│
├── Makefile
├── README.md
├── system_log.txt
└── .clangd
```

---

## 11. Execution Steps

### Compile the Project

```bash
make
```

### Run the Virtual OS Supervisor

```bash
./bin/virtual_os
```

### Available Commands

```text
start
kill
pause
resume
deps
req
rel
demo
list
schedule N
queues
cores
stats
exit
```

---

## 12. Sample Inputs and Outputs

### Example 1

Input:

```text
start game.exe
```

Output:

```text
game.exe STARTED
Task ID: 15
Assigned to Core 2
Security Check PASSED
```

### Example 2

Input:

```text
schedule 3
```

Output:

```text
SCHEDULER RUNNING
Cycle 1
Cycle 2
Cycle 3
AUTO-BALANCE:
Migrating game.exe from Core 2 to Core 1
```

---

## 13. Screenshots

### System Startup

<p align = 'left' ><img width="556" height="727" alt="Screenshot 2026-06-14 at 2 00 19 AM" src="https://github.com/user-attachments/assets/d9c00c40-f569-4fdb-9c49-1dd13a766169" /> </p>



### Demo Mode Execution

<p align = 'left' ><img width="415" height="666" alt="Screenshot 2026-06-14 at 2 01 34 AM" src="https://github.com/user-attachments/assets/a5825264-e261-40ae-b2a4-d26e1769a87b" /></p>


<p align = 'left' ><img width="470" height="675" alt="Screenshot 2026-06-14 at 2 02 00 AM" src="https://github.com/user-attachments/assets/dcffa0e6-fdcf-4fdb-98a0-7d671267a278" /></p>


<p align = 'left' ><img width="510" height="303" alt="Screenshot 2026-06-14 at 2 02 14 AM" src="https://github.com/user-attachments/assets/2fd9f090-22f3-4881-89d2-bea7aae93d8e" /></p>



### Scheduler Running

<p align = 'left' ><img width="451" height="745" alt="Screenshot 2026-06-14 at 2 08 47 AM" src="https://github.com/user-attachments/assets/124c2c2c-1412-4def-8541-7de1f6e6297d" /></p>


<p align = 'left' ><img width="398" height="151" alt="Screenshot 2026-06-14 at 2 07 27 AM" src="https://github.com/user-attachments/assets/09c1c590-6fd8-4aac-b1b2-351bfcfe8ac8" /></p>



### Dependency and Resource Management

<p align = 'left' ><img width="363" height="207" alt="Screenshot 2026-06-14 at 2 09 39 AM" src="https://github.com/user-attachments/assets/06d05322-a87a-4860-b089-2483eda082bd" /></p>


### Task Directory View

<p align = 'left' ><img width="399" height="239" alt="Screenshot 2026-06-14 at 2 10 15 AM" src="https://github.com/user-attachments/assets/ed33dee5-ccec-4d7b-a8b5-8b629c53ebd1" /></p>


### Core Balancing Output

<p align = 'left' ><img width="460" height="269" alt="Screenshot 2026-06-14 at 2 11 59 AM" src="https://github.com/user-attachments/assets/72733369-834f-4d31-89d2-9941088a0c61" /></p>


---

## 14. Results and Observations

The Virtual OS Supervisor successfully demonstrates major operating system scheduling concepts through simulation.

Observations:

* Critical processes consistently receive higher scheduling priority.
* Context switching preserves process state effectively.
* Resource dependencies are managed correctly.
* Load balancing prevents core over-utilization.
* Security checks ensure only authorized tasks execute.
* Dependency tracking reduces the possibility of resource conflicts.

The scheduler remained stable throughout multiple execution cycles while managing concurrent tasks across four processor cores.

---

## 15. Conclusion

Virtual OS Supervisor v2.0 successfully simulates the behavior of a modern operating system scheduler using advanced Data Structures and Algorithms. The project demonstrates practical implementation of task scheduling, context switching, dependency management, resource locking, priority-based execution, and multi-core load balancing.

The system provides an educational model of how operating systems coordinate multiple processes efficiently while maintaining stability, fairness, and performance.
