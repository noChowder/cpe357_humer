#include <stdio.h>

int main(){
    float x = 3.3;
    float y = 5.8;
    int x1, x2, y1, y2;
    float dx, dy;
    
    x1 = x / 1;
    if(x1 < x){
        x2 = x1 + 1;
    }
    else{
        x2 = x1;
    }
    dx = x - x1;

    printf("%d, %d, %f \n", x1, x2, dx);

    y1 = y / 1;
    if(y1 < y){
        y2 = y1 + 1;
    }
    else{
        y2 = y1;
    }
    dy = y - y1;

    printf("%d, %d, %f \n", y1, y2, dy);

    return 0;
}