#include<stdio.h>


int main(int argc, char* argv[]){
    if (argc != 1) {

        printf("argc: %d\n", argc);
        for (int i = 1; i < argc; i++) {
            printf("%s\n", argv[i]);

        }
    }
    
    else{

        printf("Wrong number of arguments");

    }
    
}
