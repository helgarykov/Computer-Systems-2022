//
//  main.c
//  C_Project_Factorial_Fibonacci_etc
//
//  Created by Helga on 11/09/2022.
//

#include <stdio.h>
#include <time.h>

//static int cache[100] = {0};

/* Function to raise to power quickly
 param: a-number to raise
 param: n- power(hele tal)
 for ulige tal: a_{n} = a * a_{n-1}, n > 0
 for lige tal: a_{n} = (a_{2})^{n/2}
 for n = 0: a_{n}=1
 вещественное число с плавающей точкой двойной точности */
double fast_power(double a, int n)
{
    if(n == 0) return 1;
    if(n%2 == 1)                                            //for ulige tal
        return a * fast_power(a, n-1);
    else
        return fast_power(a * a, n/2);
}


/* Function to canculate the factorial of a number n */
int factorial(int n)
{
    if(0 == n) return 1;
    return factorial(n-1) * n;
}

/* Function to calculate the Great Common Divider of number a and number b */
int gcd(int a, int b)
{
    if(0 == b) return a;
    //printf("a = %d and b = %d\n", a, b);
    return gcd(b, a%b);
    // How does b becomes 0? If a = 2, b = 8:
    // 1.itr) a = b = 8, b = (2 mod 8) = 2
    // 2.itr) a = b = 2, b = (2 mod 2) = 0 !! b = 0 -> return a
    // 3.itr) b = 0, return a = 2.
    
}

/* Function to calculate Fibonacci number without recursion (Faster fun!!)
 dynamic programming from the bottom */
int fib_dynamic(int n)
{
    int Fib[n+1];
    Fib[0] = 0;
    Fib[1] = 1;
    for (int i = 2; i <= n; ++i)
        Fib[i] = Fib[i-1] + Fib[i-2];
    return Fib[n];
}

int main(int argc, const char * argv[])
{
   
    int n, m;
    scanf("%d%d", &n, &m);
    printf("factorial (%d) = %d\n", n, factorial(n));
    printf("gcd of (%d, %d) = %d\n", n, m, gcd(n, m));
    printf("fast_power(%d, %d) = %lf\n", n, m, fast_power(n, m));
    
    /*for(int n = 1; n < 50; n += 1)
    {
        clock_t time1 = clock();
        int result = fib_dynamic(50);
        clock_t time2 = clock();
        int delta_ms = (int)(time2 - time1) * 1000/CLOCKS_PER_SEC;
    
        printf("fib(%d) = %d,\t time = %d ms\n", n, result, delta_ms);
    } */
    return 0;
}
