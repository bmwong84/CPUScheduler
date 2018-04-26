
//Bryant Wong
//CSCI 3453 Lab 2
#include "Scheduler.h"
//#include "Process.h"
//#include "Queue.h"

Process::Process() {}
Scheduler::Scheduler() {}



void Scheduler::menu(Queue* readyQueue)
{
	char menuchoice = '0';
	while (menuchoice != '5')
	{
		clock = 0;							//resetting values every iteration
		avgCPUBurstTime = 0;
		avgWait = 0;
		avgTurnaround = 0;
		avgResponse = 0;
		contextSwitchCounter = 0;
		timeQuantum = 0;
		std::string filename;
		std::cout << "Running simulation: Please enter the name of input file" << std::endl;
		std::getline(std::cin, filename);
		readProcesses(readyQueue, filename);		//read the input
		//SORT BY ORDER OF ARRIVAL?  THEN BY PID??
		std::cout << "Select simulation method: " << std::endl;
		std::cout << "1.  First Come First Serve." << std::endl;
		std::cout << "2.  Shortest Remaining Time First." << std::endl;
		std::cout << "3.  Round Robin." << std::endl;
		std::cout << "4.  Pre-emptive priority." << std::endl;
		std::cout << "5.  Exit." << std::endl;
		std::cin >> menuchoice;
		switch (menuchoice)
		{
		case'1':
		{
			runFCFS(readyQueue);
			break;
		}
		case '2':
		{
			runSRTF(readyQueue);
			break;
		}
		case '3':
		{
			runRR(readyQueue);
			break;
		}
		case'4':
		{
			runPP(readyQueue);
			break;
		}
		case '5':
		{
			break;
		}
		default:
			std::cout << "Invalid choice, enter a valid choice" << std::endl;
		}

	}
}

//predicates for stable sort
inline bool PIDSort(Process i, Process j)
{
	return (i.getPID() < j.getPID());
}
inline bool prioritySort(Process i, Process j)
{
	return (i.getPriority() < j.getPriority());
}
inline bool arrivalSort(Process i, Process j)
{
	return (i.getArrivalTime() < j.getArrivalTime());
}
inline bool burstRemainingSort(Process i, Process j)
{
	return (i.getCPUBurstRemaining() < j.getCPUBurstRemaining());
}

void Scheduler::runFCFS(Queue* readyQueue)
{
	//setTimeQuantum();			//for other algorithms
	Process tempProcess;
	std::vector<Process> completedProcesses;
	clock = readyQueue->peekFront().getArrivalTime();			//initial arrival
	while (readyQueue->isEmpty() == false)
	{
		tempProcess = readyQueue->peekFront();
		if (tempProcess.getProcessState() == 'R')
		{
			//clock = (float)tempProcess.getEventTime();
			FCFSBeginRun(tempProcess, readyQueue, completedProcesses);
		}
	}
	//sort by PID?

	printOutput(completedProcesses, 1);

	completedProcesses.erase(completedProcesses.begin(), completedProcesses.end());		//delete contents of completed vector
}

void Scheduler::runRR(Queue* readyQueue)
{
	setTimeQuantum();
	Process tempProcess;
	std::vector<Process> completedProcesses;
	int numProcesses = readyQueue->getSize();
	Queue arrivalQueue;											//make a queue to keep track of when processes are arriving
	for (int count = 0; count < numProcesses; count++)
	{
		arrivalQueue.pushBack(readyQueue->peekFront());
		readyQueue->popFront();
	}
	readyQueue->pushBack(arrivalQueue.peekFront());
	arrivalQueue.popFront();
	clock = readyQueue->peekFront().getArrivalTime();			//initial arrival
	while (readyQueue->isEmpty() == false)
	{
		tempProcess = readyQueue->peekFront();
		if (tempProcess.getProcessState() != 'C')			//i don't think i need this at all...
		{
			//clock = (float)tempProcess.getEventTime();
			RRBeginRun(tempProcess, readyQueue, completedProcesses, arrivalQueue);
		}
	}


	printOutput(completedProcesses, 3);

	completedProcesses.erase(completedProcesses.begin(), completedProcesses.end());		//delete contents of completed vector
}

void Scheduler::runSRTF(Queue* readyQueue)
{
	//need to make an arrival queue, every operation, check arrivalQ.peekFront().getBurstTime() to see if it is less than current running burstRemaining, if it is, context switch
	//if not, add it to ready queue, sort ready queue by burstRemaining, continue as normal.

	//MAKE CHECK CASE TO SEE IF THERE ARE NO PROCESSES IN READY QUEUE, but YES in arrival queue that have not arrived yet
	//setTimeQuantum();
	Process tempProcess;
	std::vector<Process> completedProcesses;
	int numProcesses = readyQueue->getSize();
	Queue arrivalQueue;											//make a queue to keep track of when processes are arriving
	for (int count = 0; count < numProcesses; count++)
	{
		arrivalQueue.pushBack(readyQueue->peekFront());
		readyQueue->popFront();
	}
	readyQueue->pushBack(arrivalQueue.peekFront());
	arrivalQueue.popFront();
	clock = readyQueue->peekFront().getArrivalTime();			//initial arrival
	while (readyQueue->isEmpty() == false || arrivalQueue.isEmpty() == false)
	{
		//sortQueuePriority(readyQueue);		//FIX THIS;  maybe fixed by adding O(2n) to the algorithm//stable sort ready queue by priority then by arrival time
		std::vector<Process> temp;
		int size = readyQueue->getSize();
		for (int count = 0; count < size; count++)
		{
			temp.push_back(readyQueue->peekFront());
			readyQueue->popFront();
		}
		std::stable_sort(temp.begin(), temp.end(), arrivalSort);
		std::stable_sort(temp.begin(), temp.end(), burstRemainingSort);					//arrivals sorted within burstremaining sorted order, if there is a tie in burst remaining order

		for (int count = 0; count < size; count++)
		{
			readyQueue->pushBack(temp[count]);
		}


		if (readyQueue->isEmpty() == true && arrivalQueue.isEmpty() == false)
		{
			readyQueue->pushBack(arrivalQueue.peekFront());				//if the ready queue is empty but something is still hasn't arrived, move to the arrived queue
			arrivalQueue.popFront();
			clock = readyQueue->peekFront().getArrivalTime();				//set the time to the next event if CPU was ever idle
		}
		//else if ready is not empty, move the clock
		
		tempProcess = readyQueue->peekFront();
		SRTFBeginRun(tempProcess, readyQueue, completedProcesses, arrivalQueue);
	}

	printOutput(completedProcesses, 2);

	completedProcesses.erase(completedProcesses.begin(), completedProcesses.end());		//delete contents of completed vector
}

