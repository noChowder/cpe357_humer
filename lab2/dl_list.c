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
struct node *last = NULL;

struct node * get_last(){
    struct node *temp = head;
    while(temp->next != NULL){
        temp = temp->next;
    }
    last = temp;
    return last;
}

int push_string(char *string){
    struct node *node = malloc(sizeof(struct node));
    if(node == NULL){
        return 0;
    }
    if(head == NULL){
        strcpy(node->text, string);
        node->next = last;
        node->prev = NULL;
        head = node;
        node->next = NULL;
        node->prev = head;
        last = node;
    }
    else{
        /*strcpy(node->text, string);
        node->next = NULL;
        struct node **ptr;
        *ptr = head;
        while((*ptr)->next != NULL){
            *ptr = (*ptr)->next;
        }
        node->prev = *ptr;
        (*ptr)->next = node;*/
        last = get_last();
        strcpy(node->text, string);
        node->next = NULL;
        node->prev = last;
        last->next = node;
    }

    return 0;
}

int print_list(){
    struct node *temp = head;
    if(head == NULL){
        return 0;
    }
    last = get_last();
    while(temp != last){
        printf("%s \n", temp->text);
        temp = temp->next;
    }
    printf("%s \n", temp->text);

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