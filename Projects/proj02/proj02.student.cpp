// Tyler Lawson
// Project 2

using namespace std;

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>


int title = 0;

void write_title(string cols, char* arg)
{
	string out = cols.append("\n");
	out.append(string(arg).append("\n"));
	out.append(cols.append("\n"));
	const char *outstr = out.c_str();
	write(1, outstr, (cols.size()*4-1));
}

void show_help()
{
	const char* help = "usage: proj02 [option] [filename] ...\n\
	Options and arguments (and corresponding environment variables):\n\
	+t   : display filename before showing file contents\n\
	-t   : do not display filename [default behavior]\n\
	-h   : show help menu\n\
	file : the file to display\n";
	write(1, help, 260);
}

void display_file(int fd)
{
	char buffer[512];
	while (write(1, buffer, read(fd, buffer, 512))){}
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		int fd = 0;
		for(int n = 1; n < argc; n++)
		{
			fd = open(argv[n], O_RDONLY);
			if ((argv[n][0] == '+') || (argv[n][0]=='-'))
			{
				if (strcmp(argv[n],"+t") == 0)
				{
					title = 1;
				}

				else if (strcmp(argv[n],"-h") == 0)
				{
					show_help();
				}

				else if (strcmp(argv[n],"-t") == 0)
				{
					title = 0;
				}

				else
				{
					write(1, "Error: Invalid flag/usage\n", 21);
					show_help();
				}
			}

			else if (fd > 0)
			{
				if (title)
				{
					string colons;
					for (unsigned int i = 0; i < strlen(argv[n]); i++)
					{
						colons.append(":");
					}
					write_title(colons,argv[n]);
				}
				display_file(fd);
			}

			else
			{
				write(1, "Error: Invalid file name/usage\n", 31);
				show_help();
			}
		}
	}
	return 0;
}
