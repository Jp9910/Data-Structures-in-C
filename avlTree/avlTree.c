#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define insertQnt 10

typedef struct avlTreeNode {
    int content;
    int nodeHeight;
    struct avlTreeNode* left;
    struct avlTreeNode* right;
} node;

typedef struct avlTree {
    struct avlTreeNode* root;
    size_t nodeQnt;
    int treeHeight;
} avlTree;

void insertToAvlTree(avlTree* tree, int contentToInsert);
node* insertInSubtree(node* current, int contentToInsert);
void removeFromAvlTree(avlTree* tree, int contentToRemove);
node* rotateRight(node* subtreeRoot);
node* rotateLeft(node* subtreeRoot);
node* balanceSubtree(node* subtree);
int calculateBalancingFactor(node* node);
void recalcNodeHeight(node* node);
int getNodeHeight(node* subtree);
void updateParentPointer(node* parent, node* toBePointedAt, int leftOrRight);
void printTreePreOrder(avlTree* subtree); //L-P-R
void printSubtreePreOrder(node* subtree); //L-P-R
void printTreeGraphically(avlTree* tree);
void printSubtreeGraphically(node* subtree, int nodeQnt);
int getMax(int a, int b);

avlTree* tree = NULL;

int main(int argc, char** argv)
{
    srand(time(NULL));
    tree = (avlTree*) malloc(sizeof(avlTree));
    tree->root = NULL;
    tree->nodeQnt = 0;
    tree->treeHeight = 0;

    // for (int i=0; i<insertQnt; i++) {
    //     int randomNumber = (rand() % (1000)); // random between 0 and 1000
    //     insertToAvlTree(tree, randomNumber);
    //     printTreePreOrder(tree);
    // }

    insertToAvlTree(tree, 6);
    printTreePreOrder(tree);
    insertToAvlTree(tree, 7);
    printTreePreOrder(tree);
    insertToAvlTree(tree, 8);
    printTreePreOrder(tree);
    insertToAvlTree(tree, 9);
    printTreePreOrder(tree);
    insertToAvlTree(tree, 4);
    printTreePreOrder(tree);
    insertToAvlTree(tree, 5);
    printTreePreOrder(tree);

    printTreeGraphically(tree);
}

void insertToAvlTree(avlTree* tree, int contentToInsert) {
    // The insertion works better recursively or with a stack

    printf("[ATTEMPTING INSERT] %d\n", contentToInsert);
    node* newRoot = insertInSubtree(tree->root, contentToInsert);
    if (tree->root == NULL) {
        tree->root = newRoot;
    }
    tree->nodeQnt++;
    tree->treeHeight = tree->root->nodeHeight;
    printf("[OK INSERTED] %d - # of current nodes: %d\n", contentToInsert, tree->nodeQnt);
}

node* insertInSubtree(node* current, int contentToInsert) {

    // base case - null node is found
    if (current == NULL) {
        current = (node*) malloc(sizeof(node));
        current->content = contentToInsert;
        current->left = NULL;
        current->right = NULL;
        current->nodeHeight = 1;
        return current;
    }

    int pathFromParentToCurrent = contentToInsert - current->content;
    node* newNode;

    // recur on left or right (until current = null)
    // (repeated content goes to right)
    if (pathFromParentToCurrent >= 0) {
        newNode = insertInSubtree(current->right, contentToInsert);
        current->right = newNode; // update parent reference to it
    } else {
        newNode = insertInSubtree(current->left, contentToInsert);
        current->left = newNode; // update parent reference to it
    }

    // update height and balance all nodes on way back
    recalcNodeHeight(current);
    node* newSubtreeRoot = balanceSubtree(current);
    return newSubtreeRoot;
}

void removeFromAvlTree(avlTree* tree, int contentToRemove) {
    return;
}

// returns the new root of the subtree (the axis of rotation)
node* rotateLeft(node* subtreeRoot) {
    // root               axis
    //   \                / \
    //   axis   ---->   root X
    //   / \              \
    //  Y   X              Y
    node* axis = subtreeRoot->right;
    printf("[ROTATING LEFT] root: %d - axis: %d\n", subtreeRoot->content, axis->content);
    if (subtreeRoot == tree->root) {
        tree->root = axis; // update reference to root of tree if rotation involves it
    }
    subtreeRoot->right = axis->left;
    axis->left = subtreeRoot;

    // update node heights
    recalcNodeHeight(subtreeRoot);
    recalcNodeHeight(axis);

    return axis;
}

