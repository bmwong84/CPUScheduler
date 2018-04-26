//Bryant Wong
//CSCI 3453 Lab 2
#ifndef QUEUE_
#define QUEUE_

#include "Process.h"


class Queue
{
private:
	//dynamic array containing processes
	Process* queue = nullptr;
	int qSize = 0;


public:
	Queue();
	~Queue();
	bool isEmpty();			//test whether queue is empty
	int getSize()				//returns size of queue
	{
		return qSize;
	}
	Process peekFront();		//access first element
	Process peekBack();		//access last element
	void pushBack(Process addprocess);			//addds element to the back
	void popFront();				//removes element from the front
	Process& operator[](int i)
	{
		return queue[i];
	}


};

#endif