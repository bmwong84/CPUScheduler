//Bryant Wong
//CSCI 3453 Lab 2

#ifndef PROCESS_
#define PROCESS_
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
//#include <stdio.h>	//to convert
#include <vector>
#include <algorithm>
#include <iomanip>

//this is the PCB - process control block
class Process
{
private:
	//attributes
	int arrivalTime;
	float eventTime;		//the time at which the current event is occurring to this process
	float completionTime=0;
	float waitingTime=0;
	float responseTime=0;
	float beginProcessing=0;	//time when the process begins processing for the first time
	int CPUBurst;
	float CPUBurstRemaining;	//should I make this "time left"?
	int priority;
	
	int PID;
	int contextSwitchCounter=0;

	char state;		//to keep track whether process is "R" ready or "W" waiting or "C" complete


public:
	Process();
	void setCompletionTime(float _complete)
	{
		completionTime = (float)_complete;
	}
	float getCompletionTime()
	{
		return completionTime;
	}
	void setSwitchCounter(int _switch)
	{
		contextSwitchCounter = _switch;
	}
	int getSwitchCounter()
	{
		return contextSwitchCounter;
	}
	void setBeginProcessing(float _begin)
	{
		beginProcessing = (float)_begin;
	}
	float getBeginProcessing()
	{
		return beginProcessing;
	}
	void setProcessState(char _state)
	{
		state = _state;
	}
	char getProcessState()
	{
		return state;
	}
	void setEventTime(float _event)
	{
		eventTime = (float)_event;
	}
	float getEventTime()
	{
		return eventTime;
	}
	void setArrivalTime(int _arrival)
	{
		arrivalTime = _arrival;
	}
	int getArrivalTime()
	{
		return arrivalTime;
	}
	void setCPUBurstTime(int _burst)
	{
		CPUBurst = _burst;
	}
	int getCPUBurstTime()
	{
		return CPUBurst;
	}
	void setCPUBurstRemaining(float _burstRemaining)
	{
		CPUBurstRemaining = _burstRemaining;
	}
	float getCPUBurstRemaining()
	{
		return CPUBurstRemaining;
	}
	void setResponseTime(float _response)
	{
		responseTime = (float)_response;
	}
	float getResponseTime()
	{
		return responseTime;
	}
	void setWaitingTime(float _waiting)
	{
		waitingTime = (float)_waiting;
	}
	float getWaitingTime()
	{
		return waitingTime;
	}
	void setPriority(int _priority)
	{
		priority = _priority;
	}
	int getPriority()
	{
		return priority;
	}
	void setPID(int _PID)
	{
		PID = _PID;
	}
	int getPID()
	{
		return PID;
	}

};
#endif
