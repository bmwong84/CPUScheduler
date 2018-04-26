//Bryant Wong
//CSCI 3453 Lab 2
#ifndef SCHEDULER_
#define SCHEDULER_

#include "Queue.h"



class Scheduler
{
private:
	float clock=0;				//total time elapsed
	float avgCPUBurstTime;
	float avgWait;
	float avgTurnaround;
	float avgResponse;
	int contextSwitchCounter=0;	//total number of context switches
	int timeQuantum;

public:
	Scheduler();
	//each of these must ask for time quantum as integer > 0
	//simulate fcfs
	void runFCFS(Queue* readyQueue);
	//simulate SRTF
	void runSRTF(Queue* readyQueue);
	//simulate RR
	void runRR(Queue* readyQueue);
	//simulate PP
	void runPP(Queue* readyQueue);

	void setTimeQuantum();

	void readProcesses(Queue* readyQueue, std::string filename);  //process input from input file
	void menu(Queue* readyQueue);
	void printOutput(std::vector<Process> completed, int choice);
	void printOutfile(std::vector<Process> completed, int choice);

	//process begin running a process event/end running a process event
	void FCFSBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed);
	void RRBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed, Queue& arrivalQueue);
	void PPBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed, Queue& arrivalQueue);
	void SRTFBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed, Queue& arrivalQueue);


	//void FCFSEndRun(Queue* readyQueue);
	//void SRTFEndRun(Queue readyQueue);
	//void RREndRun(Queue readyQueue);
	//void PPEndRun(Queue readyQueue);
	//perform context switch
	//void contextSwitch(Process switchTo, Queue readyQueue);

	//maybe add some sorting
	//void sortQueuePriority(Queue* queue);
	//void sortQueueBurstRemaining(Queue* queue);


};


#endif