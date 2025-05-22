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

void insertToAvlTreeIterative(avlTree* tree, int contentToInsert);
void removeFromAvlTree(avlTree* tree, int contentToRemove);
void rotateRight(node* subtreeRoot);
void rotateLeft(node* subtreeRoot);
void balanceSubtree(node* subtree, node* parentOfSubtree, int pathFromParentToSubtree);
int calculateBalancingFactor(node* node);
void recalcNodeHeight(node* node);
int getNodeHeight(node* subtree);
void updateParentPointer(node* parent, node* toBePointedAt, int leftOrRight);
void printTreeInOrder(avlTree* subtree); //L-P-R
void printSubtreeInOrder(node* subtree); //L-P-R
int getMax(int a, int b);

avlTree* tree = NULL;

int main(int argc, char** argv)
{
    srand(time(NULL));
    tree = (avlTree*) malloc(sizeof(avlTree));
    tree->root = NULL;

    // for (int i=0; i<insertQnt; i++) {
    //     int randomNumber = (rand() % (1000)); // random between 0 and 1000
    //     insertToAvlTreeIterative(tree, randomNumber);
    //     printTreeInOrder(tree);
    // }
    
    insertToAvlTreeIterative(tree, 705);
    insertToAvlTreeIterative(tree, 52);
    insertToAvlTreeIterative(tree, 756);
    insertToAvlTreeIterative(tree, 594);
    printTreeInOrder(tree);
    insertToAvlTreeIterative(tree, 292);
    printTreeInOrder(tree);
}

void insertToAvlTreeIterative(avlTree* tree, int contentToInsert) {
    // The insertion works better recursively or with a stack

    printf("[ATTEMPTING INSERT] %d\n", contentToInsert);

    if (tree->root == NULL) {
        // first insertion
        tree->root = (node*) malloc(sizeof(node));
        tree->root->left = NULL;
        tree->root->right = NULL;
        tree->root->content = contentToInsert;
        tree->root->nodeHeight = 1;
        printf("[OK INSERTED NEW ROOT] %d\n", tree->root->content);
        tree->nodeQnt = 1;
        tree->treeHeight = 1;
        return;
    }

    node* current = tree->root;
    node* parentOfCurrent = tree->root;
    int pathFromParentToCurrent = 0;
    int pathLength = 1; // determined by how many times it went to left or right (+ 1)

    // Find correct place for new element
    while(current != NULL) {
        pathFromParentToCurrent = contentToInsert - current->content;
        pathLength++;
        if (pathFromParentToCurrent < 0) {
            // go to left subtree
            parentOfCurrent = current;
            current = current->left;
            // could stack the node to avoid repathing later
        }
        if (pathFromParentToCurrent > 0) {
            // go to right subtree
            parentOfCurrent = current;
            current = current->right;
            // could stack the node to avoid repathing later
        }
        if (pathFromParentToCurrent == 0) {
            // content is already in tree
            printf("[ERROR INSERTING - REPEATED CONTENT] %d\n", contentToInsert);
            return;
        }
    }

    // here, current (or newNode) should be NULL
    node* newNode = (node*) malloc(sizeof(node));
    newNode->content = contentToInsert;
    newNode->nodeHeight = 1;
    newNode->left = NULL;
    newNode->right = NULL;

    // parent->left OR parent->right should point to him depending on last move direction
    updateParentPointer(parentOfCurrent, newNode, pathFromParentToCurrent);

    tree->nodeQnt++;
    if (pathLength > tree->treeHeight) {
        tree->treeHeight = pathLength; // tree height = height of highest node
    }

    // Repath: Update height for all nodes in path (could use a stack/recursion instead of repathing)
    current = tree->root;
    while(current != NULL) {
        if (pathLength > current->nodeHeight) {
            current->nodeHeight = pathLength;
        }
        pathLength--;
        pathFromParentToCurrent = contentToInsert - current->content;
        if (pathFromParentToCurrent < 0) current = current->left;
        if (pathFromParentToCurrent > 0) current = current->right;
        if (pathFromParentToCurrent == 0) current = NULL; // found just-inserted node. stop loop
    }

    // Repath: Balance all nodes in path (could use a stack/recursion instead of repathing)
    current = tree->root;
    parentOfCurrent = current;
    while(current != NULL) {
        balanceSubtree(current,parentOfCurrent,pathFromParentToCurrent);
        pathFromParentToCurrent = contentToInsert - current->content;
        if (pathFromParentToCurrent < 0) {
            parentOfCurrent = current;
            current = current->left;
        }
        if (pathFromParentToCurrent > 0) {
            parentOfCurrent = current;
            current = current->right;
        } 
        if (pathFromParentToCurrent == 0) current = NULL; // found just-inserted node. stop loop
    }

    printf("[OK INSERTED] %d - current nodes: %d\n", newNode->content, tree->nodeQnt);
}

void removeFromAvlTree(avlTree* tree, int contentToRemove) {
    return;
}

void rotateLeft(node* subtreeRoot) {
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
    subtreeRoot->nodeHeight = 1 + getMax(getNodeHeight(subtreeRoot->left),getNodeHeight(subtreeRoot->left));
    axis->nodeHeight = 1 + getMax(getNodeHeight(axis->left),getNodeHeight(axis->left));
}

void rotateRight(node* subtreeRoot) {
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
    subtreeRoot->nodeHeight = 1 + getMax(getNodeHeight(subtreeRoot->left),getNodeHeight(subtreeRoot->left));
    axis->nodeHeight = 1 + getMax(getNodeHeight(axis->left),getNodeHeight(axis->left));
}

void recalcNodeHeight(node* node) {
    if (node == NULL) 
        return;
    node->nodeHeight = 1 + getMax(getNodeHeight(node->left),getNodeHeight(node->left));
}

void balanceSubtree(node* subtree, node* parentOfSubtree, int pathFromParentToSubtree) {
    if (subtree == NULL) {
        return;
    }

    int balance = calculateBalancingFactor(subtree);

    if (balance > 1) {
        //rotate and recalculate height
        if (calculateBalancingFactor(subtree->right) < 0) {
            updateParentPointer(parentOfSubtree, subtree->left,pathFromParentToSubtree);
            rotateRight(subtree->right);
        }
        updateParentPointer(parentOfSubtree, subtree->right,pathFromParentToSubtree);
        rotateLeft(subtree);
    }
    
    if (balance < -1) {
        //rotate and recalculate height
        if (calculateBalancingFactor(subtree->left) > 0) {
            updateParentPointer(parentOfSubtree, subtree->right,pathFromParentToSubtree);
            rotateLeft(subtree->left);
        }
        updateParentPointer(parentOfSubtree, subtree->left,pathFromParentToSubtree);
        rotateRight(subtree);
    }
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

void printTreeInOrder(avlTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT IN ORDER - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQnt);
    printSubtreeInOrder(tree->root);
}

void printSubtreeInOrder(node* subtree) { //L-P-R
    if (subtree == NULL) {
        printf("[ERROR PRINTING InOrder - NULL SUBTREE]");
    }

    if (subtree->left != NULL) {
        printSubtreeInOrder(subtree->left);
    }

    printf("> %d\n", subtree->content);

    if (subtree->right != NULL) {
        printSubtreeInOrder(subtree->right);
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