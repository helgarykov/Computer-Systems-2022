#include<stdio.h>
#include<string.h>
#include<ctype.h>


int main(int argc, char* argv[]){

    if(argc >= 2) {

        if (tolower(argv[1][0]) == 'a')
        {
            printf("No beginning A's are allowed");
        }else{

            for( int i = 1; i < argc; i++){

                //char* arg = argv[i];
                printf("%s\n", argv[i]);
                //printf("\n");
                
            }
        }
      
    }
      
}
        




   /*  }
    if (*argv[1][0] == 'A') {

        printf("No beginning A's are allowed");
    
    }
    
    else{

        for (int i = 0; i < argc; i++)
        {
              printf("%s\n", argv[i]);
        }

    }*/
    

