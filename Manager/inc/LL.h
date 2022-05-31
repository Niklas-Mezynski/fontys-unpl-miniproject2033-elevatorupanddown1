#ifndef LL_H
#define LL_H

struct Snode
{
    int value;
    struct Snode *next;
};

typedef struct Snode node;

struct list
{
    node *head;
    /*???*/;
};

typedef struct list LinkedList;

void addFrontLL(LinkedList *list, int number);
void constructLL(LinkedList *list);
void printLL(LinkedList *list);
void addRearLL(LinkedList *list, int number);
bool isEmpty(LinkedList *list);
int numbInLL(LinkedList *list);
// Deletes the front element
void deleteLL(LinkedList *list);
void copyLL(LinkedList *list1, LinkedList *list2);
void destructLL(LinkedList *list);
#endif