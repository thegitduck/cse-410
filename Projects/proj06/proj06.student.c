// Project 6
// Simulated RAM and Cache Management

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iomanip>
using namespace std;

// Class to hold each line in Cache
class CacheLine
{
public:
	bool V = 0;
	bool M = 0;
	string tag = "00";
	char data[32]; // contents at line
	string hexSlot;
	string address;

	// prints the data elements
	string dataPrint(){ 
		ostringstream os;
		int x = 0;
		while (x < 15){
			os << data[2 * x];
			os << data[2 * x + 1];
			os << " ";
			x++;
		}
		return os.str();
	}

	// print the whole line in cache
	void printCacheLine(){
		cout << "[" << hexSlot << "]: " << V << " " << M\
		<< " " << tag << "   " << dataPrint();
	}

	// constructor sets all data vals to 0 to start
	CacheLine(){
		int x = 0;
		while ( x < 32 )
		{
			data[x] = '0';
			x++;
		}
	}
};


// Line in Random Access Memory
class RamLine
{
public:
	string address;
	char data[32]; // contents at line

	// function to print the data elements stored by line
	string dataPrint(){
		ostringstream os;
		int x = 0;
		while (x < 15){
			os << data[2 * x];
			os << data[2 * x + 1];
			os << " ";
			x++;
		}
		return os.str();
	}

	// prints full line in RAM
	void printRamLine()
	{
		cout << address << " " << dataPrint() << endl;
	}

	// constructs with setting all vals to 0 to start
	RamLine(){
		int x = 0;
		while ( x < 32 )
		{
			data[x] = '0';
			x++;
		}
	}
};

// Helper function to print the cache contents
void printCache(CacheLine* c)
{
	cout << "Contents of the Data Cache:\n";
	cout << " I   V M Tag  Block Contents\n";
	int i = 0;
	while ( i < 16 )
	{
		c[i].printCacheLine();
		cout << endl;
		i++;
	}
	cout << endl << endl;
}

// helper to see if memory address is valid
bool IsMemAddress(int n)
{
	return !(n % 4);
}

bool isNum(string line)
{
    char* p;
    strtol(line.c_str(), &p, 10);
    return *p == 0;
}

