//
//  main.c
//  CProject_With_Files
//
//  Created by Helga on 09/09/2022.
//

#include <stdio.h>
 
int main(void)
{
    FILE * fp = fopen("data.txt", "w+");
    fclose(fp);
    return 0;
}