void Scheduler::runPP(Queue* readyQueue)
{
	//setTimeQuantum();
	//same as SRTF except getPriority.
	Process tempProcess;
	std::vector<Process> completedProcesses;
	int numProcesses = readyQueue->getSize();
	Queue arrivalQueue;											//make a queue to keep track of when processes are arriving
	for (int count = 0; count < numProcesses; count++)
	{
		arrivalQueue.pushBack(readyQueue->peekFront());
		readyQueue->popFront();
	}
	readyQueue->pushBack(arrivalQueue.peekFront());
	arrivalQueue.popFront();
	clock = readyQueue->peekFront().getArrivalTime();			//initial arrival
	while (readyQueue->isEmpty() == false || arrivalQueue.isEmpty() == false)
	{
		//sorts the ready queue by priority and then by arrival//this could really be in another function but meh
		std::vector<Process> temp;
		int size = readyQueue->getSize();
		for (int count = 0; count < size; count++)
		{
			temp.push_back(readyQueue->peekFront());
			readyQueue->popFront();
		}
		std::stable_sort(temp.begin(), temp.end(), arrivalSort);
		std::stable_sort(temp.begin(), temp.end(), prioritySort);					//arrivals sorted within priority sorted order, if there is a tie in burst remaining order

		for (int count = 0; count < size; count++)
		{
			readyQueue->pushBack(temp[count]);
		}
		if (readyQueue->isEmpty() == true && arrivalQueue.isEmpty() == false)
		{
			readyQueue->pushBack(arrivalQueue.peekFront());				//if the ready queue is empty but something is still hasn't arrived, move to the arrived queue
			arrivalQueue.popFront();
			clock = readyQueue->peekFront().getArrivalTime();				//set the time to the next event if CPU was ever idle
		}
		//else if ready is not empty, move the clock

		tempProcess = readyQueue->peekFront();
		PPBeginRun(tempProcess, readyQueue, completedProcesses, arrivalQueue);
	}

	printOutput(completedProcesses, 4);

	completedProcesses.erase(completedProcesses.begin(), completedProcesses.end());		//delete contents of completed vector
}

//need to change all instances of bursttime remaining < arrival.peakfront.bursttimereamining to priority
void Scheduler::PPBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed, Queue& arrivalQueue)
{
	readyQueue->popFront();
	Process tempProcess = beginRunning;
	float response=0;
	float remainingBurst=0;
	float waiting=0;

	if (arrivalQueue.isEmpty() == true)					//if the arrival queue is empty, just run this process to completion 
	{
		if (tempProcess.getProcessState() == 'R')
		{
			tempProcess.setBeginProcessing(clock);
			response = (float)tempProcess.getBeginProcessing() - (float)tempProcess.getArrivalTime();
			tempProcess.setResponseTime(response);
			tempProcess.setWaitingTime(response);			//"increment" waiting time
			clock = (float)clock + (float)tempProcess.getCPUBurstRemaining();
			remainingBurst = 0;
			tempProcess.setCPUBurstRemaining(remainingBurst);
			tempProcess.setProcessState('C');
			tempProcess.setCompletionTime(clock);
		}
		else if (tempProcess.getProcessState() == 'W')
		{
			waiting = (float)tempProcess.getWaitingTime() + (float)clock - (float)tempProcess.getCompletionTime();		//the incremental waiting time is waiting time so far + current time - the last time this process ran
			tempProcess.setWaitingTime(waiting);
			clock = (float)clock + (float)tempProcess.getCPUBurstRemaining();											//then complete the process
			remainingBurst = 0;
			tempProcess.setCPUBurstRemaining(remainingBurst);
			tempProcess.setProcessState('C');
			tempProcess.setCompletionTime(clock);
		}
	}
	//if arrival queue is not empty, do a lot more stuff
	else
	{
		float timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
		if (tempProcess.getProcessState() == 'R')					//if the process state is R (the first time it's started processing)
		{
			tempProcess.setBeginProcessing(clock);
			response = (float)tempProcess.getBeginProcessing() - (float)tempProcess.getArrivalTime();
			tempProcess.setResponseTime(response);
			tempProcess.setWaitingTime(response);			//"increment" waiting time
			if (timeToNextArrival >= tempProcess.getCPUBurstTime())		//if there is enough time between now and the next process arriving to complete this process...
			{
				clock = (float)clock + (float)tempProcess.getCPUBurstTime();		//advance clock to CPU burst time if this process can finish;
				remainingBurst = 0;
				tempProcess.setCPUBurstRemaining(remainingBurst);
				tempProcess.setProcessState('C');
				tempProcess.setCompletionTime(clock);
				if (arrivalQueue.isEmpty() == false)
				{
					if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
					{
						readyQueue->pushBack(arrivalQueue.peekFront());
						arrivalQueue.popFront();
					}
				}
			}
			else//if the time to next arrival is less than CPU burst time
			{
				//check the time of the next arrival, advance clock to that time, sort the ready list by PRIORITY
				//if PRIORITY of arrived process is of higher priority (lower number) than here
				clock = (float)arrivalQueue.peekFront().getArrivalTime();									//clock now at arrival time of next process
				remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;
				tempProcess.setCPUBurstRemaining(remainingBurst);
				//NEED TO CHECK ALL ARRIVALS' PRIORITIES DURING THIS BURST TIME REMAINING, if ANY of those are higher than this, then perform context switch AT THAT TIME
				while (tempProcess.getPriority() <= arrivalQueue.peekFront().getPriority() && ((float)tempProcess.getCPUBurstRemaining() + clock) >= (float)arrivalQueue.peekFront().getArrivalTime() && arrivalQueue.getSize()>1)		//if PRIORITY of arrival task is of lower priority than this, try to finish this task, but check each of the next arriving priorities
				{
					readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
					arrivalQueue.popFront();
					timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
					clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
					remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
					if (remainingBurst < 0)																		//if we overshoot the remaining burst, set it to zero and rewind the clock to that time
					{
						clock = (float)clock + (float)remainingBurst;
						remainingBurst = 0;
					}
					tempProcess.setCPUBurstRemaining(remainingBurst);
				}
				if (remainingBurst == 0)
				{
					tempProcess.setCPUBurstRemaining(remainingBurst);
					tempProcess.setProcessState('C');
					tempProcess.setCompletionTime(clock);
					if (arrivalQueue.isEmpty() == false)
					{
						if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
						{
							readyQueue->pushBack(arrivalQueue.peekFront());
							arrivalQueue.popFront();
						}
					}
				}
				else if (tempProcess.getPriority() > arrivalQueue.peekFront().getPriority()) //else if PRIORITY here is lower than the priority of arrived task, perform a context switch
				{
					tempProcess.setProcessState('W');						//set this process's state to waiting
					tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
					int switches;
					switches = tempProcess.getSwitchCounter();
					switches++;
					tempProcess.setSwitchCounter(switches);
					contextSwitchCounter++;
					readyQueue->pushBack(arrivalQueue.peekFront());			//add the arriving process ot the readyQueue
					arrivalQueue.popFront();								//remove it from the arriving queue since it has arrived
					readyQueue->pushBack(tempProcess);						//add the current process to the readyQueue (will be sorted later)
					clock = (float)clock + 0.5;
					if (arrivalQueue.isEmpty() == false)
					{
						if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
						{
							readyQueue->pushBack(arrivalQueue.peekFront());
							arrivalQueue.popFront();
						}
					}
				}
				//if priority is neither lower than arrived process AND if there is still CPU burst time available, keep looping....???
				else if (tempProcess.getPriority() <= arrivalQueue.peekFront().getPriority() && remainingBurst > 0)
				{
					tempProcess.setProcessState('W');
					tempProcess.setCompletionTime(clock);		//temporarily set completion time to clock time
					readyQueue->pushBack(tempProcess);
					readyQueue->pushBack(arrivalQueue.peekFront());
					arrivalQueue.popFront();
				}
			}
		}
		else if (tempProcess.getProcessState() == 'W')			//else if this is the process's second or more time around
		{
			waiting = (float)tempProcess.getWaitingTime() + (float)clock - (float)tempProcess.getCompletionTime();		//the incremental waiting time is waiting time so far + current time - the last time this process ran
			tempProcess.setWaitingTime(waiting);
			clock = (float)arrivalQueue.peekFront().getArrivalTime();									//clock now at arrival time of next process
			remainingBurst = (float)tempProcess.getCPUBurstRemaining() - timeToNextArrival;
			tempProcess.setCPUBurstRemaining(remainingBurst);
			while (tempProcess.getPriority() <= arrivalQueue.peekFront().getPriority() && ((float)tempProcess.getCPUBurstRemaining() + clock) >= (float)arrivalQueue.peekFront().getArrivalTime()&&arrivalQueue.getSize()>1)		//if PRIORITY of arrival task is of lower priority than this, try to finish this task, but check each of the next arriving priorities
			{
				readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
				arrivalQueue.popFront();
				timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
				clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
				remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
				if (remainingBurst < 0)
				{
					clock = (float)clock + (float)remainingBurst;
					remainingBurst = 0;
				}
				tempProcess.setCPUBurstRemaining(remainingBurst);
			}
			if (remainingBurst < 0)
			{
				clock = (float)clock + (float)remainingBurst;
				remainingBurst = 0;
			}
			if (remainingBurst == 0)											//if burst time is done
			{
				tempProcess.setCPUBurstRemaining(remainingBurst);
				tempProcess.setProcessState('C');
				tempProcess.setCompletionTime(clock);
				if (arrivalQueue.isEmpty() == false)
				{
					if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
					{
						readyQueue->pushBack(arrivalQueue.peekFront());
						arrivalQueue.popFront();
					}
				}
			}
			else if (tempProcess.getPriority() > arrivalQueue.peekFront().getPriority())//else if PRIORITY here is lower than the priority of arrived task, perform a context switch
			{
				tempProcess.setProcessState('W');						//set this process's state to waiting
				tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
				int switches;
				switches = tempProcess.getSwitchCounter();
				switches++;
				tempProcess.setSwitchCounter(switches);
				contextSwitchCounter++;
				readyQueue->pushBack(arrivalQueue.peekFront());			//add the arriving process ot the readyQueue
				arrivalQueue.popFront();								//remove it from the arriving queue since it has arrived
				readyQueue->pushBack(tempProcess);						//add the current process to the readyQueue (will be sorted later)
				clock = (float)clock + 0.5;
				if (arrivalQueue.isEmpty() == false)
				{
					if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
					{
						readyQueue->pushBack(arrivalQueue.peekFront());
						arrivalQueue.popFront();
					}
				}
			}
			else if (tempProcess.getPriority() <= arrivalQueue.peekFront().getPriority() && remainingBurst > 0)
			{
				tempProcess.setProcessState('W');
				tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
				readyQueue->pushBack(tempProcess);
				readyQueue->pushBack(arrivalQueue.peekFront());
				arrivalQueue.popFront();
			}
		}
	}
	if (tempProcess.getProcessState() == 'C')			//if the process is completed, push into completed state
		completed.push_back(tempProcess);
}

