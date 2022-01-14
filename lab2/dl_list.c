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

int push_string(char *string){
    struct node *node = malloc(sizeof(struct node));
    if(node == NULL){
        return 0;
    }
    if(head == NULL){
        strcpy(node->text, string);
        node->next = NULL;
        node->prev = NULL;
        head = node;
    }
    else{
        strcpy(node->text, string);
        node->next = NULL;
        struct node **ptr;
        *ptr = head;
        while((*ptr)->next != NULL){
            *ptr = (*ptr)->next;
        }
        node->prev = *ptr;
        (*ptr)->next = node;
    }

    return 0;
}

int print_list(){
    struct node *node = malloc(sizeof(struct node));
    if(head == NULL){
        return 0;
    }
    struct node **ptr;
    *ptr = head;
    while((*ptr) != NULL){
        printf("%s \n", (*ptr)->text);
        *ptr = (*ptr)->next;
    }

    return 0;
}

int main(){
    printf("Select: \n1 push string \n2 print list \n3 delete item \n4 end program \n");
    int sel;
    scanf("%d", &sel);
    if(sel == 1){
        printf("insert text \n");
        char string[SIZE];
        scanf("%s", string);
        push_string(string);
        printf("\n");
        main();
    }
    else if(sel == 2){
        print_list();
        printf("\n");
        main();
    }

    //free(node);
    return 0;
}