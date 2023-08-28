#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>


// Add fields here.
struct worker_payload {
  long* sum;
   size_t start;
   size_t end;
   unsigned char* data;
   pthread_mutex_t* sum_mutex;
};

void* thread(void* p) {
  struct worker_payload work = *(struct worker_payload*)p;

  long local_sum = 0;

  for (size_t i = work.start; i < work.end; i++) {
    local_sum += work.data[i];
  }

  pthread_mutex_lock(work.sum_mutex);
  *work.sum += local_sum;
  pthread_mutex_unlock(work.sum_mutex);

  return NULL;
}

int main(int argc, char** argv) {
  assert(argc == 3);
  int nthreads = atoi(argv[1]);
  char *filename = argv[2];

  // Open the given file and determine its size.
  FILE* file = fopen(filename, "r");
  assert(file != NULL);
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // mmap file as an array of 'size' bytes.
  //mmap -- allocate memory, or map files or devices into memory (simil to malloc)
  unsigned char *data = mmap(NULL,
                             size,
                             PROT_READ,
                             MAP_PRIVATE,
                             fileno(file), 0);
  assert(data != MAP_FAILED);
  fclose(file);

  pthread_t ptids[nthreads];
  struct worker_payload payloads[nthreads];
  int chunk_size = (size + nthreads - 1) / nthreads;
  long sum = 0; // TODO: also create mutex.

  // Create 'nthreads' threads.  Thread 'i' should sum from
  // data[i*chunk_size] to data[(i+1)*chunk_size] - but note that the
  // last thread may read out-of-bounds if the file size is not
  // divisible by the chunk size!  Make sure you avoid this by making
  // the chunk size smaller for the last thread.
  pthread_t ptids[nthreads];                      //initialize an array of threads
  struct worker_payload payloads[nthreads];       //struct array of size = nthreads
  int chunk_size = (size + nthreads - 1) / nthreads; // 3+3/3=2
  long sum = 0;
  pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;  //pthread_mutex_t is of a type pointer

   // Initialise payloads[i] and launch thread 'i'.
  for (int i = 0; i < nthreads; i++) {
    payloads[i].data = data;
    payloads[i].start = i * chunk_size;
    payloads[i].end = (i+1) * chunk_size;
    payloads[i].sum = &sum;
    payloads[i].sum_mutex = &sum_mutex;
    if (payloads[i].end > size) {
      payloads[i].end = size;
    }

    assert(pthread_create(&ptids[i], NULL, thread, &payloads[i]) == 0);
  }
  // Wait for thread 'i' to finish.
  for (int i = 0; i < nthreads; i++) {
     assert(pthread_join(ptids[i], NULL) == 0);
  }

  printf("Sum: %ld\n", sum);
}

/*Thread synchronization is defined as a mechanism which ensures 
that two or more concurrent processes or threads do not simultaneously
execute some particular program segment known as a critical section.

When one thread starts executing the critical section (a serialized segment 
of the program) the other thread should wait until the first thread finishes.

If proper synchronization techniques are not applied, it may cause a race 
condition (one thread tries to change the value of shared data at the same 
time as another thread tries to read the value) where the values of 
variables may be unpredictable and vary 
depending on the timings of context switches of the processes or threads.

 A critical section is a group of instructions/statements or region of 
 code that need to be executed atomically
*/