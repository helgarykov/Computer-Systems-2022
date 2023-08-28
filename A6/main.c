#include "memory.h"
#include "cache_sim.h"

int main(int argc, char* argv[]){

    if(argc != 4) {
        printf("%s\n", "Wrong number of arguments:\n
            To run correctly enter: ./cache_sim 'ways' 'sets' 'block size'");
        exit(-1);
    }
    struct memory *mem = memory_create();

    long int counter = cache_sim(argv[1], argv[2], argv[3]);

    memory_delete(mem);

    return 0;
}
