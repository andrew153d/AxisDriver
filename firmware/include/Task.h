#pragma once

#include <vector>
#include <cstdint>
#include <Arduino.h>

class ITask {
private:
    uint32_t lastExecutionTime = 0;
    bool isRunning = false;
protected: 
    uint32_t executionPeriod = 0;
public:

private:

protected:
    virtual void OnStart() = 0;   // Pure virtual function
    virtual void OnStop() = 0;    // Pure virtual function
    virtual void OnRun() = 0;     // Pure virtual function

public:
    void Start();   // Pure virtual function
    void Stop();    // Pure virtual function
    void Run();     // Pure virtual function
};

class TaskManager {
private:
    std::vector<ITask*> tasks;   // Vector to hold pointers to tasks

public:
    // Add a task to the task manager
    void AddTask(ITask* task) {
        tasks.push_back(task);
    }

    void StartTasks(){
        for (ITask* task : tasks) {
            task->Start();
        }
    }

    // Run all tasks
    void RunTasks() {
        for (ITask* task : tasks) {
            task->Run();
        }
    }

    void StopTasks(){
        for (ITask* task : tasks) {
            task->Stop();
        }
    }
};