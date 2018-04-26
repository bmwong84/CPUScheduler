*******************************************************
*  Name      :      Bryant Wong    
*  Student ID:      107571009           
*  Class     :  CSC 3453         
*  HW#       :  Lab 2                
*  Due Date  :  Oct. 30, 2017
*******************************************************


                 Read Me


*******************************************************
*  Description of the program
*******************************************************

This program simulates a CPU scheduler running 4 different algorithms:
First come first served, Round Robin, Shortest Remaining Time First, 
and Priority.  Round Robin also has the option to set the desired time quantum.
Statistics reported for each process: response time, turnaround time, priority,
arrival time, waiting time, number of context switches, and CPU burst time..
Statistics reported for the input: average CPU burst time, average waiting time,
average turnaround time, average response time and total number of context switches.
The input file should be named input.txt

*******************************************************
*  Source files
*******************************************************

Name:  main.cpp
   Main program.  This has member function calls that contains all of the actual code.

Name:  Process.h
   Holds the class Process to simulate the process control block.

Name: Scheduler.h
   Contains class Scheduler.  Has declarations for all of the algorithms as well as a menu.

Name: Scheduler.cpp
   Contains all of the actual coding and algorithms, the heavy stuff.

Name Queue.h
   Contains the declarations for the dynamic array class that simulates a queue.

Name Queue.cpp
   Contains the implementation functions to make the dynamic array act as a queue.
   
   
*******************************************************
*  Circumstances of programs
*******************************************************

Compiles and runs on csegrid.

Developed on Visual Studio Community 2017 version 15.3.2.  


*******************************************************
*  How to build and run the program
*******************************************************


    You should see a directory named homework with the files:
        main.cpp
	Queue.cpp
	Queue.h
	Scheduler.cpp
	Scheduler.h
	Process.h
        makefile
	input.txt
        Readme.txt
	Report.txt	(contains sample outputs)

    Compile the program by:
    % make

Run the program by:
   % ./lab2

Delete the obj files, executables, and core dump by
   %./make clean
