// Project 5
// Producer/Consumer Problem

#include <pthread.h> // thread fns
#include <semaphore.h> // semaphores
#include <iostream> // standard output
#include <string.h> // string comparisons
#include <sstream> // to build output

using namespace std;

const int buffSize = 20; // buffer size

// Will hold records in a buffer
struct Record {
  long tid;
  int seq;
};

Record buff[buffSize]; // the buffer

sem_t S, N, E; // semaphores needed
int in = 0; // indexes in buffer being used
int out = 0;
long numConsumers = 1; // default vals for consumers and producers
long numProducers = 1;
int iterations = 0; // total iterations in consumers

// produces the sequence numbers and inserts to the buffer
void *producer( void *arg ) 
{
	long tid = (long) pthread_self(); // thread id
	for (int i = 0; i < 100; i++)
	{
		sem_wait(&E);
		sem_wait(&S);
		buff[in].tid = tid; // insert produced items
		buff[in].seq = i;
		in = (in+1) % buffSize;
		if (!(rand() % 8)) sched_yield(); // 12.5% simulated interrupt
		sem_post(&S);
		sem_post(&N);
		if (!(rand() % 4)) sched_yield(); // 25% simulated interrupt
	}
	pthread_exit( NULL ); // exit thread
}

void *consumer( void *arg )
{
	stringstream ss;
	while (iterations < (numProducers * 100))
	{
		iterations++; // add to times through consumer loop
		sem_wait(&N);
		sem_wait(&S);
		// save item to consume
		ss <<  "Thread id " << buff[out].tid << " has the sequence number: "\
		<< buff[out].seq + 1 << endl;
		out = (out + 1) % buffSize;
		if (!(rand() % 8)) sched_yield(); // 12.5% simulated interrupt
		sem_post(&S);
		// consume item
		cout << ss.str();
		ss.str("");
		sem_post(&E);
		if (!(rand() % 4)) sched_yield(); // 25% simulated interrupt
	}
	pthread_exit( NULL ); // exit thread
}

int main(int argc, char* argv[])
{
	char c[] = "-c"; // valid options
	char p[] = "-p";
	int cnt = 0; // count of flags that I have gone through
	long n; // number given with option

	if (argc % 2)
	{
		while (cnt < argc / 2) // loops for each set of options
		{
			if (!strcmp(argv[1+(cnt*2)],c)) // c flag
			{
				n = strtol(argv[2+(cnt*2)], NULL, 10);
				if ((0 < n) &&  (n <= 5)) // check range
				{
					numConsumers = n;
				} else {
					cout << "Error: " << n << " must be in the range of 1-5.\n";
					exit(-1);
				}
			} else if(!strcmp(argv[1+(cnt*2)], p)) // p flag
			{
				n = strtol(argv[2+(cnt*2)], NULL, 10);
				if ((0 < n) &&  (n <= 5)) // check range
				{
					numProducers = n;
				} else {
					cout << "Error: " << n << " must be in the range of 1-5.\n";
					exit(-1);
				}
			} else {
				cout << "Invalid input: you may use options -p and -c with\
 corresponding amounts of producers and consumers\n";
		 exit(-1);
			}
			cnt++;
		}
	} else if (argc != 1) {
		cout << "Invalid input: you may use options -p and -c with\
 corresponding amounts of producers and consumers\n";
		 exit(-1);
	}

	sem_init( &S, 0, 1 ); // initialize semaphores
	sem_init( &N, 0, 0 );
	sem_init( &E, 0, buffSize );

	pthread_t conThreads[numConsumers]; // init threads
	pthread_t prodThreads[numProducers];

	int rc;
  long t;
	// create threads 
	for( t=0; t < numProducers; t++ )
	{
	 rc = pthread_create( &prodThreads[t], NULL, producer, NULL );
	 if (rc)
	 {
	   printf( "ERROR; return code from Producer pthread_create() is %d\n", rc );
	   exit(-1);
	 }
	}

	for( t=0; t < numConsumers; t++ )
	{
	 rc = pthread_create( &conThreads[t], NULL, consumer, NULL );
	 if (rc)
	 {
	   printf( "ERROR; return code from Consumer pthread_create() is %d\n", rc );
	   exit(-1);
	 }
	}

	// join threads when done
	for (int i=0; i < numProducers; i++)
	{
	  if (pthread_join( prodThreads[i], NULL ))
	  {
	    printf( "*** Error joining thread ***\n" );
	    exit( -2 );
	  }
	}

	for (int i=0; i < numConsumers; i++)
	{
	  if (pthread_join( conThreads[i], NULL ))
	  {
	    printf( "*** Error joining thread ***\n" );
	    exit( -2 );
	  }
	}
	
	// get rid of semaphores
	sem_destroy(&S);
	sem_destroy(&N);
	sem_destroy(&E);

	// All done
	exit(0);
	return 0;
}