void Scheduler::RRBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed, Queue& arrivalQueue)
{
	readyQueue->popFront();
	Process tempProcess;
	float response;
	float remainingBurst;
	float waiting;
	tempProcess = beginRunning;
	if (arrivalQueue.isEmpty() == true)
	{
		if (tempProcess.getProcessState() == 'R')
		{
			tempProcess.setBeginProcessing(clock);					//if this is the first time it's beginning to be processed, the beginning process time is the clock time
			response = (float)tempProcess.getBeginProcessing() - (float)tempProcess.getArrivalTime();
			tempProcess.setResponseTime(response);					//set the response time
			tempProcess.setWaitingTime(response);					//"increment" waiting time 
			if (timeQuantum >= tempProcess.getCPUBurstTime())
			{
				clock = (float)clock + (float)tempProcess.getCPUBurstTime();			//if the time quantum is longer than the burst time, advance clock the CPUburst time
				remainingBurst = 0;														//the remaining burst time is 0
				tempProcess.setCPUBurstRemaining(remainingBurst);
				tempProcess.setProcessState('C');										//it is completed
				tempProcess.setCompletionTime(clock);									//set the completion time to the clock
			}
			else//if the time quantum is less than CPUburst time
			{
				remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeQuantum;		//remaining burst is remaining burst - time quantum
				tempProcess.setCPUBurstRemaining(remainingBurst);
				clock = (float)clock + (float)timeQuantum;				//the processing time is the allowed time quantum
				tempProcess.setProcessState('W');
				tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
				if (readyQueue->isEmpty() == true)
					readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
				else
				{
					int switches;
					switches = tempProcess.getSwitchCounter();
					switches++;
					tempProcess.setSwitchCounter(switches);
					contextSwitchCounter++;
					readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
					clock = (float)clock + 0.5;
				}
			}
		}
		else if (tempProcess.getProcessState() == 'W')			//if it is waiting for CPU, then 
		{
			waiting = (float)tempProcess.getWaitingTime() + (float)clock - (float)tempProcess.getCompletionTime();		//the incremental waiting time is waiting time so far + current time - the last time this process ran
			tempProcess.setWaitingTime(waiting);
			if (timeQuantum >= tempProcess.getCPUBurstRemaining())
			{
				clock = (float)clock + (float)tempProcess.getCPUBurstRemaining();			//if the time quantum is longer than the burst time, advance clock the CPUburst time
				remainingBurst = 0;														//the remaining burst time is 0
				tempProcess.setCPUBurstRemaining(remainingBurst);
				tempProcess.setProcessState('C');										//it is completed
				tempProcess.setCompletionTime(clock);									//set the completion time to the clock
			}
			else
			{
				remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeQuantum;		//remaining burst is remaining burst - time quantum
				tempProcess.setCPUBurstRemaining(remainingBurst);
				clock = (float)clock + (float)timeQuantum;				//the processing time is the allowed time quantum
				tempProcess.setProcessState('W');
				tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
				if (readyQueue->isEmpty() == true)
					readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
				else
				{
					int switches;
					switches = tempProcess.getSwitchCounter();
					switches++;											//counting context switches for itself
					tempProcess.setSwitchCounter(switches);
					contextSwitchCounter++;
					readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
					clock = (float)clock + 0.5;
				}
			}
		}
	}
	else//else if arrival queue isn't empty
	{
		float timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
		if (tempProcess.getProcessState() == 'R')					//if the process state is R (the first time it's started processing)
		{
			tempProcess.setBeginProcessing(clock);
			response = (float)tempProcess.getBeginProcessing() - (float)tempProcess.getArrivalTime();
			tempProcess.setResponseTime(response);
			tempProcess.setWaitingTime(response);			//"increment" waiting time
			if (timeToNextArrival > timeQuantum)								//if there's time to finish this time quantum
			{
				if (timeToNextArrival > tempProcess.getCPUBurstRemaining())		//and if time to next arrival greater than CPUBursttime
				{
					if (timeQuantum >= tempProcess.getCPUBurstTime())
					{
						clock = (float)clock + (float)tempProcess.getCPUBurstTime();			//if the time quantum is longer than the burst time, advance clock the CPUburst time
						remainingBurst = 0;														//the remaining burst time is 0
						tempProcess.setCPUBurstRemaining(remainingBurst);
						tempProcess.setProcessState('C');										//it is completed
						tempProcess.setCompletionTime(clock);									//set the completion time to the clock
					}
					else//if the time quantum is less than CPUburst time
					{
						remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeQuantum;		//remaining burst is remaining burst - time quantum
						tempProcess.setCPUBurstRemaining(remainingBurst);
						clock = (float)clock + (float)timeQuantum;				//the processing time is the allowed time quantum
						tempProcess.setProcessState('W');
						tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
						if (readyQueue->isEmpty() == true)
							readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
						else
						{
							int switches;
							switches = tempProcess.getSwitchCounter();
							switches++;
							tempProcess.setSwitchCounter(switches);
							contextSwitchCounter++;
							readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
							clock = (float)clock + 0.5;
						}
					}
				}
				else//if timetonextarrival is more than time quantum, but less than burstremaining, it will finish this time quantum, loop through again
				{
					clock = (float)clock + (float)timeQuantum;				//the processing time is the allowed time quantum
					remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeQuantum;		//remaining burst is remaining burst - time quantum
					tempProcess.setCPUBurstRemaining(remainingBurst);
					tempProcess.setProcessState('W');
					tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
					if (readyQueue->isEmpty() == true)
						readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
					else
					{
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
						int switches;
						switches = tempProcess.getSwitchCounter();
						switches++;
						tempProcess.setSwitchCounter(switches);
						contextSwitchCounter++;
						readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
						clock = (float)clock + 0.5;
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
					}
				}
			}
			else//if timetonextarrival is less than time quantum, check if CPU burst remaining is less than time quantum
			{

				if (timeQuantum >= tempProcess.getCPUBurstTime())					//if cpuburst is less than timeQuantum
				{
					if (tempProcess.getCPUBurstRemaining() <= timeToNextArrival)				//and if cpuburst is less than time to next arrival, finish this process
					{
						clock = (float)clock + (float)tempProcess.getCPUBurstTime();			//if the time quantum is longer than the burst time, advance clock the CPUburst time
						remainingBurst = 0;														//the remaining burst time is 0
						tempProcess.setCPUBurstRemaining(remainingBurst);
						tempProcess.setProcessState('C');										//it is completed
						tempProcess.setCompletionTime(clock);									//set the completion time to the clock
					}
					else//if cpuburst is longer than next arrival, run this cpuburst time while adding every arriving process to the ready queue
					{
						clock = (float)arrivalQueue.peekFront().getArrivalTime();									//clock now at arrival time of next process
						remainingBurst = (float)tempProcess.getCPUBurstRemaining() - timeToNextArrival;
						tempProcess.setCPUBurstRemaining(remainingBurst);
						while (arrivalQueue.getSize() > 1 && clock + tempProcess.getCPUBurstRemaining() >= (float)arrivalQueue.peekFront().getArrivalTime() )
						{
							readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
							arrivalQueue.popFront();
							timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
							clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
							remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
							if (remainingBurst < 0)
							{
								clock = (float)clock + (float)remainingBurst;
								remainingBurst = 0;
							}
							tempProcess.setCPUBurstRemaining(remainingBurst);
						}
					
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
						clock = clock + remainingBurst;
						remainingBurst = 0;
						tempProcess.setCPUBurstRemaining(remainingBurst);
						tempProcess.setProcessState('C');
						tempProcess.setCompletionTime(clock);

					}
				}
				else //if time quantum is LESS than time to next arrival AND CPUburst time, perform context switch at end of time quantum, while adding every arriving process ot the ready queue
				{
					float runningTimeRemaining = (float)timeQuantum;		//amount of time quantum remaining

					while (arrivalQueue.getSize() > 1 && clock + runningTimeRemaining >(float)arrivalQueue.peekFront().getArrivalTime() )
					{
						readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
						arrivalQueue.popFront();
						timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
						clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
						remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
						runningTimeRemaining = runningTimeRemaining - (float)timeToNextArrival;
						if (runningTimeRemaining < 0)
						{
							clock = (float)clock + (float)runningTimeRemaining;
							remainingBurst = remainingBurst - runningTimeRemaining;
							runningTimeRemaining = 0;
						}
						tempProcess.setCPUBurstRemaining(remainingBurst);
					}

					tempProcess.setCPUBurstRemaining((float)tempProcess.getCPUBurstRemaining() - runningTimeRemaining);
					clock = clock + runningTimeRemaining;					//needed thisssssssssss
					tempProcess.setProcessState('W');
					tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
					if (readyQueue->isEmpty() == true)
						readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
					else
					{
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock > (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
						int switches;
						switches = tempProcess.getSwitchCounter();
						switches++;
						tempProcess.setSwitchCounter(switches);
						contextSwitchCounter++;
						readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
						clock = (float)clock + 0.5;
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
					}

				}
			}
		}
		else if (tempProcess.getProcessState() == 'W')
		{
			waiting = (float)tempProcess.getWaitingTime() + (float)clock - (float)tempProcess.getCompletionTime();		//the incremental waiting time is waiting time so far + current time - the last time this process ran
			tempProcess.setWaitingTime(waiting);
			if (timeToNextArrival > timeQuantum)								//if there's time to finish this time quantum
			{
				if (timeToNextArrival > tempProcess.getCPUBurstRemaining())		//and if time to next arrival greater than CPUBursttime
				{
					if (timeQuantum >= tempProcess.getCPUBurstRemaining())
					{
						clock = (float)clock + (float)tempProcess.getCPUBurstRemaining();			//if the time quantum is longer than the burst time, advance clock the CPUburst time
						remainingBurst = 0;														//the remaining burst time is 0
						tempProcess.setCPUBurstRemaining(remainingBurst);
						tempProcess.setProcessState('C');										//it is completed
						tempProcess.setCompletionTime(clock);									//set the completion time to the clock
					}
					else//if the time quantum is less than CPUburst time
					{
						remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeQuantum;		//remaining burst is remaining burst - time quantum
						tempProcess.setCPUBurstRemaining(remainingBurst);
						clock = (float)clock + (float)timeQuantum;				//the processing time is the allowed time quantum
						tempProcess.setProcessState('W');
						tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
						if (readyQueue->isEmpty() == true)
							readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
						else
						{
							if (arrivalQueue.isEmpty() == false)
							{
								if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
								{
									readyQueue->pushBack(arrivalQueue.peekFront());
									arrivalQueue.popFront();
								}
							}
							int switches;
							switches = tempProcess.getSwitchCounter();
							switches++;
							tempProcess.setSwitchCounter(switches);
							contextSwitchCounter++;
							readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
							clock = (float)clock + 0.5;
							if (arrivalQueue.isEmpty() == false)
							{
								if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
								{
									readyQueue->pushBack(arrivalQueue.peekFront());
									arrivalQueue.popFront();
								}
							}
						}
					}
				}
				else//if timetonextarrival is more than time quantum, but less than burstremaining, it will finish this time quantum, loop through again
				{
					clock = (float)clock + (float)timeQuantum;				//the processing time is the allowed time quantum
					remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeQuantum;		//remaining burst is remaining burst - time quantum
					tempProcess.setCPUBurstRemaining(remainingBurst);
					tempProcess.setProcessState('W');
					tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
					if (readyQueue->isEmpty() == true)
						readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
					else
					{
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
						int switches;
						switches = tempProcess.getSwitchCounter();
						switches++;
						tempProcess.setSwitchCounter(switches);
						contextSwitchCounter++;
						readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
						clock = (float)clock + 0.5;
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
					}
				}
			}
			else//if timetonextarrival is less than time quantum, check if CPU burst remaining is less than time quantum
			{

				if (timeQuantum >= tempProcess.getCPUBurstTime())					//if cpuburst is less than timeQuantum
				{
					if (tempProcess.getCPUBurstTime() <= timeToNextArrival)				//and if cpuburst is less than time to next arrival, finish this process
					{
						clock = (float)clock + (float)tempProcess.getCPUBurstRemaining();			//if the time quantum is longer than the burst time, advance clock the CPUburst time
						remainingBurst = 0;														//the remaining burst time is 0
						tempProcess.setCPUBurstRemaining(remainingBurst);
						tempProcess.setProcessState('C');										//it is completed
						tempProcess.setCompletionTime(clock);									//set the completion time to the clock
					}
					else//else, run this cpuburst time while adding every arriving process to the ready queue
					{
						while (clock + tempProcess.getCPUBurstRemaining() >= (float)arrivalQueue.peekFront().getArrivalTime() && arrivalQueue.getSize() > 1)
						{
							readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
							arrivalQueue.popFront();
							timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
							clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
							remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
							if (remainingBurst < 0)
							{
								clock = (float)clock + (float)remainingBurst;
								remainingBurst = 0;
							}
							tempProcess.setCPUBurstRemaining(remainingBurst);
						}
						if (remainingBurst == 0)			//when this burst time is completed, complete this task
						{
							if (arrivalQueue.isEmpty() == false)
							{
								if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
								{
									readyQueue->pushBack(arrivalQueue.peekFront());
									arrivalQueue.popFront();
								}
							}
							tempProcess.setCPUBurstRemaining(remainingBurst);
							tempProcess.setProcessState('C');
							tempProcess.setCompletionTime(clock);

						}
					}
				}
				else //if time quantum is LESS than time to next arrival AND CPUburst time, perform context switch at end of time quantum, while adding every arriving process ot the ready queue
				{
					float runningTimeRemaining = (float)timeQuantum;		//amount of time quantum remaining
					while (clock + runningTimeRemaining >= (float)arrivalQueue.peekFront().getArrivalTime() && arrivalQueue.getSize() > 1)
					{
						readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
						arrivalQueue.popFront();
						timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
						clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
						remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
						runningTimeRemaining = runningTimeRemaining - (float)timeToNextArrival;
						if (runningTimeRemaining < 0)
						{
							clock = (float)clock + (float)runningTimeRemaining;
							remainingBurst = remainingBurst - runningTimeRemaining;
							runningTimeRemaining = 0;
						}
						tempProcess.setCPUBurstRemaining(remainingBurst);
					}
					clock = (float)clock + (float)runningTimeRemaining;
					tempProcess.setCPUBurstRemaining((float)tempProcess.getCPUBurstRemaining() - runningTimeRemaining);
					tempProcess.setProcessState('W');
					tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
					if (readyQueue->isEmpty() == true)
						readyQueue->pushBack(tempProcess);					//if this is the last process left, it will continue running with no context switch cost
					else
					{
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
						int switches;
						switches = tempProcess.getSwitchCounter();
						switches++;
						tempProcess.setSwitchCounter(switches);
						contextSwitchCounter++;
						readyQueue->pushBack(tempProcess);					//if there are other processes left, context switch cost will be incurred
						clock = (float)clock + 0.5;
						if (arrivalQueue.isEmpty() == false)
						{
							if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
							{
								readyQueue->pushBack(arrivalQueue.peekFront());
								arrivalQueue.popFront();
							}
						}
					}
				}
			}
		}
	}
	if (tempProcess.getProcessState() == 'C')			//if the process is completed, push into completed state
		completed.push_back(tempProcess);
}

void Scheduler::SRTFBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed, Queue& arrivalQueue)
{
	readyQueue->popFront();
	Process tempProcess = beginRunning;
	float response = 0;
	float remainingBurst = 0;
	float waiting = 0;

	if (arrivalQueue.isEmpty() == true)					//if the arrival queue is empty, just run this process to completion 
	{
		if (tempProcess.getProcessState() == 'R')
		{
			tempProcess.setBeginProcessing(clock);
			response = (float)tempProcess.getBeginProcessing() - (float)tempProcess.getArrivalTime();
			tempProcess.setResponseTime(response);
			tempProcess.setWaitingTime(response);			//"increment" waiting time
			clock = (float)clock + (float)tempProcess.getCPUBurstRemaining();
			remainingBurst = 0;
			tempProcess.setCPUBurstRemaining(remainingBurst);
			tempProcess.setProcessState('C');
			tempProcess.setCompletionTime(clock);
		}
		else if (tempProcess.getProcessState() == 'W')
		{
			waiting = (float)tempProcess.getWaitingTime() + (float)clock - (float)tempProcess.getCompletionTime();		//the incremental waiting time is waiting time so far + current time - the last time this process ran
			tempProcess.setWaitingTime(waiting);
			clock = (float)clock + (float)tempProcess.getCPUBurstRemaining();											//then complete the process
			remainingBurst = 0;
			tempProcess.setCPUBurstRemaining(remainingBurst);
			tempProcess.setProcessState('C');
			tempProcess.setCompletionTime(clock);
		}
	}
	//if arrival queue is not empty, do a lot more stuff
	else
	{
		float timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
		if (tempProcess.getProcessState() == 'R')					//if the process state is R (the first time it's started processing)
		{
			tempProcess.setBeginProcessing(clock);
			response = (float)tempProcess.getBeginProcessing() - (float)tempProcess.getArrivalTime();
			tempProcess.setResponseTime(response);
			tempProcess.setWaitingTime(response);			//"increment" waiting time
			if (timeToNextArrival >= tempProcess.getCPUBurstTime())		//if there is enough time between now and the next process arriving to complete this process...
			{
				clock = (float)clock + (float)tempProcess.getCPUBurstTime();		//advance clock to CPU burst time if this process can finish;
				remainingBurst = 0;
				tempProcess.setCPUBurstRemaining(remainingBurst);
				tempProcess.setProcessState('C');
				tempProcess.setCompletionTime(clock);
				if (arrivalQueue.isEmpty() == false)
				{
					if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
					{
						readyQueue->pushBack(arrivalQueue.peekFront());
						arrivalQueue.popFront();
					}
				}
			}
			else//if the time to next arrival is less than CPU burst time
			{
				//check the time of the next arrival, advance clock to that time, sort the ready list by REMAINING BURST TIME
				//if REMAINING TIME of arrived process is SHORTER than current process
				clock = (float)arrivalQueue.peekFront().getArrivalTime();									//clock now at arrival time of next process
				remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;
				tempProcess.setCPUBurstRemaining(remainingBurst);
				//NEED TO CHECK ALL ARRIVALS' BURST TIMES DURING THIS BURST TIME REMAINING, if ANY of those are LOWER than this, then perform context switch AT THAT TIME
				while (tempProcess.getCPUBurstRemaining() <= arrivalQueue.peekFront().getCPUBurstTime() && ((float)tempProcess.getCPUBurstRemaining() + clock) >= (float)arrivalQueue.peekFront().getArrivalTime() && arrivalQueue.getSize() > 1)		//if PRIORITY of arrival task is of lower priority than this, try to finish this task, but check each of the next arriving priorities
				{
					readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
					arrivalQueue.popFront();
					timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
					clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
					remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
					if (remainingBurst < 0)
					{
						clock = (float)clock + (float)remainingBurst;
						remainingBurst = 0;
					}
					tempProcess.setCPUBurstRemaining(remainingBurst);
				}
				if (remainingBurst == 0)
				{
					tempProcess.setCPUBurstRemaining(remainingBurst);
					tempProcess.setProcessState('C');
					tempProcess.setCompletionTime(clock);
					if (arrivalQueue.isEmpty() == false)
					{
						if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
						{
							readyQueue->pushBack(arrivalQueue.peekFront());
							arrivalQueue.popFront();
						}
					}
				}
				else if (tempProcess.getCPUBurstRemaining() > arrivalQueue.peekFront().getCPUBurstTime()) //else if remaining burst time here is greater than the arrival's burst time, perform a context switch
				{
					tempProcess.setProcessState('W');						//set this process's state to waiting
					tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
					int switches;
					switches = tempProcess.getSwitchCounter();
					switches++;
					tempProcess.setSwitchCounter(switches);
					contextSwitchCounter++;
					readyQueue->pushBack(arrivalQueue.peekFront());			//add the arriving process ot the readyQueue
					arrivalQueue.popFront();								//remove it from the arriving queue since it has arrived
					readyQueue->pushBack(tempProcess);						//add the current process to the readyQueue (will be sorted later)
					clock = (float)clock + 0.5;
					if (arrivalQueue.isEmpty() == false)
					{
						if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
						{
							readyQueue->pushBack(arrivalQueue.peekFront());
							arrivalQueue.popFront();
						}
					}
				}
				//if priority is neither lower than arrived process AND if there is still CPU burst time available, keep looping....???
				else if (tempProcess.getCPUBurstRemaining() <= arrivalQueue.peekFront().getCPUBurstRemaining() && remainingBurst > 0)
				{
					tempProcess.setProcessState('W');
					tempProcess.setCompletionTime(clock);		//temporarily set completion time to clock time
					readyQueue->pushBack(tempProcess);
					readyQueue->pushBack(arrivalQueue.peekFront());
					arrivalQueue.popFront();
				}
			}
		}
		else if (tempProcess.getProcessState() == 'W')			//else if this is the process's second or more time around
		{
			waiting = (float)tempProcess.getWaitingTime() + (float)clock - (float)tempProcess.getCompletionTime();		//the incremental waiting time is waiting time so far + current time - the last time this process ran
			tempProcess.setWaitingTime(waiting);
			clock = (float)arrivalQueue.peekFront().getArrivalTime();									//clock now at arrival time of next process
			remainingBurst = (float)tempProcess.getCPUBurstRemaining() - timeToNextArrival;
			tempProcess.setCPUBurstRemaining(remainingBurst);
			while (tempProcess.getCPUBurstRemaining() <= arrivalQueue.peekFront().getCPUBurstTime() && ((float)tempProcess.getCPUBurstRemaining() + clock) >= (float)arrivalQueue.peekFront().getArrivalTime() && arrivalQueue.getSize() > 1)		//if PRIORITY of arrival task is of lower priority than this, try to finish this task, but check each of the next arriving priorities
			{
				readyQueue->pushBack(arrivalQueue.peekFront());								//move the arrived process to the ready queue
				arrivalQueue.popFront();
				timeToNextArrival = (float)arrivalQueue.peekFront().getArrivalTime() - clock;				//get the time to the next arrival
				clock = (float)arrivalQueue.peekFront().getArrivalTime();									//advance clock to the NEXT process's arrival time
				remainingBurst = (float)tempProcess.getCPUBurstRemaining() - (float)timeToNextArrival;		//the remaining burst is now lowered
				if (remainingBurst < 0)
				{
					clock = (float)clock + (float)remainingBurst;
					remainingBurst = 0;
				}
				tempProcess.setCPUBurstRemaining(remainingBurst);
			}
			if (remainingBurst == 0)
			{
				tempProcess.setCPUBurstRemaining(remainingBurst);
				tempProcess.setProcessState('C');
				tempProcess.setCompletionTime(clock);
				if (arrivalQueue.isEmpty() == false)
				{
					if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
					{
						readyQueue->pushBack(arrivalQueue.peekFront());
						arrivalQueue.popFront();
					}
				}
			}
			else if (tempProcess.getCPUBurstRemaining() > arrivalQueue.peekFront().getCPUBurstTime()) //else if remaining burst time here is greater than the arrival's burst time, perform a context switch
			{
				tempProcess.setProcessState('W');						//set this process's state to waiting
				tempProcess.setCompletionTime(clock);					//temporarily set the completion time to the clock time; will be updated when the process comes back around
				int switches;
				switches = tempProcess.getSwitchCounter();
				switches++;
				tempProcess.setSwitchCounter(switches);
				contextSwitchCounter++;
				readyQueue->pushBack(arrivalQueue.peekFront());			//add the arriving process ot the readyQueue
				arrivalQueue.popFront();								//remove it from the arriving queue since it has arrived
				readyQueue->pushBack(tempProcess);						//add the current process to the readyQueue (will be sorted later)
				clock = (float)clock + 0.5;
				if (arrivalQueue.isEmpty() == false)
				{
					if (clock >= (float)arrivalQueue.peekFront().getArrivalTime())						//if, at any time, the clock has passed the arrival of the next process, put the next process into the ready Queue
					{
						readyQueue->pushBack(arrivalQueue.peekFront());
						arrivalQueue.popFront();
					}
				}
			}
			//if priority is neither lower than arrived process AND if there is still CPU burst time available, keep looping....???
			else if (tempProcess.getCPUBurstRemaining() <= arrivalQueue.peekFront().getCPUBurstRemaining() && remainingBurst > 0)
			{
				tempProcess.setProcessState('W');
				tempProcess.setCompletionTime(clock);		//temporarily set completion time to clock time
				readyQueue->pushBack(tempProcess);
				readyQueue->pushBack(arrivalQueue.peekFront());
				arrivalQueue.popFront();
			}
		}
	}
	if (tempProcess.getProcessState() == 'C')			//if the process is completed, push into completed state
		completed.push_back(tempProcess);
}



