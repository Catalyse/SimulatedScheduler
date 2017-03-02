#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdlib>

using namespace std;

struct process//This is the PCB
{
	int pid;
	int arrival;
	int burstTime;
	float startTime;
	float finishTime;
	int curProcessed;
	int remaining;
	int queue;
	int switches;
	bool complete;
	bool arrived;
};

//Header Stuff
void FCFS();
void SRTF();
void RR(int quantum);
vector<process> ReadInputFile();
void ScreenWrite(string, vector<process>);
//End Header Stuff

char* fileName;

int main(int argc, char* argv[])
{
	//cout << "Entered Main" << endl;
	if(argc)
	{
		if(atoi(argv[2]) == 0)
		{
			fileName = argv[1];
			FCFS();
		}
		else if(atoi(argv[2]) == 1)
		{
			fileName = argv[1];
			SRTF();
		}
		else if(atoi(argv[2]) == 2 && argc > 3)
		{
			fileName = argv[1];
			RR(atoi(argv[3]));
		}
		else
		{
			cout << "Invalid command please try again" << endl;
		}
	}
	else
	{
		cout << "Invalid command please try again" << endl;
	}
}

vector<process> ReadInputFile()
{
	vector<process> group;
	int iterator = 0;
	int currentSize = 10;
	ifstream file;
	cout << "Reading File: " << fileName << endl;
	file.open(fileName);
	if(file.is_open())
	{
		int read;
		while(file >> read)
		{
			process temp;
			temp.pid = read;
			file >> read;
			temp.arrival = read;
			file >> read;
			temp.burstTime = read;
			temp.startTime = -1;
			temp.finishTime = -1;
			temp.curProcessed = 0;
			temp.remaining = read;
			temp.switches = 0;
			temp.complete = false;
			group.push_back(temp);
		}
		cout << "Read Complete" << endl;
		return group;
	}
	else
	{
		cout << "File not found" << endl;
		return group;
	}
}

void FCFS()
{
	bool running = true;
	bool allArrived = false;
	float moment = 0.0;
	vector<process> masterList = ReadInputFile();
	if(masterList.size() < 1)
		return;
	int size = masterList.size();
	int current = -1;
	int queueSize = 0;
	int arrivalCount = 0;
	cout << "FCFS Setup Complete Starting Simulation" << endl;
	while(running)
	{
		//Setup and status check
		if(arrivalCount < size)//Skip this check if all have arrived.
		{
			for(int i = 0; i < size; i++)//Check for new arrivals
			{
				if(masterList[i].arrived != true)
				{
					if(masterList[i].arrival <= moment)
					{
						if(queueSize == 0)
						{
							current = i;
							queueSize++;
							arrivalCount++;
							masterList[i].arrived = true;
						}
						else
						{
							masterList[i].queue = queueSize;
							queueSize++;
							arrivalCount++;
							masterList[i].arrived = true;
						}
					}
				}
			}
		}
		else
		{
			allArrived = true;
		}
		//cout << "Processing Moment: " << moment << endl;
		//Processing
		if(current != -1)
		{//Skip this if we have nothing running
			if(masterList[current].startTime == -1)
			{
				masterList[current].startTime = moment;
			}
			masterList[current].curProcessed++;
			//cout << "Burst: " << masterList[current].burstTime << endl << "Currently Processed: " << masterList[current].curProcessed << endl;
			masterList[current].remaining = masterList[current].burstTime - masterList[current].curProcessed;
			//cout << "Remaining: " << masterList[current].remaining << endl;
			
			moment += 1;
			if(masterList[current].remaining == 0)
			{//if process complete
				masterList[current].complete = true;
				masterList[current].queue = -1;//This is so that we dont pick this again
				masterList[current].finishTime = moment;
				queueSize--;
				for(int i = 0; i < size; i++)
				{
					if(!masterList[i].complete)
					{
						masterList[i].queue--;
					}
				}
				current = -1;
				for(int i = 0; i < size; i++)
				{
					if(masterList[i].queue == 0)
					{
						current = i;
						break;
					}
				}
				if(current == -1 && allArrived)
				{
					running = false;//completed sim
				}
			}
		}
		else
		{
			moment += 1;
		}
	}
	ScreenWrite("FCFS", masterList);
}

