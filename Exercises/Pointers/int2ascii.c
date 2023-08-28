#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<errno.h>

/// @brief Converting binary integers to ASCII integers
int read_int(FILE* f, int* out){

    int e;
    
    e = fread(out, sizeof(int), 1, f);
    if(e == 1) return 0;    // freads returns no errors
    return 1;               // read_int returns errors
}

int main()
{
    FILE* f = fopen("int.txt", "r");

    assert(f != NULL);

    int asciiNumber, errorMessage;
    
    while(1)
    {
        errorMessage = read_int(f, &asciiNumber);
        printf("Int val (%d) corresponds to ascii - %c\n", asciiNumber, asciiNumber);
        if(errorMessage == 1) break;

    }
    return 0;
}
