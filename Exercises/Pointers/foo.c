#include<stdio.h>
#include<string.h>

int main()
{
    int x = 3;
    int* p;
    p = &x;
    *p = 0;

    printf("x = %d\n", x);

    int y = 8;
    int* py;
    int** ppy;
    ppy = &py;
    py = &y;
    **ppy = 0;

    printf("y = %d\n", y);

    int a, b;
    int* pb = &b;
    pb = &a;         // pb points at the address of a
    *pb = 0;             // a = 0
    pb = &b;         // pb points at b
    *pb = 1;             // b = 1

    printf("a = %d, b = %d\n", a, b);

    int c;
    int* arr[2];
    arr[0] = &c;
    arr[1] = arr[0];                // arr[1] = &c
    *(arr[1]) = 1;                  // c = 1
    *(arr[0]) = *(arr[0]) - 1;      // c = c - 1 = 1 - 1

    printf("c = %d\n", c);
    return 0;
}
