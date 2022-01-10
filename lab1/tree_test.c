#include <stdio.h>

int main(){
    int height;
    scanf("%d", &height);
    if(!isdigit(height)){
        printf("not a digit \n");
    }
    printf("%d \n", height);
    return 0;
}