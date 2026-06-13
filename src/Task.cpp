#include "../include/Task.h"
#include <algorithm>
#include <iostream>

using namespace std;


Task::Task(int id, const string &name, const string &secID)
    : taskID(id), name(name), memoryUsage(0),
      state(TaskState::RUNNING), 
      priority(Priority::NORMAL), securityID(secID), assignedCore(-1),
      creationTime(time(nullptr)), cpuTimeUsed(0) {}

void Task::addDependency(int depID) {
  if (find(dependencies.begin(), dependencies.end(), depID) ==
      dependencies.end()) {
    dependencies.push_back(depID);
  }
}

void Task::removeDependency(int depID) {
  dependencies.erase(
      remove(dependencies.begin(), dependencies.end(), depID),
      dependencies.end());
}

void Task::saveContext(int pc, const vector<int> &regs) {
  savedContext.programCounter = pc;
  savedContext.registers = regs;
  savedContext.lastSavedTime = time(nullptr);
}

void Task::restoreContext(int &pc, vector<int> &regs) {
  pc = savedContext.programCounter;
  regs = savedContext.registers;
}

void Task::display() const {
  cout << "Task ID: " << taskID << "\n"
            << "Name: " << name << "\n"
            << "Memory: " << memoryUsage << " MB\n"
            << "State: "
            << (state == TaskState::RUNNING   ? "Running"
                : state == TaskState::PAUSED  ? "Paused"
                : state == TaskState::WAITING ? "Waiting"
                                              : "Terminated")
            << "\n"
            << "Priority: " << static_cast<int>(priority) << "\n"
            << "Core: "
            << (assignedCore == -1 ? "Not assigned"
                                   : to_string(assignedCore))
            << "\n"
            << "CPU Time: " << cpuTimeUsed << " ms\n";
}