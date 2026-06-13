#include "../include/TaskManager.h"
#include "../include/Scheduler.h"
#include "../include/CoreBalancer.h"
#include <iostream>
#include <cassert>

void testTaskManager() {
    TaskManager tm;
    
    // Test adding tasks
    assert(tm.addTask("Test1", "TEST123"));
    assert(tm.addTask("Test2", "TEST456"));
    assert(tm.getTaskCount() == 2);
    
    // Test finding tasks
    Task* task = tm.findTask(1);
    assert(task != nullptr);
    assert(task->getName() == "Test1");
    
    // Test removing tasks
    assert(tm.removeTask(1));
    assert(tm.getTaskCount() == 1);
    
    std::cout << "✓ TaskManager tests passed\n";
}

void testScheduler() {
    Scheduler scheduler;
    TaskManager tm;
    
    tm.addTask("TaskA", "A123");
    tm.addTask("TaskB", "B456");
    
    Task* task1 = tm.findTask(1);
    Task* task2 = tm.findTask(2);
    
    // Test priority queue
    task1->setPriority(Priority::HIGH);
    task2->setPriority(Priority::LOW);
    
    scheduler.addToPriorityQueue(task1);
    scheduler.addToPriorityQueue(task2);
    
    // Highest priority should come first
    Task* highest = scheduler.getHighestPriorityTask();
    assert(highest->getPriority() == Priority::HIGH);
    
    // Test context stack
    Context ctx;
    ctx.programCounter = 100;
    scheduler.saveContextToStack(ctx);
    assert(scheduler.hasSavedContexts());
    
    Context restored = scheduler.restoreContextFromStack();
    assert(restored.programCounter == 100);
    
    std::cout << "✓ Scheduler tests passed\n";
}

void testCoreBalancer() {
    CoreBalancer balancer(4);
    TaskManager tm;
    
    tm.addTask("CoreTest1", "CORE1");
    tm.addTask("CoreTest2", "CORE2");
    
    Task* task1 = tm.findTask(1);
    Task* task2 = tm.findTask(2);
    
    int core1 = balancer.assignTask(task1);
    int core2 = balancer.assignTask(task2);
    
    assert(core1 >= 0 && core1 < 4);
    assert(core2 >= 0 && core2 < 4);
    
    std::cout << "✓ CoreBalancer tests passed\n";
}

int main() {
    std::cout << "Running Unit Tests...\n\n";
    
    testTaskManager();
    testScheduler();
    testCoreBalancer();
    
    std::cout << "\n✅ All tests passed!\n";
    return 0;
}
