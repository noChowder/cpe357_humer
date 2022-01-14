#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 1000

struct node{
    struct node *next;
    struct node *prev;
    char text[SIZE];
};
struct node *head = NULL;

int push_string(struct node *node, char *string){
    if(head == NULL){
        node = malloc(SIZE*sizeof(char));
        strcpy(node->text, string);
        node->next = NULL;
    }
    else if(head != NULL){
        if(node == NULL){
            strcpy(node->text, string);
            node->next = NULL;
        }
        else{
            node->next->prev = node;
            push_string(node->next, string);
        }
    }

    return 0;
}

int print_list(){
    return 0;
}

int main(){
    printf("Select: \n1 push string \n2 print list \n3 delete item \n4 end program \n");
    int sel;
    scanf("%d", &sel);
    if(sel == 1){
        printf("insert text \n");
        char string[SIZE];
        scanf("%s", &string);
        push_string(head, string);
        main();
    }
    else if(sel == 2){
        print_list();
        main();
    }
    return 0;
}