// Project 7
// Simulated Memory Management

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iomanip>
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

		if (toProcess > 0) TABLE.print();

		int numProcessed = 0;
		ifstream refFile;
		refFile.open(argv[2]);
		if (!refFile) {
		  cout << "Unable to open file: " << argv[2] << endl;
		  exit(1);
		}

		string str, pageNum, pageOffset, operation;
		int numWrites = 0;
		int numReads = 0;
		while(refFile >> str)
		{
			if (!IsMemAddress(stol(str, nullptr, 16)))
			{
				cout << "Failure to read bad address: " << str;
				cout << endl << endl;
			}

			pageNum = str.substr(0,1);
			pageOffset = str.substr(1,3);
			refFile >> operation;

			if (operation == "W")
			{
				numWrites++;
			} else {
				numReads++;
			}

			cout << "Address referenced: " << str << ", Page Number: " << pageNum << ", Page Offset: "\
			<< pageOffset << ", Operation: " << operation << endl << endl;

			numProcessed++;
			if (numProcessed == toProcess)
			{
				TABLE.print();
				numProcessed = 0;
			}
		}

		refFile.close();
		if ((toProcess > 0) && numProcessed) TABLE.print();

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
