#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "elevator.h"
#include "LL.h"

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
        printf("%d. Person in the list: appartment: %d | dest: %d\n", i, current->value->arpartmentFloor, current->value->destFloor);
        current = current->next;
    }
}

void addFrontLL(LinkedList *list, person *item)
{
    node *newNode;
    newNode = (node *)malloc(sizeof(node));

    newNode->value = item;
    newNode->next = list->head;
    list->head = newNode;
}

void addRearLL(LinkedList *list, person *item)
{
    node *tail = list->head;
    node *current = list->head;
    if (current == NULL)
    {
        addFrontLL(list, item);
        return;
    }
    while (current->next != NULL)
    {
        current = current->next;
    }
    tail = current;

    node *newNode;
    newNode = (node *)malloc(sizeof(node));

    newNode->value = item;
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

person *peekFront(LinkedList *list)
{
    if (list->head != NULL)
    {
        return list->head->value;
    }
    return NULL;
}

// Deletes the front element
bool deleteFrontLL(LinkedList *list)
{
    if (list->head->next != NULL)
    {
        node *toRemove = list->head;
        list->head = toRemove->next;
        free(toRemove->value);
        free(toRemove);
        return true;
    }
    list->head = NULL;
    return false;
}

int deleteAllByDestFloorLL(LinkedList *list, person *item)
{
    printf("LL deleteAllByDestFloorLL not implemented yet\n");
    exit(EXIT_FAILURE);
}