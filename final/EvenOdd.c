#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

int compare(int num1, int num2){ // true(1) if 1st number greater, false(0) if 2nd number greater
    if(num1 > num2){
        return 1;
    }
    return 0;
}

void swap(int *iap, int index1, int index2){
    int num1_old = *(iap+index1);
    int num2_old = *(iap+index2);
    *(iap+index1) = num2_old;
    *(iap+index2) = num1_old;
}

int check_sorted(int *iap, int arr_size){ // 1 if sorted, 0 if unsorted
    for(int i = 0; i < arr_size-1; i++){
        if(*(iap+i) > *(iap+i+1)){
            return 0;
        }
    }
    return 1;
}

void sort(int *iap, int arr_size){
    while(!check_sorted(iap, arr_size)){
        if((arr_size % 2) == 0){ // array size is even
            for(int i = 0; i < arr_size-1; i+=2){ // odd phase
                if(compare(*(iap+i), *(iap+i+1)))
                    swap(iap, i, i+1);
            }
            for(int i = 1; i < arr_size-2; i+=2){ // even phase
                if(compare(*(iap+i), *(iap+i+1)))
                    swap(iap, i, i+1);
            }
        }
        else if((arr_size % 2) != 0){ // array size is odd
            for(int i = 0; i < arr_size-2; i+=2){ // odd phase
                if(compare(*(iap+i), *(iap+i+1)))
                    swap(iap, i, i+1);
            }
            for(int i = 1; i < arr_size-1; i+=2){ // even phase
                if(compare(*(iap+i), *(iap+i+1)))
                    swap(iap, i, i+1);
            }
        }
    }
}

void main(int argc, char *argv[]){
    /* set the integer array */
    char c;
    int arr_size = 0;
    while( (c = getchar()) != EOF ){
        arr_size++;
    }
    rewind(stdin);
    int int_arr[arr_size];
    int n;
    int i = 0;
    while( (c = getchar()) != EOF ){
        n = c-'0';
        int_arr[i] = n;
        i++;
    }
    // for(int i = 0; i < arr_size; i++){
    //     printf("%d \n", int_arr[i]);
    // }

    /* start timer */
    time_t start, end;
    double time_elapsed;
    start = clock();

    /* fork */
    int processes = atoi(argv[1]);
    int pids[processes];
    int *iap = (int *)mmap(NULL, arr_size*sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    for(int i = 0; i < arr_size; i++){
        *(iap+i) = int_arr[i];
    }
    // printf("%d \n", *(iap+arr_size-1));
    if(processes < 1){
        fprintf(stderr, "Not enough processes requested. \n");
        exit(-1);
    }
    else if(processes == 1){ // no fork, single process
        if(check_sorted(iap, arr_size)){
            //printf("sorted \n");
        }
        else{ // do sorting
            sort(iap, arr_size);
        }
    }
    else{
        int processes_old = processes;
        if(processes > (arr_size / 2)){
            processes = arr_size / 2;
        }
        if(processes < processes_old)
            printf("Too many processes, truncated to %d processes. \n", processes);
        int size_split = arr_size / processes;
        int arr_index;
        for(int i = 0; i < processes; i++){
            arr_index = i * size_split;
            // if( (arr_size % 2) != 0 && i == (processes-1) ) // last process does remainder for odd splits
            //     size_split++;
            if(i != (processes-1)){
                size_split++;
            }
            if(fork() == 0){
                while(!check_sorted(iap, arr_size)){
                    printf("%d \n", i);
                    sort(iap+arr_index, size_split);
                    printf("Sorted Array: \t[");
                    for(int i = 0; i < arr_size-1; i++){
                        printf(" %d,", *(iap+i));
                    }
                    printf(" %d ]\n", *(iap+arr_size-1));
                }
                exit(0);
            }
        }
    }

    /* end timer */
    for(int i = 1; i < processes; i++){
        wait(0);
    }
    end = clock();
    time_elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;

    /* print all */
    printf("Initial Array: \t[");
    for(int i = 0; i < arr_size-1; i++){
        printf(" %d,", int_arr[i]);
    }
    printf(" %d ]\n", int_arr[arr_size-1]);
    printf("Sorted Array: \t[");
    for(int i = 0; i < arr_size-1; i++){
        printf(" %d,", *(iap+i));
    }
    printf(" %d ]\n", *(iap+arr_size-1));
    printf("Processes: \t%d\n", processes);
    printf("Time to Sort: \t%.2f sec\n", time_elapsed);

    munmap(iap, arr_size*sizeof(int));
}