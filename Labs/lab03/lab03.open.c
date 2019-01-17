
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main()
{
    int fd;
    int status;

    errno = 0;
    fd = open( "lab03.example", O_CREAT|O_RDONLY, S_IRUSR );

    if (fd == -1)
    {
        printf( "\n open() failed with error [%s]\n", strerror(errno) );
        status = 1;
    }
    else
    {
        printf( "\n open() successful\n" );

        close( fd );
        status = 0;
    }

    return status;
}

