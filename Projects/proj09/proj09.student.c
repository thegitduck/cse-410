// Project 9
// OS process management simulation
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

// Class to store all info for a process
class Process
{
public:
	int ID;
	int priority;
	int numBursts;
	int burstTime;
	int blockedTime;
	int arrivalTime;
	int blockedCnt = 0;
	int burstCnt = 0;
	int numBurstsCnt = 0;
	int cReady = 0;
	int cRunning = 0;
	int cBlocked = 0;

	void operator = (Process p)
	{
		ID = p.ID;
		priority = p.priority;
		numBursts = p.numBursts;
		burstTime = p.burstTime;
		blockedTime = p.blockedTime;
		arrivalTime = p.arrivalTime;
		blockedCnt = p.blockedCnt;
		burstCnt = p.burstCnt;
		numBurstsCnt = p.numBurstsCnt;
		cReady = p.cReady;
		cRunning = p.cRunning;
		cBlocked = p.cBlocked;
	}

	Process(vector<int> v)
	{
		ID = v[0];
		priority = v[1];
		numBursts = v[2];
		burstTime = v[3];
		blockedTime = v[4];
		arrivalTime = v[5];
	}
};

// Helper Function to print all necessary info
void finalReport(Process p, int t)
{
	cout << "Priority: " << p.priority << endl;
	cout << "Number of CPU bursts: " << p.numBursts << endl;
	cout << "Burst time: " << p.burstTime << endl;
	cout << "Blocked time: " << p.blockedTime << endl;
	cout << "Arrival Time: " << p.arrivalTime << endl;
	cout << "Departure Time: " << t << endl;
	cout << "Cumulative Time in Ready: " << p.cReady << endl;
	cout << "Cumulative Time in Running: " << p.cRunning << endl;
	cout << "Cumulative Time in Blocked: " << p.cBlocked << endl;
	cout << "Turnaround Time: " << (t - p.arrivalTime) << endl;
	double d = ((double)(t - p.arrivalTime) / p.cRunning);
	cout << "Normalized Turnaround Time: " << fixed << setprecision(2) << d;
	cout << endl << endl;
}

int main (int argc, char* argv[])
{

	if (argc >= 2)
	{
		if (argc > 2) cout << "proj09: Ignoring extra tokens.\n";

		// Open the File
		ifstream refFile;
		refFile.open(argv[1]);
		if (!refFile) {
		  cout << "Unable to open file: " << argv[1] << endl;
		  exit(1);
		}

		// Get the Length of the simulation in ticks
		string tickStr;
		refFile >> tickStr;
		int ticks = stoi(tickStr);

		// Get the Short-term scheduling algorithm
		string alg;
		refFile >> alg;

		string str;
		vector<Process> New;

		// read all processes into data structure
		while (refFile >> str)
		{
			int cnt = 0;
			vector<int> build;
			while (cnt <= 5)
			{
				build.push_back(stoi(str));
				cnt++;
				if (cnt <= 5) refFile >> str;
			}

			Process p(build);
			New.push_back(p);
		}

		int tick = 0;

		// All of the Process data Structures
		vector<Process> Ready;
		vector<Process> Blocked;
		bool isRunning = false;
		Process Running(vector<int>(6,100));
		vector<Process> Exited;

		// save indexes of processes to erase safely
		vector<int> Temp;

		// Each loop here is a tick
		while (tick < ticks)
		{
			cout << "Current tick: " << tick << endl;

			// Recognize requests from Running processes
			// in last burst
			if (isRunning)
			{
				if ((Running.numBurstsCnt == (Running.numBursts - 1))
				 && (Running.burstCnt == (Running.burstTime - 1)))
				{
					Exited.push_back(Running);
					isRunning = false;
					cout << endl << "PID " << Running.ID << ": Running to Exited\n"; 
					finalReport(Running, tick);
				}
				// this burst done, move to blocked
				else if (Running.burstCnt == (Running.burstTime - 1))
				{
					Running.numBurstsCnt++;
					Running.burstCnt = 0;
					Blocked.push_back(Running);
					isRunning = false;
					cout << "PID " << Running.ID << ": Running to Blocked\n"; 
				}
				// keep running this burst
				else
				{
					Running.burstCnt++;
					Running.cRunning++;
				}
			}
			
			// Recognize Blocked -> Unblocked
			for (unsigned int i = 0; i < Blocked.size(); i++)
			{
				// Move to Ready
				if (Blocked[i].blockedCnt == Blocked[i].blockedTime)
				{
					Blocked[i].blockedCnt = 0;
					Ready.push_back(Blocked[i]);
					Temp.push_back(i);
					cout << "PID " << Blocked[i].ID << ": Blocked to Ready\n";
				}
				// keep blocked
				else
				{
					Blocked[i].blockedCnt++;
					Blocked[i].cBlocked++;
				}
			}
			// delete necessary processes from blocked vector
			for (auto ele : Temp)
			{
				Blocked.erase(Blocked.begin() + ele);
			}
			Temp.clear();

			// Recognize New Processes
			for (unsigned int i = 0; i < New.size(); i++)
			{
				if (tick == New[i].arrivalTime)
				{
					Ready.push_back(New[i]);
					Temp.push_back(i);
					cout << "PID " << New[i].ID << ": New to Ready\n";
				}
			}

			for (auto ele : Temp)
			{
				New.erase(New.begin() + ele);
			}
			Temp.clear();

			// Dispatch a Process Ready to Running
			int minID = 100000;
			int minIndex = 0;
			
			if ((!isRunning) && !(Ready.empty()))
			{
				// loop through to find which one to move to Ready
				for (unsigned int i = 0; i < Ready.size(); i++)
				{
					Ready[i].cReady++;
					if (Ready[i].ID < minID)
					{
						minID = Ready[i].ID;
						minIndex = i;
					}
				}
				// Reassign Ready
				Running = Ready[minIndex];
				Ready.erase(Ready.begin() + minIndex);
				isRunning = true;
				cout << "PID " << Running.ID << ": Ready to Running\n";
			}

			
			// PID num of all Ready
			cout << "Ready PIDs: ";
			if (!(Ready.empty()))
			{
				for (auto ele : Ready)
				{
					cout << ele.ID << " ";
				}
			}
			cout << endl;

			// PID num of Running
			if (isRunning) cout << "Running PID: " << Running.ID << endl;

			// PID num of all Blocked
			cout << "Blocked PIDs: ";
			if (!(Blocked.empty()))
			{
				for (auto ele : Blocked)
				{
					cout << ele.ID << " ";
				}
			}
			cout << endl;
			cout << endl << endl;
			tick++;
		}

	}
	else 
	{
		cout << "proj09: Error: No input file given.\n";
		exit(1);
	}

	return 0;
}
