#include<stdio.h>
#include<stdlib.h>

  int main(int argc, char* argv[]) {
  int sum = 0;
  int x;
  printf("Enter an intiger: \n");
  scanf( "%d", &x);
  for (int i = 0; i < argc; i++) {
    
    if (x == 1) {
      sum = sum + x;

    } else {

      printf("Not an integer: %s\n", argv[i]);

      return 1;
    }
  }

  printf("The sum : %d\n", sum);
} 

//Arrays in C
/*int main() {
    int a[16];
    a[0] = 1;
    for (int i = 1; i < 16; i++) {
        a[i] = a[i-1] + 1;
    }
    printf("final = %d\n", a[15]);

    return 0;
} */
