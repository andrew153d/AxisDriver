#include "Task.h"

void ITask::Run(){
    if(!isRunning)
        return;
    if(millis()-lastExecutionTime>executionPeriod){
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