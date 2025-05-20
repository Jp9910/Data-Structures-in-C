#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define qntDocuments 20
#define documentNameSize 10

typedef struct stackNode {
    char name[documentNameSize];
    struct stackNode* next;
} stackNode;

typedef struct stack {
    stackNode* top;
    size_t size;
} stack;

void concatChar(char* s, char c);
void addToStack(char* nome);
void popFromStack();
void printStack();

// global variable
stack* stackInstance = NULL;

int main(int argc, char* argv[])
{
    // char arr[qntDocuments][documentNameSize] = {"Document A", "BCASD","ASDFASD"};

    char** documents = (char**) malloc(qntDocuments * sizeof(char*));
    for (int i=0; i<qntDocuments; i++) {
        documents[i] = (char*) malloc(sizeof(char[documentNameSize]));
        strcpy(documents[i],"Document ");
        char docNumber[7];
        // concatChar(printers[i], i+1+'0'); // +'0' to convert to it to a char
        strcat(documents[i], itoa(i+1,docNumber,10));
    }

    printStack();

    for (int i=0; i<qntDocuments; i++) {
        // printf("%s\n",documents[i]);
        addToStack(documents[i]);
    }

    printStack();

    popFromStack();
    popFromStack();
    popFromStack();
    popFromStack();
    popFromStack();

    printStack();
    
    for (int i=0; i<qntDocuments; i++) {
        popFromStack(documents[i]);
    }
    printStack();
}

void addToStack (char* nome) {
    if (stackInstance == NULL) {
        stackInstance = (stack*) malloc(sizeof(stack));
        stackInstance->size = 0;
    }

    stackNode* newTop = (stackNode*) malloc(sizeof(stackNode));
    strcpy(newTop->name, nome);
    newTop->next = stackInstance->top;
    stackInstance->top = newTop; // new top is the newly inserted node
    stackInstance->size++; // optionally increment size
    printf("[INSERTED TO STACK]\n");
}

void popFromStack() {
    if (stackInstance == NULL || stackInstance->top == NULL) {
        printf("[ERROR POPPING - STACK IS EMPTY]\n");
        return;
    }

    stackNode* poppedNode = stackInstance->top;
    stackInstance->top = stackInstance->top->next; // shift top to next node
    free(poppedNode);
    if (stackInstance->top == NULL) {
        free(stackInstance);
    }
    stackInstance->size--; // optionally decrement size
    printf("[POPPED FROM STACK]\n");
}

void printStack() {
    if (stackInstance == NULL) {
        printf("[STACK IS EMPTY]\n");
        return;
    }
    
    printf("[STACK SIZE: %d]\n", stackInstance->size);
    stackNode* current = stackInstance->top;

    printf(">>>[STACK TOP]\n");
    while(current != NULL) {
        printf(">%s\n", current->name);
        current = current->next;
    }
    printf(">>>[STACK BOTTOM]\n");
}

void concatChar(char *s, char c) {
    // Get the current length of the string
    int l = strlen(s);
    // Add the new character at the end
    s[l] = c;
    // Add the null terminator
    s[l + 1] = '\0';
}