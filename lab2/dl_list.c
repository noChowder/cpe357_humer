#include <stdio.h>

struct string{
    char *next, *prev;
    char text[1000];
};
char *head = NULL;

int main(){
    printf("Select: \n1 push string \n2 print list \n3 delete item \n4 end program \n");
    int selection;
    scanf("%d", &selection);
}