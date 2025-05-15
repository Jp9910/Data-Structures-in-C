#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct  listNode {
    char nome[51];
    struct  listNode* next;
    struct  listNode* prev;
}  listNode;

typedef struct list {
     listNode* head;
} list;

void addToList(char* nome);
void removeFromList(char* nome);
void showListNode(char* nome);
void fprintList();
void printList();
void freeList();

// global variables
list* rede = NULL;
FILE* input;
FILE* output;

int main(int argc, char *argv[])
{
    input = fopen("InputExample.txt","r");
    output = fopen("output.txt","w");
    char comando[7], nome[51];

    // %[] da match em uma certa sequência de caracteres
    // %[^] da match em qualquer sequência de caracteres exceto a descrita dentro das chaves
    // %[]s é uma string que da match em uma sequência de caracteres
    // %[^\n]s é uma string que vai ser lida até encontrar um caractere "\n"
    while(fscanf(input, "%s %[^\n]s", comando, nome) != EOF) {
        fprintList();
        if (strcmp(comando, "ADD") == 0) {
            printf("[ADD] %s \n", nome);
            addToList(nome);
        }
        if (strcmp(comando, "SHOW") == 0) {
            printf("[SHOW] %s \n",nome);
            showListNode(nome);
        }
        if (strcmp(comando, "REMOVE") == 0) {
            printf("[REMOVE] %s \n", nome);
            removeFromList(nome);
        }
    }

    freeList();
    return 0;
}

void addToList(char* nome) {
    if (rede == NULL) {
        // List is empty. Alocate list and insert first element
        rede = (list*) malloc(sizeof(list));

        rede->head = ( listNode*) malloc(sizeof( listNode));
        rede->head->prev = rede->head;
        rede->head->next = rede->head;
        strcpy(rede->head->nome, nome);

        fprintf(output,"[OK ADDED - CREATED NEW HEAD] ADD %s\n", nome);
        return;
    }

     listNode* lastNode = rede->head;

    // navegar até o ultimo elemento, checando se o nome já existe
    while(lastNode->next != rede->head) {
        if (strcmp(lastNode->nome,nome) == 0) {
            // nome já existe na rede, não inserir novamente
            fprintf(output,"[ERROR ADDING - ALREADY EXIST] ADD %s\n", nome);
            return;
        }
        lastNode = lastNode->next;
    }

    // checar se o ultimo elemento é igual
    if (strcmp(lastNode->nome,nome) == 0) {
        fprintf(output,"[ERROR ADDING - ALREADY EXIST] ADD %s\n", nome);
        return;
    }

    // Alocar o próximo elemento
     listNode* newNode = ( listNode*) malloc(sizeof( listNode));
    lastNode->next = newNode;

    newNode->prev = lastNode;
    newNode->next = rede->head; // circular list
    rede->head->prev = newNode; // circular list
    strcpy(lastNode->next->nome, nome);

    fprintf(output,"[OK ADDED] ADD %s\n", nome);
}

void removeFromList(char* nome) {
     listNode* current = rede->head;

    while(strcmp(current->nome,nome) != 0) {
        if (current->next == rede->head) {
            // Swept list and didnt find
            fprintf(output,"[ERROR REMOVING - NOT FOUND] REMOVE %s\n", nome);
            return;
        }
        current = current->next;
    }

    // se chegou aqui é pq achou o nome no node atual

    if (current->next == rede->head && current->prev == rede->head) {
        // se for o único elemento...
        rede->head = NULL;
        rede = NULL;
        free(current);
        free(rede);
        fprintf(output,"[OK REMOVING - REMOVED HEAD] REMOVE %s\n", nome);
        return;
    }

    if (current == rede->head) {
        // caso a remoção seja a head, tem que atualizar o ponteiro na rede
        rede->head = current->next;
    }
    current->prev->next = current->next; // anterior aponta pra o prox
    current->next->prev = current->prev; // prox aponta para o anterior
    free(current);

    fprintf(output,"[OK REMOVING] REMOVE %s\n", nome);
}

void showListNode(char* nome) {
    // printf("[SHOW] %s \n", nome);

    if (rede == NULL || rede->head == NULL) {
        fprintf(output,"[ERROR SHOW - EMPTY LIST] ? <- %s -> ?\n", nome);
        return;
    }

     listNode* current = rede->head;

    while(strcmp(current->nome,nome) != 0) {
        if (current->next == rede->head) {
            // ja deu a volta na lista e não achou
            fprintf(output,"[ERROR SHOW - NOT FOUND] ? <- %s -> ?\n", nome);
            return;
        }
        current = current->next;
    }

    fprintf(output,"[OK SHOWING] %s <- %s -> %s\n", current->prev->nome, current->nome, current->next->nome);
}

void fprintList() {
    if (rede == NULL || rede->head == NULL) {
        fprintf(output,"[EMPTY LIST]\n");
        return;
    }

     listNode* current = rede->head;
    fprintf(output,"%s <- [", current->prev->nome);

    do {
        fprintf(output,"%s, ", current->nome);
        current = current->next;
    } while(current != rede->head);

    fprintf(output,"] -> %s\n", current->nome);
}

void printList() {
    if (rede == NULL || rede->head == NULL) {
        printf("[EMPTY LIST]\n");
        return;
    }

    listNode* current = rede->head;
    printf("%s <- [", current->prev->nome);

    do {
        printf("%s, ", current->nome);
        current = current->next;
    } while(current != rede->head);

    printf("] -> %s\n", current->nome);
}

void freeList() {
    if (rede == NULL) {
        free(rede);
        return;
    }

    if (rede->head == NULL) {
        free(rede->head);
        return;
    }

    listNode* current = rede->head;
    listNode* nextNode = NULL;
    do {
        nextNode = current->next;
        free(current);
        current = nextNode;
    } while(current != NULL && current != rede->head);
}