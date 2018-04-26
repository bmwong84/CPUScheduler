//Bryant Wong
//CSCI 3453 Lab 2
#include "Scheduler.h"




int main()

{
	Queue readyQueue;
	Scheduler schedulerSim;
	schedulerSim.menu(&readyQueue);
	std::cin.get();
	return 0;
}