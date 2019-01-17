// Tyler Lawson
// Project Three

using namespace std;
#include <unistd.h> // access uses this
#include <iostream> // Cause cin and cout are cool
#include <cstring>  // for string ops
#include <time.h>   // for time
#include <limits.h> // for PATH_MAX

extern char **environ; // for the environment vars

int main()
{
	// seq is the prompt counter
	int seq = 1;
	// input holds the user input
	char input[128];
	//display the prompt
	cout << "<" << seq << " " << getlogin() << "> ";
	// collect the user input (128 chars long)
	cin.getline(input,128);
	// get first command
	char *arr = strtok(input, " \t");
	// rest will store the next command and the ones after that if needed
	char *rest;
	rest = strtok(NULL, " \t");

	// vars to hold the char* of each valid command
	char quit[] = "quit";
	char date[] = "date";
	char env[] = "env";
	char cwd[] = "cwd";
	char cd[] = "cd";

	// add to the prompt counter
	seq++;

	// var for the time
	time_t t;

	// while not a valid quit command has been called
	while ((strcmp(arr,quit)) || rest)
	{
		if (!strcmp(arr,date)) // date is the command
		{
			if (rest) // if there are extra tokens display the error msg
			{
				cout << "date: invalid extra tokens have been ignored\n";
			}
			// collect the time of the command
			time(&t);
			// display the time nice and pretty
			cout << ctime(&t);
		}
		else if (!strcmp(arr,env)) // env is the command
		{
			if (rest) // if there are extra tokens display the error msg
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
		else if (!strcmp(arr,cwd))
		{
			if (rest) // if there are extra tokens display the error msg
			{
				cout << "cwd: invalid extra tokens have been ignored\n";
			}
			// collect and display the current working dir
			char aDir[PATH_MAX];
			getcwd(aDir, sizeof(aDir));
			cout << aDir << endl;
		}
		// when cd is the command
		else if (!strcmp(arr, cd))
		{
			string dir = ""; // will hold the destination directory
			// if there is more commands than just cd
			if (rest)
			{
				dir = rest;
				rest = strtok(NULL, " \t"); // collect more if there is any

				// there was a third token so display error
				if (rest)
				{
					cout << "cd: Too many arguments.\n";
				}
				// 2 args (cd and a path)
				else {
					// replace the home dir char ~
					if (dir.find("~") == 0 && dir.find("~") < dir.length())
					{
						string userDir = "/user/";
						dir.replace(dir.find("~"), 1, userDir.append(getlogin()));
					}
					// test if dir exists
					if (!access(dir.c_str(), F_OK))
					{
						chdir(dir.c_str());
					}
					// dir doesn't exist
					else {
						cout << dir << ": No such file or directory.\n";
					}
				}
			}
			// cd was the only command so change to home
			else {
				dir = "/user/";
				dir.append(getlogin());
				chdir(dir.c_str());
			}
		}
		else if (!strcmp(arr,quit))
		{
			// Error message modeling the tcsh exit command
			// when you type exit blah blah you get the expression
			// syntax error so I thought it would be good to replicate that
			cout << "quit: Expression Syntax.\n";
		}
		else // wasn't a recognized command so show error replicating tcsh unrecognized command error
		{
			cout << arr << ": Command not found.\n";
		}

		// display the prompt and collect input for next command prompt to repeat the cycle
		cout << "<" << seq << " " << getlogin() << "> ";
		cin.getline(input,128);
		arr = strtok(input, " \t");
		rest = strtok(NULL, " \t");
		seq++;
	}
	return 0;
}