void Scheduler::FCFSBeginRun(Process beginRunning, Queue* readyQueue, std::vector<Process>& completed)
{
	readyQueue->popFront();								//remove the item from the queue
	Process tempProcess;								//create a temp process
	tempProcess = beginRunning;							//set it to the thing we're processing
	tempProcess.setBeginProcessing(clock);				//process begins at the running clock time
	float finishRunTime;								
	float response;										//time between arrival and first running
	response = (float)tempProcess.getBeginProcessing() - (float)tempProcess.getArrivalTime();		
	tempProcess.setResponseTime(response);				//set the response time
	tempProcess.setWaitingTime(response);				//for FCFS, waiting is response time
	finishRunTime = (float)clock + (float)tempProcess.getCPUBurstTime();		//when process completes is begin time + burst time
	clock = (float)finishRunTime;						//the clock is now at this time
	tempProcess.setCompletionTime(finishRunTime);		//the completion time is the finish run time
	tempProcess.setProcessState('C');								//process set to completed
	if (tempProcess.getProcessState() == 'C')			//holds the completed processes.  this is needed for the algorithms with context switching(along with other logic), but I thought I'd add it here so I can cut and paste later
		completed.push_back(tempProcess);				
}

void Scheduler::readProcesses(Queue* readyQueue, std::string filename)
{

	std::cout << "Reading " << filename<< std::endl;
	std::ifstream infile;
	infile.open(filename.c_str());
	std::string readLine;
	while (std::getline(infile, readLine))
	{
		Process tempProcess;
		std::stringstream inputStream(readLine);
		std::string tempString;
		std::vector <int> tempVec;
		while (inputStream >> tempString)
		{
			int tempNum = std::stoi(tempString);
			tempVec.push_back(tempNum);
		}
		tempProcess.setProcessState('R');
		tempProcess.setPID(tempVec[0]);
		tempProcess.setArrivalTime(tempVec[1]);
		tempProcess.setEventTime(tempVec[1]);
		tempProcess.setCPUBurstTime(tempVec[2]);
		tempProcess.setCPUBurstRemaining(tempVec[2]);
		tempProcess.setPriority(tempVec[3]);
		tempVec.erase(tempVec.begin(), tempVec.end());
		readyQueue->pushBack(tempProcess);
	}
	infile.close();
}

