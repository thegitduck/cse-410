// Tyler Lawson
// Project Four

using namespace std;
#include <unistd.h>   // access uses this
#include <iostream>   // Cause cin and cout are cool
#include <cstring>    // for string ops
#include <time.h>     // for time
#include <limits.h>   // for PATH_MAX
#include <sys/wait.h> // for wait()
#include <stdlib.h>
#include <vector>     // vector of string entries as commands
#include <sstream>    // will be used to build the array for execvp

extern char **environ; // for the environment vars

int main()
{
	// seq is the prompt counter
	int seq = 1;
	// allTokens holds the user input
	vector<string> allTokens;
	allTokens.push_back("holder"); // temp val to not be quit

	// var for the time
	time_t t;
	int theSize = 0;

	bool ampFlag = 0;
	// while not a valid quit command has been called
	while (allTokens[0]!="quit" || theSize > 1)
	{
		// reset vars 
		ampFlag = 0;
		allTokens.clear();

		char input[128]; // hold buffer

		//display the prompt
		cout << "<" << seq << " " << getlogin() << "> ";

		// collect the user input (128 chars long)
		cin.getline(input,128);
		
		string theCommandString(input);
		stringstream ss(theCommandString);
		string command;

		// build vector of all tokens
		while (ss >> command)
		{
			allTokens.push_back(command);
		}

		theSize = allTokens.size();

		//check if last token is &
		if (allTokens[theSize-1] =="&")
		{
			ampFlag = 1;
			theSize -= 1;
		}

		// make array to run exec with size for all tokens and a NULL
		char* finalCommandsArr[theSize+1];

		// copy vector into array
		for(int position = 0; position < theSize; position++)
		{
			finalCommandsArr[position] = (char*)allTokens[position].c_str();
		}

		//append NULL to vector to make execvp happy
		finalCommandsArr[theSize] = NULL;

		// if no input go back to the top of the while and reprompt
		if (!theSize) continue;
		
		// add to the prompt counter
		seq++;

		if (allTokens[0]=="date") // date is the command
		{
			if (theSize>1) // if there are extra tokens display the error msg
			{
				cout << "date: invalid extra tokens have been ignored\n";
			}
			// collect the time of the command
			time(&t);
			// display the time nice and pretty
			cout << ctime(&t);
		}
		else if (allTokens[0]=="env") // env is the command
		{
			if (theSize>1) // if there are extra tokens display the error msg
			{
				cout << "env: invalid extra tokens have been ignored\n";
			}
			int i = 0;
			while (environ[i] != NULL) // display the environment vars
			{
				printf( "%s\n", environ[i] );
				i++;
			}
		}
		else if (allTokens[0]=="cwd")
		{
			if (theSize>1) // if there are extra tokens display the error msg
			{
				cout << "cwd: invalid extra tokens have been ignored\n";
			}
			// collect and display the current working dir
			char aDir[PATH_MAX];
			getcwd(aDir, sizeof(aDir));
			cout << aDir << endl;
		}
		// when cd is the command
		else if (allTokens[0]=="cd")
		{
			string dir = ""; // will hold the destination directory
			// if there is more commands than just cd
			if (theSize>1)
			{
				dir = allTokens[1];

				// there was a third token so display error
				if (theSize>2)
				{
					cout << "cd: Too many arguments.\n";
				}
				// 2 args (cd and a path)
				else {
					// replace the home dir char ~
					string userDir = "/user/";
					if (dir == "~")
					{
						dir.replace(dir.find("~"), 1, userDir.append(getlogin()));
					}
					// ~DIR
					else if (dir.find("~/") == 0 && dir.find("~/") < dir.length())
					{
						string aUser(getlogin());
						dir.replace(dir.find("~/"), 2, userDir.append(aUser.append("/")));
					}
					//~USER
					else if (dir.find("~") == 0 && dir.find("~") < dir.length())
					{
						dir = userDir.append(dir.substr(1,string::npos));
					}

					// test if dir exists
					if (!access(dir.c_str(), F_OK))
					{
						chdir(dir.c_str());
					}
					// dir doesn't exist
					else 
					{
						cout << dir << ": No such file or directory.\n";
					}
				}
			}
			// cd was the only command so change to home
			else 
			{
				dir = "/user/";
				dir.append(getlogin());
				chdir(dir.c_str());
			}
		}
		else if (allTokens[0]=="quit" && (theSize>1))
		{
			// Error message modeling the tcsh exit command
			// when you type exit blah blah you get the expression
			// syntax error so I thought it would be good to replicate that
			cout << "quit: Expression Syntax.\n";
		}
		else if (allTokens[0]!="quit") // wasn't an internal command so run exec process
		{
			int pid, flag;
			
			pid = fork();

			if (pid < 0)
			{
			  flag = -1;
			}
			else if (pid > 0)
			{
				if(!ampFlag) // check for last token &
				{
					waitpid( pid, NULL, 0 );
			  	flag = 0;
				}
			}
			else
			{
				flag = execvp(finalCommandsArr[0], finalCommandsArr); // execute the command
				if (flag == -1) // if error return the error msg
				{
					cout << allTokens[0] << ": Command not found.\n";
				}
			 	exit( flag ); // exit the process
			}
		}
		memset(&finalCommandsArr[0], 0, sizeof(finalCommandsArr)); // clear the array memory
	}
	return 0;
}
