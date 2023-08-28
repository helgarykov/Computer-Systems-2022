#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<errno.h>

int main(int argc, char** argv) 
{

    FILE* fileOfHex =fopen(argv[1], "r");

    assert(fileOfHex != NULL);

    unsigned char readFile;
    int e;

    while(1)
    {
         e = fread(&readFile, 1, 1, fileOfHex);
         if(e == 0)
         {
            break;
         }

         printf("Byte (%.2x) \t symbol %c\n", readFile, readFile);

          //e = fwrite(&readFile, 1, 1, stdout);
          //assert(e == 1);

    }

    assert(fclose(fileOfHex) == 0);

    return 0;

}

/*
int main(int argc, char** argv) 
{

    FILE* fileOfHex =fopen(argv[1], "r");

    assert(fileOfHex != NULL);

    unsigned char readFile;
    int e;

    while(1)
    {
         e = fread(&readFile, 1, 1, fileOfHex);
         if(e == 0)
         {
            break;
         }

          e = fwrite(&readFile, 1, 1, stdout);
          assert(e == 1);

    }

    assert(fclose(fileOfHex) == 0);

    return 0;

}*/