int main (int argc, char* argv[])
{
	RamLine RAM[4096];  // create memory objects
	CacheLine CACHE[16];
	int in = 0;
	while ( in < 16 ) // set hex indexes in cache
	{
		stringstream astream;
		astream << hex << in;
		CACHE[in].hexSlot = astream.str();
		in++;
	}

	// these will save number of lines intialized with
	// and where in RAM they are stored
	int linesRead = 0;
	int saveIndex = 0;
	
	if (argc == 4) // see if valid prompt
	{
		// number between each cache output
		int toProcess;
		if (!(isNum((string)argv[1])))
		{
			cout << "Bad value: '" << argv[1] << "' for cache print frequency given\n";
			exit ( 1 );
		} else
		{
			toProcess = stoi(argv[1],nullptr,10);
		}
		
		if (toProcess > 0) printCache(CACHE);
		int numProcessed = 0;

		// Part one init ram
		ifstream initFile;
		initFile.open(argv[2]);
		if (!initFile) {
		  cout << "Unable to open file: " << argv[2] << endl;
		  exit(1);
		}

		string x, s;
		initFile >> s;

		// RAM is indexed by 16 less since each line holds 16 vals
		int indexRAM = stol(s, nullptr, 16) / 16;
		saveIndex = indexRAM;

		// see if valid mem address
		if (!IsMemAddress(stol(s, nullptr, 16)))
		{
			cout << "Failure to read into RAM: Bad Memory address given: " << s;
			cout << endl << endl;
		}

		string readAddress(s);
		int cnt = 0;

		while (initFile >> x) // reads the file into ram
		{
			RAM[indexRAM].data[cnt] = x[0];
			cnt++;
			RAM[indexRAM].data[cnt] = x[1];
			cnt++;
			if (cnt == 32)
			{
				stringstream ss;
				ss << hex << readAddress;
				RAM[indexRAM].address = ss.str();
				cnt = 0;
				indexRAM++;
				linesRead++;
			}
		}
		initFile.close();
		
		// Part two process memory references
		ifstream memRefs;
		memRefs.open(argv[3]);
		if (!memRefs) {
		  cout << "Unable to open file: " << argv[3] << endl;
		  exit(1);
		}

		// process each line in processing file
		string line, instruction, fourBytes;
		while (getline(memRefs, line)) 
		{
			istringstream iss(line);
			iss >> s;
			string address(s);
			indexRAM = stol(s, nullptr, 16) / 16;
			if (!IsMemAddress(stol(s, nullptr, 16)))
			{
				cout << "Failure: Bad Memory address given: " << s << endl << endl;
				continue;
			}

			// initialize all useful variables
			iss >> s;
			instruction = s;
			cnt = 0;
			fourBytes = "";
			string rowHex(address.substr(2,1));
			int rowNum = stol(rowHex, nullptr, 16);
			string offsetStr(address.substr(3,1));
			int offset = stol(offsetStr, nullptr, 16);
			string theTag(address.substr(0,2));
			string hitStatus;
			int a = 0;

			if (instruction == "R") // read operation
			{
				// if cache at desired slot has value and matches tag, HIT
				if (CACHE[rowNum].V && (CACHE[rowNum].tag == theTag))
				{
					hitStatus = "H";
				}
				// either the tag was wrong or the slot was not set MISS
				// copy from ram to cache and set fourBytes 
				else {
					
					if (CACHE[rowNum].M) // see if its not mucked up
					{ // write back
						int ind = stol(CACHE[rowNum].address, nullptr, 16)/16;
						a = 0;
						for (auto ele : CACHE[rowNum].data)
						{
							RAM[ind].data[a] = ele;
							a++;
						}
						CACHE[rowNum].M = 0;
					}

					// copy from ram to cache
					a = 0;
					for (auto ele : RAM[indexRAM].data)
					{
						CACHE[rowNum].data[a] = ele;
						a++;
					}

					CACHE[rowNum].V = 1;
					CACHE[rowNum].M = 0;
					CACHE[rowNum].tag = theTag;
					CACHE[rowNum].hexSlot = rowHex;
					CACHE[rowNum].address = address;
					hitStatus = "M";
				}

				// copy the four bytes from cache to show in output
				cnt = 0;
				while (cnt < 3)
				{
					fourBytes += CACHE[rowNum].data[offset + (cnt*2)];
					fourBytes += CACHE[rowNum].data[offset + (cnt*2) + 1];
					fourBytes += " ";
					cnt++;
				}
				fourBytes += CACHE[rowNum].data[offset + (cnt*2)];
				fourBytes += CACHE[rowNum].data[offset + (cnt*2) + 1];
			} else
			{
				if (CACHE[rowNum].V && (CACHE[rowNum].tag == theTag))
				{
					string tempString;
					int p = 0;

					while (iss >> tempString) // write in new bytes
					{
						CACHE[rowNum].data[offset + p] = tempString[0];
						p++;
						CACHE[rowNum].data[offset + p] = tempString[1];
						p++;
						fourBytes += tempString;
					}

					CACHE[rowNum].M = 1;
					hitStatus = "H";
				}
				else
				{
					if (CACHE[rowNum].M) // see if its not mucked up
					{ // write back
						int ind = stol(CACHE[rowNum].address, nullptr, 16)/16;
						a = 0;
						for (auto ele : CACHE[rowNum].data)
						{
							RAM[ind].data[a] = ele;
							a++;
						}
						CACHE[rowNum].M = 0;
					}

					// copy from ram to cache
					a = 0;
					for (auto ele : RAM[indexRAM].data)
					{
						CACHE[rowNum].data[a] = ele;
						a++;
					}

					CACHE[rowNum].V = 1;
					CACHE[rowNum].M = 0;
					CACHE[rowNum].tag = theTag;
					CACHE[rowNum].hexSlot = rowHex;
					CACHE[rowNum].address = address;

					// write to cache finally
					string tempString;
					int p = 0;
					while (iss >> tempString) // write in new bytes
					{
						CACHE[rowNum].data[offset + p] = tempString[0];
						p++;
						CACHE[rowNum].data[offset + p] = tempString[1];
						p++;
						fourBytes += tempString + " ";
					}
					CACHE[rowNum].M = 1;

					hitStatus = "M";
				}
			}

			cout << "Memory Reference:\n";
			// output the processing
			cout << address << " " << rowHex << " " << hitStatus << " "\
			<< instruction << " " << fourBytes << endl << endl;
			numProcessed++;
			if (numProcessed == toProcess)
			{
				printCache(CACHE);
				numProcessed = 0;
			}
		}
		memRefs.close();
		if ((toProcess > 0) && numProcessed) printCache(CACHE);
	} 
	else if (argc == 3)
	{
		cout << "Invalid input: Missing memory references file\n";
		exit ( 1 );
	}
	else if (argc == 2)
	{
		cout << "Invalid input: Missing ram read file and memory references file\n";
		exit ( 1 );
	}
	// error check initial params
	else {
		cout << "Invalid input: you must give a frequency to print the cache\n\
with an initial contents file and a processing file.\n";
		exit ( 1 );
	}

	/// final output of the RAM
	cout << "Final contents of Initialized RAM:\n";
	int z = 0;
	while (z < linesRead)
	{
		RAM[saveIndex+z].printRamLine();
		z++;
	}
	cout << endl;
	return 0;
}