void Scheduler::printOutput(std::vector<Process> completed, int choice)
{
	std::vector<Process> output = completed;
	std::sort(output.begin(), output.end(), PIDSort);			//sorts the results by Process ID
	if (choice == 4)
	{
		std::cout << "Algorithm: Pre-emptive Priority" << std::endl;
		std::cout << "Total number of processes run: " << output.size() << std::endl;
	}
	else if (choice == 2)
	{
		std::cout << "Algorithm: Shortest Remaining Time First" << std::endl;
		std::cout << "Total number of processes run: " << output.size() << std::endl;
	}
	else if (choice == 3)
	{
		std::cout << "Algorithm: Round Robin with time quantum of " << timeQuantum << "ms" << std::endl;
		std::cout << "Total number of processes run: " << output.size() << std::endl;
	}
	else if (choice == 1)
	{
		std::cout << "Algorithm: First Come First Served" << std::endl;
		std::cout << "Total number of processes run: " << output.size() << std::endl;
	}
	for (int count = 0; count < output.size(); count++)
	{
		float tempTurnaround = (float)output[count].getCompletionTime() - (float)output[count].getArrivalTime();
		std::cout << std::setw(3) << "PID" << std::setw(9) << "Arrival"
			<< std::setw(7) << "Burst" << std::setw(10) << "Priority"
			<< std::setw(8) << "Finish" << std::setw(9) << "Waiting"
			<< std::setw(12) << "Turnaround" << std::setw(10) << "Response"
			<< std::setw(10) << "Switches" << std::endl;
		std::cout << std::setw(2) << output[count].getPID()
			<< std::setw(7) << output[count].getArrivalTime()
			<< std::setw(8) << output[count].getCPUBurstTime()
			<< std::setw(9) << output[count].getPriority()
			<< std::setw(9) << output[count].getCompletionTime()
			<< std::setw(8) << output[count].getWaitingTime()
			<< std::setw(11) << tempTurnaround
			<< std::setw(11) << output[count].getResponseTime()
			<< std::setw(9) << output[count].getSwitchCounter() << std::endl;
		avgCPUBurstTime = avgCPUBurstTime + (float)output[count].getCPUBurstTime();			//increment all of the processes attributes together
		avgWait = avgWait + (float)output[count].getWaitingTime();
		avgTurnaround = avgTurnaround + tempTurnaround;
		avgResponse = avgResponse + (float)output[count].getResponseTime();
	}
	avgCPUBurstTime = avgCPUBurstTime / (float)output.size();					//calculate all the averages
	avgWait = avgWait / (float)output.size();
	avgTurnaround = avgTurnaround / (float)output.size();
	avgResponse = avgResponse / (float)output.size();
	std::cout << "Average CPU Burst: " << avgCPUBurstTime << "ms" << std::endl;
	std::cout << "Average waiting time: " << avgWait << "ms" << std::endl;
	std::cout << "Average turnaround time: " << avgTurnaround << "ms" << std::endl;
	std::cout << "Average response time: " << avgResponse << "ms" << std::endl;
	std::cout << "Total number of context switches: " << contextSwitchCounter << std::endl << std::endl;
	printOutfile(output, choice);
}

