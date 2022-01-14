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
    if(head == NULL){
        printf("here1 \n");
        strcpy(node->text, string);
        node->next = NULL;
        head = node;
    }
    else{
        //struct node *next = malloc(sizeof(struct node));
        //node = head->next;
        //node->prev = head;
        struct node *ptr = head;
        printf("here2 \n");
        while(ptr != NULL){
            printf("here3 \n");
            ptr = ptr->next;
        }
        if(ptr == NULL){
            printf("here4 \n");
            strcpy(node->text, string);
            printf("here5 \n");
            node->next = NULL;
            printf("here6 \n");
            ptr = node;
        }
    }

    free(node);
    return 0;
}

int print_list(){
    printf("here\n");
    struct node *ptr = head;
    while(ptr != NULL){
        printf("here1\n");
        printf("%s \n", ptr->text);
        ptr = ptr->next;
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
        scanf("%s", &string);
        push_string(string);
        main();
    }
    else if(sel == 2){
        print_list();
        main();
    }

    //free(node);
    return 0;
}