void SRTF()
{
	bool running = true;
	bool allArrived = false;
	bool noJob = false;
	float moment = 0.0;
	vector<process> masterList = ReadInputFile();
	if(masterList.size() < 1)
		return;
	vector<int> queue;
	int size = masterList.size();
	int current = -1;
	int queueSize = 0;
	int arrivalCount = 0;
	cout << "SRTF Setup Complete Starting Simulation" << endl;
	while(running)
	{
		//Setup and status check
		if(arrivalCount < size)//Skip this check if all have arrived.
		{
			for(int i = 0; i < size; i++)//Check for new arrivals and reorder the queue
			{
				if(masterList[i].arrived != true)
				{
					if(masterList[i].arrival <= moment)
					{
						if(queueSize == 0)
						{
							noJob = false;
							current = i;
							queue.push_back(i);
							queueSize++;
							arrivalCount++;
							masterList[i].queue = 0;
							masterList[i].arrived = true;
							//cout << masterList[i].pid << " inserted at start" << endl;
						}
						else
						{
							bool inserted = false;
							for(int j = 0; j < queue.size(); j++)
							{
								if(masterList[i].remaining < masterList[queue[j]].remaining)
								{
									//cout << masterList[i].pid << " inserted at " << j << endl;
									inserted = true;
									queue.insert(queue.begin()+j, i);
									break;
								}
							}
							if(!inserted)
							{
								queue.push_back(i);
								//cout << masterList[i].pid << " inserted at end"<< endl;
							}
							queueSize++;
							arrivalCount++;
							masterList[i].arrived = true;
						}
					}
				}
			}
		}
		else
		{
			allArrived = true;
		}
		//Checking if queue has shifted the current job
		if(!noJob && queue[0] != current && masterList[current].queue != -1)
		{
			//cout << "Context switching ------------------------------------" << endl;
			masterList[current].switches++;
			current = queue[0];
			moment += 0.5;//context switch
		}
		//cout << "Processing Moment: " << moment << endl;
		//Processing
		if(current != -1)
		{//Skip this if we have nothing running
			if(masterList[current].startTime == -1)
			{
				masterList[current].startTime = moment;
			}
			masterList[current].curProcessed++;
			//cout << "Burst: " << masterList[current].burstTime << endl << "Currently Processed: " << masterList[current].curProcessed << endl;
			masterList[current].remaining = masterList[current].burstTime - masterList[current].curProcessed;
			//cout << "Remaining: " << masterList[current].remaining << endl;
			
			moment += 1;
			if(masterList[current].remaining <= 0)
			{//if process complete
				masterList[current].complete = true;
				masterList[current].queue = -1;//This is so that we dont pick this again
				masterList[current].finishTime = moment;
				queueSize--;
				queue.erase(queue.begin());
				//cout << "Job Complete" << endl;
				if(queue.size() > 0)
				{
					current = queue[0];
				}
				else
				{
					current = -1;
					noJob = true;
				}
				if(current == -1 && allArrived)
				{
					running = false;//completed sim
				}
			}
		}
		else if(current == -1 && allArrived)
		{
			running = false;//completed sim
		}
		else
		{
			moment += 1;
		}
	}
	ScreenWrite("SRTF", masterList);
}

