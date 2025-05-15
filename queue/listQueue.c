#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define qntPeople 17
#define nameSize 20

typedef struct queueNode {
    char name[nameSize];
    struct queueNode* next;
} queueNode;

typedef struct queue {
    queueNode* start;
    queueNode* end;
    size_t currentSize;
    // size_t capacity;
} queue;

void enqueue(char* nome);
void dequeue();
void printQueue();

// global variable
queue* queueInstance = NULL;

int main(int argc, char* argv)
{
    // char arr[qntDocuments][documentNameSize] = {"Document A", "BCASD","ASDFASD"};

    char** people = (char**) malloc(qntPeople * sizeof(char*));
    for (int i=0; i<qntPeople; i++) {
        people[i] = (char*) malloc(sizeof(char[nameSize]));
        strcpy(people[i],"WAITER ");
        char personNumber[7];
        strcat(people[i], itoa(i+1,personNumber,10));
    }

    printQueue();

    enqueue(people[0]);
    dequeue();
    printQueue();

    enqueue(people[0]);
    enqueue(people[1]);
    dequeue();
    dequeue();
    printQueue();

    for (int i=0; i<qntPeople; i++) {
        // printf("%s\n",documents[i]);
        enqueue(people[i]);
    }

    printQueue();

    dequeue();
    dequeue();
    dequeue();
    dequeue();
    dequeue();

    printQueue();

    for (int i=0; i<qntPeople; i++) {
        dequeue(people[i]);
    }
    printQueue();

    enqueue(people[0]);
    enqueue(people[1]);

    printQueue();
}

void enqueue (char* nome) {
    if (queueInstance == NULL) {
        queueInstance = (queue*) malloc(sizeof(queue));
        queueInstance->currentSize = 0;
        queueInstance->start = NULL;
        queueInstance->end = NULL;
    }

    queueNode* newNode = (queueNode*) malloc(sizeof(queueNode));
    strcpy(newNode->name, nome);
    newNode->next = NULL; // new node will be last in queue so points to none

    if (queueInstance->end != NULL) {
        queueInstance->end->next = newNode; // previous last node now points to the new node
    }
    if (queueInstance->start == NULL) {
        queueInstance->start = newNode; // if its the first inserted node, point start to it
    }

    queueInstance->end = newNode; // new node becomes the last node
    queueInstance->currentSize++; // optionally increment size
    printf("[INSERTED TO QUEUE] %s\n", newNode->name);
}

void dequeue() {
    if (queueInstance == NULL || queueInstance->currentSize == 0) {
        printf("[ERROR DEQUEUING - QUEUE IS EMPTY]\n");
        return;
    }

    queueNode* poppedNode = queueInstance->start;
    printf("[DEQUEUED] %s\n", poppedNode->name);
    queueInstance->start = queueInstance->start->next; // shift start to next node
    free(poppedNode);
    queueInstance->currentSize--; // optionally decrement size
    if (queueInstance->start == NULL) {
        free(queueInstance);
        queueInstance = NULL;
    }
}

void printQueue() {
    if (queueInstance == NULL) {
        printf("[QUEUE IS EMPTY]\n");
        return;
    }

    printf("[QUEUE SIZE: %d]\n", queueInstance->currentSize);
    queueNode* current = queueInstance->start;

    printf(">>>[QUEUE START]\n");
    while(current != NULL) {
        printf("'%s' -> ", current->name);
        current = current->next;
    }
    printf("\n>>>[QUEUE END]\n");
}
