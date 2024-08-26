#include "Task.h"

void ITask::Run(){
    if(!isRunning)
        return;
    if(executionPeriod == 0 || millis()-lastExecutionTime>executionPeriod){
        lastExecutionTime = millis();
        OnRun();
    }
}

void ITask::Start(){
    if(isRunning)
        return;
    isRunning = true;
    OnStart();
}

void ITask::Stop(){
    if(!isRunning)
        return;
    isRunning = false;
    OnStop();
}