void Scheduler::printOutfile(std::vector<Process> completed, int choice)
{
	std::vector<Process> output = completed;
	std::sort(output.begin(), output.end(), PIDSort);			//sorts the results by Process ID
	std::ofstream outfile;
	std::string filename;
	std::cin.ignore(1);
	std::cout << "Enter an output file name" << std::endl;
	std::getline(std::cin, filename);
	outfile.open(filename.c_str());
	if (choice == 4)
	{
		outfile << "Algorithm: Pre-emptive Priority" << std::endl;
		outfile << "Total number of processes run: " << output.size() << std::endl;
	}
	else if (choice == 2)
	{
		outfile << "Algorithm: Shortest Remaining Time First" << std::endl;
		outfile << "Total number of processes run: " << output.size() << std::endl;
	}
	else if (choice == 3)
	{
		outfile << "Algorithm: Round Robin with time quantum of " << timeQuantum << "ms" << std::endl;
		outfile << "Total number of processes run: " << output.size() << std::endl;
	}
	else if (choice == 1)
	{
		outfile << "Algorithm: First Come First Served" << std::endl;
		outfile << "Total number of processes run: " << output.size() << std::endl;
	}
	for (int count = 0; count < output.size(); count++)
	{
		float tempTurnaround = (float)output[count].getCompletionTime() - (float)output[count].getArrivalTime();
		outfile << std::setw(3) << "PID" << std::setw(9) << "Arrival"
			<< std::setw(7) << "Burst" << std::setw(10) << "Priority"
			<< std::setw(8) << "Finish" << std::setw(9) << "Waiting"
			<< std::setw(12) << "Turnaround" << std::setw(10) << "Response"
			<< std::setw(10) << "Switches" << std::endl;
		outfile << std::setw(2) << output[count].getPID()
			<< std::setw(7) << output[count].getArrivalTime()
			<< std::setw(8) << output[count].getCPUBurstTime()
			<< std::setw(9) << output[count].getPriority()
			<< std::setw(9) << output[count].getCompletionTime()
			<< std::setw(8) << output[count].getWaitingTime()
			<< std::setw(11) << tempTurnaround
			<< std::setw(11) << output[count].getResponseTime()
			<< std::setw(9) << output[count].getSwitchCounter() << std::endl;
		//avgCPUBurstTime = avgCPUBurstTime + (float)output[count].getCPUBurstTime();			//increment all of the processes attributes together
		//avgWait = avgWait + (float)output[count].getWaitingTime();
		//avgTurnaround = avgTurnaround + tempTurnaround;
		//avgResponse = avgResponse + (float)output[count].getResponseTime();
	}
	//avgCPUBurstTime = avgCPUBurstTime / (float)output.size();					//calculate all the averages
	//avgWait = avgWait / (float)output.size();
	//avgTurnaround = avgTurnaround / (float)output.size();
	//avgResponse = avgResponse / (float)output.size();
	outfile << "Average CPU Burst: " << avgCPUBurstTime << "ms" << std::endl;
	outfile << "Average waiting time: " << avgWait << "ms" << std::endl;
	outfile << "Average turnaround time: " << avgTurnaround << "ms" << std::endl;
	outfile << "Average response time: " << avgResponse << "ms" << std::endl;
	outfile << "Total number of context switches: " << contextSwitchCounter << std::endl << std::endl;
	outfile.close();
}

