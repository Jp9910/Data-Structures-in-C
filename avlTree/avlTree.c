#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void insertToAvlTree(avlTree* tree, int contentToInsert);
void removeFromAvlTree(avlTree* tree, int contentToRemove);
void printSubtreeInOrder(node* subtree); //L-P-R

typedef struct avlTreeNode {
    int content;
    char balancingFactor; // [-128, 127] needs balance if < -2 or > 2
    struct avlTreeNode* left;
    struct avlTreeNode* right;
} node;

typedef struct avlTree {
    struct avlTreeNode* root;
    size_t nodeQnt;
} avlTree;

avlTree* tree = NULL;

int main(int argc, char** argv)
{
    tree = (avlTree*) malloc(sizeof(avlTree));
    tree->root = NULL;
    tree->nodeQnt = 0;


}

void insertToAvlTree(avlTree* tree, int contentToInsert) {
    printf("[ATTEMPTING INSERT] %d\n", contentToInsert);

    if (tree->root == NULL) {
        // first insertion
        tree->root = (node*) malloc(sizeof(node));
        tree->root->left = NULL;
        tree->root->right = NULL;
        tree->root->content = contentToInsert;
        printf("[OK INSERTED NEW ROOT] %d\n", tree->root->content);
        tree->nodeQnt++;
        return;
    }

    node* grandparentOfCurrent = tree->root;
    node* parentOfCurrent = tree->root;
    node* current = tree->root;
    int goToLeftOrRight = 0;
    while(current != NULL) {
        goToLeftOrRight = contentToInsert - current->content;
        if (goToLeftOrRight < 0) {
            // go to left subtree
            grandparentOfCurrent = parentOfCurrent;
            parentOfCurrent = current;
            current = current->left;
        }
        if (goToLeftOrRight > 0) {
            // go to right subtree
            grandparentOfCurrent = parentOfCurrent;
            parentOfCurrent = current;
            current = current->right;
        }
        if (goToLeftOrRight == 0) {
            // content is already in tree
            printf("[ERROR INSERTING - REPEATED CONTENT] %s\n", contentToInsert);
            return;
        }
    }

    // here, current (or newNode) should be NULL
    node* newNode = (node*) malloc(sizeof(node));
    newNode->content = contentToInsert;
    newNode->left = NULL;
    newNode->right = NULL;

    // parent->left OR parent->right should point to him depending on last move direction
    updateParentPointer(parentOfCurrent, newNode, goToLeftOrRight);
    tree->nodeQnt++;

    // check balance and rotate if necessary
    if (grandparentOfCurrent->balancingFactor > 1) {
        // rotate right
    }
    if (grandparentOfCurrent->balancingFactor < -1) {
        // rotate left
    }

    printf("[OK INSERTED] %s - current nodes: %d\n", newNode->content, tree->nodeQnt);
}

void removeFromAvlTree(avlTree* tree, int contentToRemove) {

}

void updateParentPointer(node* parent, node* toBePointedAt, int leftOrRight) {
    if (leftOrRight == 0) {
        printf(">>>[ERROR - INVALID POINTER UPDATE OR COMPARISON]<<<");
    }

    if (leftOrRight < 0) {
        // last move was to the left
        parent->left = toBePointedAt;
    }

    if (leftOrRight > 0) {
        // last move was to the right
        parent->right = toBePointedAt;
    }
}

void freeSubtree(node* subtree) { //L-P-R
    // freeing has to be done in Post Order
    if (subtree == NULL) {
        printf("[FREEING - SUBTREE IS ALREADY NULL]");
        return;
    }

    if (subtree->left != NULL) {
        freeSubtree(subtree->left);
    }

    if (subtree->right != NULL) {
        freeSubtree(subtree->right);
    }

    free(subtree);
    subtree = NULL;
}

void printTreeInOrder(avlTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT IN ORDER - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQnt);
    printSubtreeInOrder(tree->root);
}