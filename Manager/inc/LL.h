#ifndef LL_H
#define LL_H

#include "stdbool.h"
#include "elevator.h"

struct Snode
{
    person *value;
    struct Snode *next;
};

typedef struct Snode node;

struct list
{
    node *head;
};

typedef struct list LinkedList;

void addFrontLL(LinkedList *list, person *item);
void constructLL(LinkedList *list);
void printLL(LinkedList *list);
void addRearLL(LinkedList *list, person *item);
bool isEmpty(LinkedList *list);
int numbInLL(LinkedList *list);
person *peekFront(LinkedList *list);
// Deletes the front element
bool deleteFrontLL(LinkedList *list);

// Deletes all persons from the list that share target floor 
int deleteAllByDestFloorLL(LinkedList *list, person *item);

void copyLL(LinkedList *list1, LinkedList *list2);
void destructLL(LinkedList *list);
#endif