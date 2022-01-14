#include <stdio.h>
#include <string.h>

int main(){
    char arr[10];
    char *string;
    string = "hello";
    strcpy(arr, string);
    printf("%s \n", arr);
}