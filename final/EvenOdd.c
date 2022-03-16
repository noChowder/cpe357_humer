#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int compare(int num1, int num2){ // true(1) if 2nd number greater, false(0) if 1st number greater
    if(num1 > num2){
        return 0;
    }
    return 1;
}

void main(int argc, char *argv[]){
    /* set the integer array */
    char c;
    int n;
    int arr_size = 0;
    while( (c = getchar()) != EOF ){
        arr_size++;
    }
    rewind(stdin);
    int int_arr[arr_size];
    int i = 0;
    while( (c = getchar()) != EOF ){
        n = c-'0';
        int_arr[i] = n;
        i++;
    }
    for(int i = 0; i < arr_size; i++){
        printf("%d \n", int_arr[i]);
    }

    /* start timer */
    time_t start, end;
    double time_elapsed;
    start = clock();

    /* fork */
    int processes = atoi(argv[1]);
    int pids[processes];
    

    /* end timer */
    for(int i = 0; i < processes; i++){
        wait(0);
    }
    end = clock();
    time_elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
}