#include <stdio.h>

struct listelement{
    listelement *next, *prev;
    char text[1000];
};
listelement *head = NULL;