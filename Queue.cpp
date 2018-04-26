//Bryant Wong
//CSCI 3453 Lab 2

#include "Queue.h"
#include <algorithm>		//for stable sort for priority queue

Queue::Queue()
{
	queue = new Process[qSize];		//creates a new array containing instances of class process to simulate queue
}
Queue::~Queue()
{
	delete[] queue;				//destructor
}

bool Queue::isEmpty()
{
	if (qSize == 0)
		return true;
	else
		return false;
}

Process Queue::peekFront()
{
	Process temp;
	if (qSize == 0)
	{
		std::cout << "Queue is empty" << std::endl;
		return temp;					//returns blank item
	}
	else
		return queue[0];			//returns first item
}
Process Queue::peekBack()
{
	Process temp;			//create blank process
	if (qSize == 0)
	{
		std::cout << "Queue is empty" << std::endl;
		return temp;			//returns blank item
	}
	else
		return queue[qSize - 1];				//returns last item
}
void Queue::pushBack(Process addProcess)
{
	Process* temp = nullptr;							//create new pointer pointing at new array of processes
	temp = new Process[qSize + 1];						//make the new array the size of the queue +1
	for (int count = 0; count < qSize; count++)			//populate the new array with contents of queue array
		temp[count] = queue[count];
	qSize++;											//increase the size counter of the queue array
	temp[qSize - 1] = addProcess;						//make the last item the process we want to add
	delete[] queue;								//delete the original queue array
	queue = temp;									//make the queue the new array by pointing the new pointer at the new array
}

void Queue::popFront()
{
	Process* temp = nullptr;
	temp = new Process[qSize - 1];
	for (int count = 0; count < qSize - 1; count++)
		temp[count] = queue[count + 1];
	qSize--;
	delete[] queue;
	queue = temp;
}

