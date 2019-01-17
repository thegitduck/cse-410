// Project 8
// Simulated Memory Management Continued

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

// Class to hold each line in Page Table
class PageTableEntry
{
public:
	bool V = 0;
	bool R = 0;
	bool M = 0;
 
	string frame = "00";
	string hexSlot;

	// print the whole line in table entry
	void print(){
		cout << "[" << hexSlot << "]: " << V << " " << R\
		<< " "  << M << "  " << frame << endl;
	}
};

// class to hold full table with all entries
class PageTable
{
public:
	PageTableEntry entries[16];

	// print the page table
	void print(){
		cout << "Contents of the Page Table:\n";
		cout << " I   V R M  Frame\n";
		cout << "-----------------\n";
		for ( auto ele : entries ){
			ele.print();
		}
		cout << endl;
	}

	// constructs with setting hex indexes
	PageTable(){
		for (int in = 0; in < 16; in++)
		{
			stringstream astream;
			astream << hex << in;
			entries[in].hexSlot = astream.str();
		}
	}
};

// helper to see if memory address is valid
bool IsMemAddress(int n)
{
	return !(n % 4);
}

// helper to convert ints to hex strings for frame numbers
string int_to_hexstring(int num)
{
  stringstream ss;
  ss << setfill ('0') << setw(sizeof(int)/2) 
         << hex << num;
  return ss.str();
}

// helper to test if a string is a number
bool isNum(string line)
{
  char* p;
  strtol(line.c_str(), &p, 10);
  return *p == 0;
}

int main (int argc, char* argv[])
{
	PageTable TABLE; // table
	vector<int> recent;
	vector<int> firt;

	if (argc == 3) // see if valid prompt
	{
		// number between each cache output
		int toProcess;

		if (!(isNum((string)argv[1])))
		{
			cout << "Bad value: '" << argv[1] << "' for page table print frequency given\n";
			exit ( 1 );
		} else
		{
			toProcess = stoi(argv[1],nullptr,10);
		}

		int numProcessed = 0;
		ifstream refFile;
		refFile.open(argv[2]);
		if (!refFile) {
		  cout << "Unable to open file: " << argv[2] << endl;
		  exit(1);
		}

		string str, pageNum, pageOffset, operation, alg, frames;
		int numWrites = 0;
		int numReads = 0;

		// read in mode
		refFile >> alg;
		if (!((alg == "FIFO") || (alg == "LRU")))
		{
			cout << "Bad Replacement algorithm: " << alg << endl;
			cout << "Please use LRU or FIFO.\n";
		  exit(1);
		}

		// read in number of page frames allocated
		refFile >> frames;
		int numFrames = 0;
		if (!(isNum(frames) && (stoi(frames, nullptr, 10) >= 0)))
		{
			cout << "Bad number of frames allocated: " << alg << endl;
			cout << "Please use a positive integer.\n";
		  exit(1);
		} else {
			numFrames = stoi(frames, nullptr,10);
		}

		// allocate frames
		if (numFrames > 16) numFrames = 16;
		for (int i = 0; i < numFrames; i++)
		{
			TABLE.entries[i].frame = int_to_hexstring(64 + i);
			TABLE.entries[i].V = 1;
			firt.push_back(i);
			recent.push_back(i);
		}

		if (toProcess > 0) TABLE.print();

		while(refFile >> str)
		{
			bool isWrite = 0;
			if (!IsMemAddress(stoi(str, nullptr, 16)))
			{
				cout << "Failure to read bad address: " << str;
				cout << endl << endl;
			}

			pageNum = str.substr(0,1);
			pageOffset = str.substr(1,3);
			refFile >> operation;

			// read and write counters
			if (operation == "W")
			{
				numWrites++;
				isWrite = 1;
			} else {
				numReads++;
			}

			int ind = stoi(pageNum, nullptr, 16);

			TABLE.entries[ind].R = 1;

			auto findLoc = find(recent.begin(), recent.end(), ind);

			// list that sorts least recent at front to most recent in back
			if (findLoc != recent.end()) recent.erase(findLoc);

			recent.push_back(ind);

			string fault = "F";
			if (TABLE.entries[ind].V) fault = " ";

			string writeBack = "  ";
			int victim = 0;
			if (fault == "F"){
				// Page fault
				// Select victim based on algorithm
				if (alg == "LRU") {
					victim = recent[0];
					recent.erase(recent.begin());
					recent.push_back(victim);
				} else {
					victim = firt[0];
					firt.erase(firt.begin());
					firt.push_back(victim);
				}

				// copy victim frame and assign new vals necessary
				TABLE.entries[ind].V = 1;
				TABLE.entries[ind].M = 0;
				TABLE.entries[ind].frame = TABLE.entries[victim].frame;

				// write back if victim was mucked up (modified)
				TABLE.entries[victim].V = 0;
				if (TABLE.entries[victim].M) writeBack = "WB";
			}
			
			if (isWrite) TABLE.entries[ind].M = 1;

			// get physical address
			string phys = TABLE.entries[ind].frame + pageOffset;

			cout << "Logical Address referenced: " << str << ", Page Number: " << pageNum 
			<< ", Page Offset: " << pageOffset << ", Operation: " << operation << ", Page Fault: "
			<< fault << ", Write back flag: " << writeBack << ", Physical Address: "
			<< phys << endl << endl;

			numProcessed++;
			if (numProcessed == toProcess)
			{
				TABLE.print();
				numProcessed = 0;
			}
		}

		refFile.close();

		if ((toProcess > 0) && numProcessed) TABLE.print();

		cout << "Replacement algorithm: " << alg << endl;
		cout << "Total Memory References: " << to_string(numWrites + numReads) << endl;
		cout << "Total Reads: " << numReads << endl;
		cout << "Total Writes: " << numWrites << endl << endl;
	}
	else if (argc == 2)
	{
		cout << "Invalid input: Missing memory references file\n";
		exit ( 1 );
	}
	else {
		cout << "Invalid input: you must give a frequency to print the page table\n\
with a memory references file.\n";
		exit ( 1 );
	}
	return 0;
}
