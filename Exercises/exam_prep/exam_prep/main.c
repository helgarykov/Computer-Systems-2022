//
//  main.c
//  exam_prep
//
//  Created by Helga on 02/01/2023.
//

#include <stdio.h>


int fib(int n)
{
    if (n < 3)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    int n = 6;
    printf("%d\n", fib(n));
    return 0;
}
