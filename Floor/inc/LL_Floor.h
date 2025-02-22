#ifndef LL_FlOOR_H
#define LL_FLOOR_H

#include "stdbool.h"

typedef struct
{
    //char *timestapmt;
    clock_t spawnTime;
} person;

struct Snode
{
    person* value;
    struct Snode *next;
};
typedef struct Snode node;


struct list
{
    node *head;
    /*???*/;
};
typedef struct list LinkedList;

void addFrontLL(LinkedList *list, person* info);
void constructLL(LinkedList *list);
void printLL(LinkedList *list);
void addRearLL(LinkedList *list, person* info);
bool isEmpty(LinkedList *list);
int numbInLL(LinkedList *list);
// Deletes the front element
void deleteLL(LinkedList *list);
void copyLL(LinkedList *list1, LinkedList *list2);
void destructLL(LinkedList *list);
#endif