void Scheduler::setTimeQuantum()
{
	int quantum;
	std::cout << "Enter a positive time quantum integer (ms): " << std::endl;
	std::cin >> quantum;
	while (std::cin.fail() || quantum < 1)
	{
		std::cout << "Invalid entry, enter a positive integer for time quantum: " << std::endl;
		std::cin.clear();
		std::cin.ignore(256, '\n');
		std::cin >> quantum;
	}
	timeQuantum = quantum;
}



//fix this
//void Scheduler::sortQueuePriority(Queue* queue)
//{
//	std::vector<Process> temp;
//	int size = queue->getSize();
//	for (int count = 0; count < size; count++)
//	{
//		temp.push_back(queue->peekFront());
//		queue->popFront();
//	}
//	std::stable_sort(temp.begin(), temp.end(), prioritySort);
//	//std::stable_sort(temp.begin(), temp.end(), arrivalSort);
//	for (int count = 0; count < size; count++)
//	{
//		queue->pushBack(temp[count]);
//	}
//}

//void Scheduler::sortQueueBurstRemaining(Queue* queue)
//{
//	std::vector<Process> temp;
//	int size = queue->getSize();
//	for (int count = 0; count < size; count++)
//	{
//		temp.push_back(queue->peekFront());
//		queue->popFront();
//	}
//	std::stable_sort(temp.begin(), temp.end(), burstRemainingSort);
//	//std::stable_sort(temp.begin(), temp.end(), arrivalSort);
//	for (int count = 0; count < size; count++)
//	{
//		queue->pushBack(temp[count]);
//	}
//}