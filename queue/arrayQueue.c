#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define qntPeople 20
#define nameSize 10
#define queueCapacity 15

typedef struct queue {
    size_t start;
    size_t end;
    size_t currentSize;
    size_t capacity;
    char** contents; // array of data (strings in this case)
} queue;

void enqueue(char* name);
void dequeue();
void printQueue();

// global variable
queue* Q = NULL;

int main(int argc, char* argv)
{
    // char arr[qntPrinters][nameSize] = {"printer1", "printer2","printer3"};
    char** waiters = (char**) malloc(qntPeople * sizeof(char*));
    for (int i=0; i<qntPeople; i++) {
        waiters[i] = (char*) malloc(sizeof(char[nameSize]));
        strcpy(waiters[i],"Person ");
        char waiterNumber[7];
        strcat(waiters[i], itoa(i+1,waiterNumber,10));
    }

    printQueue();

    enqueue(waiters[0]);
    dequeue();
    printQueue();

    enqueue(waiters[0]);
    enqueue(waiters[1]);
    dequeue();
    dequeue();
    printQueue();

    for (int i=0; i<qntPeople; i++) {
        // printf("%s\n",documents[i]);
        enqueue(waiters[i]);
    }

    printQueue();

    dequeue();
    dequeue();
    dequeue();
    dequeue();
    dequeue();

    printQueue();

    for (int i=0; i<qntPeople; i++) {
        dequeue(waiters[i]);
    }
    printQueue();

    enqueue(waiters[0]);
    enqueue(waiters[1]);

    printQueue();
}

void enqueue (char* nome) {
    if (Q == NULL) {
        Q = (queue*) malloc(sizeof(queue));
        Q->currentSize = 0;
        Q->capacity = queueCapacity;
        Q->start = 0; // starts at 0th index by default
        Q->end = 0;
        Q->contents = (char**) malloc(queueCapacity*sizeof(char*));
    }

    if (Q->currentSize >= Q->capacity) {
        printf("[ERROR QUEUING - QUEUE IS FULL]\n");
        return;
    }

    Q->contents[Q->currentSize] = (char*) malloc(sizeof(char[nameSize]));
    strcpy(Q->contents[Q->currentSize], nome);
    Q->end = (Q->end + 1) % Q->capacity; // end of queue will move by one index, modded by capacity
    Q->currentSize++;

    // Q->start will remain same during insertion

    printf("[INSERTED TO QUEUE] %s\n", Q->contents[Q->currentSize-1]);
}

void dequeue() {
    if (Q == NULL || Q->currentSize == 0) {
        printf("[ERROR DEQUEUING - QUEUE IS EMPTY]\n");
        return;
    }

    printf("[DEQUEUED] %s\n", Q->contents[Q->start]);
    free(Q->contents[Q->start]);
    Q->start = (Q->start + 1) % Q->capacity; // start of queue will move by one index, modded by capacity
    Q->currentSize--;
    if (Q->currentSize == 0) {
        free(Q->contents);
        free(Q);
        Q = NULL;
    }
}

void printQueue() {
    if (Q == NULL || Q->currentSize == 0) {
        printf("[QUEUE IS EMPTY]\n");
        return;
    }

    printf("[QUEUE SIZE: %d]\n", Q->currentSize);

    printf(">>>[QUEUE START]\n");
    size_t curr = Q->start;
    do {
        printf("'%s' -> ", Q->contents[curr]);
        curr = (curr+1) % Q->capacity;
    } while(curr != Q->end);
    printf("\n>>>[QUEUE END]\n");
}