// returns the new root of the subtree (the axis of rotation)
node* rotateRight(node* subtreeRoot) {
    //    root           axis
    //    /              / \
    //  axis   ---->    X  root
    //  / \                 /
    // X   Y               Y
    node* axis = subtreeRoot->left;
    printf("[ROTATING RIGHT] root: %d - axis: %d\n", subtreeRoot->content, axis->content);
    if (subtreeRoot == tree->root) {
        tree->root = axis;
    }
    subtreeRoot->left = axis->right;
    axis->right = subtreeRoot;

    // update node heights
    recalcNodeHeight(subtreeRoot);
    recalcNodeHeight(axis);

    return axis;
}

void recalcNodeHeight(node* node) {
    if (node == NULL)
        return;
    node->nodeHeight = 1 + getMax(getNodeHeight(node->left),getNodeHeight(node->right));
}

// returns the new root of the subtree (the axis of rotation, or itself if its not rotated)
node* balanceSubtree(node* subtree) {
    if (subtree == NULL) {
        return NULL;
    }

    node* newSubtreeRoot = subtree;
    int balance = calculateBalancingFactor(subtree);

    if (balance > 1) {
        //rotate and recalculate height
        if (calculateBalancingFactor(subtree->right) < 0) {
            subtree->right = rotateRight(subtree->right); // update the parent's right pointer ("subtree->right") of the rotated subtree to point to the axis of rotation, which will turn into the new root of the right subtree
        }
        newSubtreeRoot = rotateLeft(subtree); // return the new subtree root so the parent of it can point to it on the previous function
    }

    if (balance < -1) {
        //rotate and recalculate height
        if (calculateBalancingFactor(subtree->left) > 0) {
            subtree->left = rotateLeft(subtree->left); // update the parent's left pointer ("subtree->left") of the rotated subtree to point to the axis of rotation, which will turn into the new root of the left subtree
        }
        newSubtreeRoot = rotateRight(subtree); // return the new subtree root so the parent of it can point to it on the previous function
    }

    return newSubtreeRoot;
}

int calculateBalancingFactor(node* node) {
    int factor = getNodeHeight(node->right) - getNodeHeight(node->left);
    return factor;
}

int getNodeHeight(node* node) {
    if (node == NULL) {
        return 0;
    }
    return node->nodeHeight;
}

int getMax(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

void updateParentPointer(node* parent, node* toBePointedAt, int leftOrRight) {
    if (parent == toBePointedAt) {
        printf(">>>[Can't update parent pointer - same as to be pointed at]<<<\n");
        return;
    }

    if (leftOrRight == 0) {
        printf(">>>[ERROR - INVALID POINTER UPDATE OR COMPARISON]<<<\n");
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

void printTreePreOrder(avlTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT PRE ORDER - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQnt);
    printSubtreePreOrder(tree->root);
}

void printSubtreePreOrder(node* subtree) { //P-L-R
    if (subtree == NULL) {
        printf("[ERROR PRINTING PreOrder - NULL SUBTREE]");
    }

    printf("> %d [height: %d]\n" , subtree->content, subtree->nodeHeight);

    if (subtree->left != NULL) {
        printSubtreePreOrder(subtree->left);
    }

    if (subtree->right != NULL) {
        printSubtreePreOrder(subtree->right);
    }
}

void printTreeGraphically(avlTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT GRAPHICALLY - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQnt);
    printSubtreeGraphically(tree->root, tree->nodeQnt);
}

void printSubtreeGraphically(node* subtree, int nodeQnt) { 
    //     P
    //    / \
    //   L   R
    //  /   / \
    // LL  RL RR

    // Tem que ser em largura...

    int spacingNumber = nodeQnt;
    char* spaces = (char*) malloc((nodeQnt+1)*sizeof(char));
    while (subtree != NULL) {
        spaces[0] = '\0';
        // strcpy(spaces, "");

        for (int i=0; i<spacingNumber; i++) {
            // strcat(spaces, " ");
            spaces[i] = ' ';
        }
        spaces[spacingNumber] = '\0';

        printf("  %s%d\n",spaces,subtree->content);
        if (subtree->left != NULL && subtree->right != NULL) {
            printf(" %s/ \\ \n",spaces);
            printf("%s%d   %d\n",spaces,subtree->left->content,subtree->right->content);
        }
        if (subtree->left == NULL && subtree->right != NULL) {
            printf(" %s  \\ \n",spaces);
            printf("%s %s%d \n",spaces,spaces,subtree->right->content);
        }
        if (subtree->left != NULL && subtree->right == NULL) {
            printf(" %s/ \n",spaces);
            printf("%s%d \n",spaces,spaces,subtree->right->content);
        }
        spacingNumber--;
        subtree = NULL;
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