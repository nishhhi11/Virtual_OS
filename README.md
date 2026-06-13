# Virtual OS Core Task Thread Supervisor

## Project Overview
This project implements a virtual operating system task scheduler demonstrating various data structures and algorithms for process management, scheduling, and resource allocation.

## Data Structures Used

1. **Hash Tables (unordered_map)** - Task Directory for O(1) lookups
2. **Priority Queue** - Priority-based scheduling
3. **Queue (FIFO)** - Processing line for fair scheduling  
4. **Stack (LIFO)** - Context switching (undo/restore)
5. **Binary Search** - Security check for approved programs
6. **Graph (Adjacency List)** - Dependency grid between tasks
7. **Arrays/Vectors** - Core balancing and load management

## Features Implemented

✅ **Task Directory** - Fast O(1) task management  
✅ **Context Undo** - LIFO context saving/restoring  
✅ **Processing Line** - FIFO queue for fair scheduling  
✅ **Security Check** - Binary search O(log n) validation  
✅ **Priority Sorter** - Max-heap priority queue  
✅ **Dependency Grid** - Graph-based dependency tracking  
✅ **Fastest Lock** - Resource management with deadlock detection  
✅ **Core Balancer** - Load balancing across multiple cores  

## How to Compile and Run

```bash
# Clean previous builds
make clean

# Compile the project
make

# Run the virtual OS
make run
```

## Project Structure
```
DSA-Project/
├── bin/           # Executable files
├── docs/          # Documentation
├── include/       # Header files
├── obj/           # Object files  
├── src/           # Source files
├── tests/         # Unit tests
├── Makefile       # Build automation
└── README.md      # This file
```