void RR(int quantum)
{
	bool running = true;
	bool allArrived = false;
	float moment = 0.0;
	int quantCount = 0;//This counts how far into the quantum a process is.
	vector<process> masterList = ReadInputFile();
	if(masterList.size() < 1)
		return;
	int size = masterList.size();
	int current = -1;
	int queueSize = 0;
	int arrivalCount = 0;
	cout << "Round Robin Setup Complete Starting Simulation - Quantum: " << quantum << endl;
	while(running)
	{
		//Setup and status check
		if(arrivalCount < size)//Skip this check if all have arrived.
		{
			for(int i = 0; i < size; i++)//Check for new arrivals
			{
				if(masterList[i].arrived != true)
				{
					if(masterList[i].arrival <= moment)
					{
						if(queueSize == 0)
						{
							current = i;
							queueSize++;
							arrivalCount++;
							quantCount = 0;
							masterList[i].arrived = true;
						}
						else
						{
							masterList[i].queue = queueSize;
							queueSize++;
							arrivalCount++;
							masterList[i].arrived = true;
						}
					}
				}
			}
		}
		else
		{
			allArrived = true;
		}
		//cout << "Processing Moment: " << moment << endl;
		//Processing
		if(current != -1)
		{//Skip this if we have nothing running
			if(masterList[current].startTime == -1)
			{
				masterList[current].startTime = moment;
			}
			masterList[current].curProcessed++;//push the processed count
			quantCount++;
			moment++;
			//cout << "Process PID: " << masterList[current].pid << endl;
			masterList[current].remaining = masterList[current].burstTime - masterList[current].curProcessed;
			//cout << "Remaining: " << masterList[current].remaining << endl;
			
			if(masterList[current].remaining == 0)
			{//if process complete
				masterList[current].complete = true;
				masterList[current].queue = -1;//This is so that we dont pick this again
				masterList[current].finishTime = moment;
				queueSize--;
				for(int i = 0; i < size; i++)
				{
					if(!masterList[i].complete)
					{
						masterList[i].queue--;
					}
				}
				current = -1;
				for(int i = 0; i < size; i++)
				{
					if(masterList[i].queue == 0)
					{
						quantCount = 0;
						current = i;
						break;
					}
				}
				if(current == -1 && allArrived)
				{
					running = false;//completed sim
				}
			}
			else if(quantCount >= quantum)//TIMES UP
			{
				//I would like to note that I did not really want to copy paste this extra bit in here
				//But I also did not want to make it into a method because that would have been a lot 
				//More effort and made this problem harder.
				if(arrivalCount < size)//Skip this check if all have arrived.
				{
					for(int i = 0; i < size; i++)//Check for new arrivals
					{
						if(masterList[i].arrived != true)
						{
							if(masterList[i].arrival <= moment)
							{
								if(queueSize == 0)
								{
									current = i;
									queueSize++;
									arrivalCount++;
									quantCount = 0;
									masterList[i].arrived = true;
								}
								else
								{
									masterList[i].queue = queueSize;
									queueSize++;
									arrivalCount++;
									masterList[i].arrived = true;
								}
							}
						}
					}
				}
				else
				{
					allArrived = true;
				}
				masterList[current].queue = queueSize;
				masterList[current].switches++;
				quantCount = 0;
				for(int i = 0; i < size; i++)
				{
					if(!masterList[i].complete)
					{
						masterList[i].queue--;
						if(masterList[i].queue == 0)
							current = i;
					}
				}
				moment += 0.5;
			}
		}
		else
		{
			moment += 1;
		}
	}
	string temp = "Round Robin";
	ScreenWrite(temp, masterList);
}

void ScreenWrite(string algo, vector<process> list)
{
	int procCount = list.size();
	int burstAvg = 0;
	int turnAvg = 0;
	int respAvg = 0;
	int waitAvg = 0;
	int switches = 0;
	cout << "*********************************************************************"<<endl;
	cout << "** " << algo << endl;
	cout << "*********************************************************************"<<endl;
	cout << " ___________________________________________________________________" <<endl;
	cout << "|pid | arrival | burst | finish | waiting| turn- | response | No.of |" <<endl;
	cout << "|    |         |       |  time  |  time  | around|   time   |Context|" <<endl;
	cout << "|____|_________|_______|________|________|_______|__________|_______|" <<endl;
	for(int i = 0; i < list.size(); i++)
	{
		cout << "| " << left << setw(3) << setfill(' ') << list[i].pid;
		cout << "| " << left << setw(8) << setfill(' ') << list[i].arrival;
		burstAvg += list[i].burstTime;
		cout << "| " << left << setw(6) << setfill(' ') << list[i].burstTime;
		cout << "| " << left << setw(7) << setfill(' ') << list[i].finishTime;
		int wait = list[i].finishTime - list[i].arrival - list[i].burstTime;
		waitAvg += wait;
		cout << "| " << left << setw(7) << setfill(' ') << wait;//wait
		int turn = list[i].finishTime - list[i].arrival;
		turnAvg += turn;
		cout << "| " << left << setw(6) << setfill(' ') << turn;//turnaround
		int resp = list[i].startTime - list[i].arrival;
		respAvg += resp;
		cout << "| " << left << setw(9) << setfill(' ') << resp;
		switches += list[i].switches;
		cout << "| " << left << setw(6) << setfill(' ') << list[i].switches << "|" << endl;
	}
	cout << "|___________________________________________________________________|" << endl << endl;
	cout << "Average CPU Burst Time: " << (burstAvg/procCount) << endl;
	cout << "Average Wait Time: " << (waitAvg/procCount) << endl;
	cout << "Average TurnAround Time: " << (turnAvg/procCount) << endl;
	cout << "Average Response Time: " << (respAvg/procCount) << endl;
	cout << "Total Context Switches: " << switches << endl;
}