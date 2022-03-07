#include <stdio.h>
#include <stdlib.h>

int main(){
    int **mat;
    int *arr1;
    int *arr2;
    int var1[] = {1, 2};
    int var2[] = {3, 4};
    arr1 = var1;
    arr2 = var2;
    *(mat+0) = arr1;
    *(mat+1) = arr2;
    int i = 0;
    while(i < 2){
        for(int j = 0; j < 2; j++){
            printf("%d \n", mat[0][j]);
        }
        **mat++;
        i++;
    }
}