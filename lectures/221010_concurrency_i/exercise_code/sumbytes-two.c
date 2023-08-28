#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>

// Structure for a thread
struct worker_payload {
  long* sum; 
  unsigned char* data;        // mmap
  size_t n;                   //size of the file 
  pthread_mutex_t* sum_mutex; // mutex to protect sum

};

void* thread(void* p) {
  struct worker_payload payload = *(struct worker_payload*)p;   //payload struct == the first cell of the array(file) *p points at
    long t_sum = 0;                                             //create a local variable
    for(size_t i = 0; i < payload.n; ++i) {                     //Accessing a global variable in a thread is generally a bad idea
                                                                //if it is required to access global variable by multiple threads, then they should be accessed using a mutex.
      t_sum+= payload.data[i];
    }
    pthread_mutex_lock(payload.sum_mutex);
      *payload.sum += t_sum;                                  //update the global var with the value of the local
    pthread_mutex_unlock(payload.sum_mutex);

  return NULL;
}

int main(int argc, char** argv) {
  assert(argc == 2);
  char *filename = argv[1];

  // Open the given file and determine its size.
  FILE *file = fopen(filename, "r");
  assert(file != NULL);
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // mmap file as an array of 'size' bytes.
  unsigned char *data = mmap(NULL,
                             size,
                             PROT_READ,
                             MAP_PRIVATE,
                             fileno(file), 0);
  assert(data != MAP_FAILED);
  fclose(file);

  long sum = 0;

  // Create a mutex protecting 'sum'.
  pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

  // Launch two threads which receive as payload struct.
  // Thread 1: type 'pthread_t' is an integer used to identify the thread in the system
  pthread_t pthread_1;
  
  // 'data' received by the thread is that of the struct 'worker_payload', the array 'payload'
  struct worker_payload payload_1;

  // Start and end offsets in 'data' indicating which part of the
  // array each thread should traverse.
  payload_1.data = data;      // [0]. element in the array data
  payload_1.n = size/2;       // last element in the array data

  // A pointer to 'sum' it can update.
  payload_1.sum = &sum;

  // A pointer to a mutex protecting 'sum'.
  payload_1.sum_mutex = &sum_mutex;

  //Create a thread: 1.arg  pointer to thread_id ; 2.arg NULL means default attributes 
  // shall be used; 3.arg name of function to be executed for the thread to be created;
  // 4.arg is used to pass arguments to the function Thread (line 18)
  assert(pthread_create(&pthread_1, NULL, thread, &payload_1) == 0);

  // Thread 2
  pthread_t pthread_2;
  struct worker_payload payload_2;
  payload_2.data = &data[size/2];   // pointer to the mid cell in the array 'data'/file
  payload_2.n = size/2;
  payload_2.sum = &sum;
  payload_2.sum_mutex = &sum_mutex;

  assert(pthread_create(&pthread_2, NULL, thread, &payload_2) == 0);

   // Join the two threads.
  assert(pthread_join(pthread_1, NULL) == 0);                       //pthread_join() is equivalent to wait() for processes 
  assert(pthread_join(pthread_2, NULL) == 0);                       //A call to pthread_join blocks the calling thread until 
                                                                    //the thread with identifier equal to the first argument terminates

  printf("Sum: %ld\n", sum);
}

/* Example: 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void* myThreadFun(void* vargp)
{
	sleep(1);             //suspend thread execution for 1 second
	printf("Printing GeeksQuiz from Thread \n");
	return NULL;
}

int main()
{
	pthread_t thread_id;
	printf("Before Thread\n");
	pthread_create(&thread_id, NULL, myThreadFun, NULL);
	pthread_join(thread_id, NULL);
	printf("After Thread\n");
	exit(0);
}
*/

/* All threads share data segment. 
Global and static variables are stored in data segment.
Therefore, they are shared by all threads. 
*/

/* Another Exc
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Let us create a global variable to change it in threads
int g = 0;

// The function to be executed by all threads
void *myThreadFun(void *vargp)
{
	// Store the value argument passed to this thread
	int *myid = (int *)vargp;

	// Let us create a static variable to observe its changes
	static int s = 0;

	// Change static and global variables
	++s; ++g;

	// Print the argument, static and global variables
	printf("Thread ID: %d, Static: %d, Global: %d\n", *myid, ++s, ++g);
}

int main()
{
	int i;
	pthread_t tid;

	// Let us create three threads
	for (i = 0; i < 3; i++)
		pthread_create(&tid, NULL, myThreadFun, (void *)&tid);

	pthread_exit(NULL);
	return 0;
}
*/