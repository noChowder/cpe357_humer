#include <stdio.h>
#include <stdlib.h>

int main(){
    int arr[4];
    printf("%ld \n", sizeof(int));
    printf("%ld \n", sizeof(arr));
    arr[0] = 1;
    arr[1] = 1;
    arr[2] = 1;
    arr[3] = 1;
    printf("%ld \n", sizeof(arr));

    return 0;
}