#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LL_Floor.h"

void constructLL(LinkedList *list)
{
    // node *firstNode;
    node *head;
    head = (node *)malloc(sizeof(node));
    head = NULL;
    list->head = head;
}

void printLL(LinkedList *list)
{
    
    node *current = list->head;
    int i = 0;
    while (current != NULL)
    {
        i++;
        
        printf("%d. Element in the list: %d\n", i, current->value->floorID);
        // printf("%d. Element in the list: %d\n", i, current->value);
        current = current->next;
    }
}

void addFrontLL(LinkedList *list, fInfo* info)
{
    node *newNode;
    newNode = (node *)malloc(sizeof(node));

    newNode->value = info;
    newNode->next = list->head;
    list->head = newNode;
}

void addRearLL(LinkedList *list, fInfo* info)
{
    node *tail = list->head;
    node *current = list->head;
    if (current == NULL)
    {
        addFrontLL(list, info);
        return;
    }
    while (current->next != NULL)
    {
        current = current->next;
    }
    tail = current;

    node *newNode;
    newNode = (node *)malloc(sizeof(node));

    newNode->value = info;
    newNode->next = tail->next;
    tail->next = newNode;
}

int numbInLL(LinkedList *list)
{
    node *current = list->head;
    int i = 0;
    while (current != NULL)
    {
        i++;
        current = current->next;
    }
    return i;
}

bool isEmpty(LinkedList *list)
{
    return list->head == NULL ? true : false;
}

void copyLL(LinkedList *list1, LinkedList *list2)
{
    constructLL(list1);
    node *currentL2 = list2->head;
    while (currentL2 != NULL)
    {
        addRearLL(list1, currentL2->value);
        currentL2 = currentL2->next;
    }
}

void destructLL(LinkedList *list)
{
    node *toClear;
    node *current = list->head;
    while (current->next != NULL)
    {
        toClear = current;
        current = current->next;
        free(toClear);
    }
    free(current);
}

// Deletes the front element
void deleteLL(LinkedList *list)
{
    if (list->head->next != NULL)
    {
        node* toRemove = list->head;
        list->head = toRemove->next;
        free(toRemove->value);
        free(toRemove);
    }
}