#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define nameSize 12
#define qntContentInsert 200
#define qntContentRemove 150

typedef struct binaryTreeNode {
    char content[nameSize];
    // uint32_t order;
    struct binaryTreeNode* left; // there are 2 children on a binary tree - left and right
    struct binaryTreeNode* right;
} node;

typedef struct binaryTree {
    struct binaryTreeNode* root;
    size_t nodeQuantity;
} tree;

void insertToTree(tree* tree, char* contentToInsert);
void removeFromTree(tree* tree, char* contentToRemove);
void searchInTree(tree* tree, char* contentToSearch);
void printSubtreeInOrder(node* subtree); //L-P-R
void printSubtreePreOrder(node* subtree); //P-L-R
void printSubtreePostOrder(node* subtree); //L-R-P
void printTreeInOrder(tree* tree);
void printTreePreOrder(tree* tree);
void printTreePostOrder(tree* tree);
void updateParentPointer(node* parent, node* toBePointedAt, int leftOrRight);
void freeSubtree(node* subtree); //L-R-P

// global variable
tree* treeInstance = NULL;

int main(int argc, char* argv[])
{
    srand(time(NULL));
    char** content = (char**) malloc(qntContentInsert * sizeof(char*));
    for (int i=0; i < qntContentInsert; i++) {
        content[i] = (char*) malloc(sizeof(char[nameSize]));
        strcpy(content[i],"Content ");
        char randomChar = (rand() % (126 + 1 - 33) + 33); // random ascii char between 33 and 126
        strncat(content[i], &randomChar, 1);
    }

    treeInstance = (struct binaryTree*) malloc(sizeof(struct binaryTree));
    treeInstance->root = NULL;
    treeInstance->nodeQuantity = 0;

    // insert generated content strings
    for(int i=0; i < qntContentInsert; i++) {
        insertToTree(treeInstance, content[i]);
        // printTreeInOrder(treeInstance);
    }

    // remove some random content
    for(int i=0; i < qntContentRemove; i++) {
        char randomRemove[nameSize] = "Content ";
        char randomChar = (rand() % (126 + 1 - 33) + 33); // random ascii char between 33 and 126
        strncat(randomRemove, &randomChar, 1);
        removeFromTree(treeInstance, randomRemove);
        // printTreeInOrder(treeInstance);
    }

    printf("\n[IN ORDER]\n");
    printTreeInOrder(treeInstance);
    printf("\n[PRE ORDER]\n");
    printTreePreOrder(treeInstance);
    printf("\n[POST ORDER]\n");
    printTreePostOrder(treeInstance);

    for (int i=0; i < qntContentInsert; i++) {
        free(content[i]);
    }
    free(content);

    if (treeInstance != NULL) {
        freeSubtree(treeInstance->root);
        free(treeInstance);
    }

    return 0;
}

void insertToTree(tree* tree, char* contentToInsert)
{
    printf("[ATTEMPTING INSERT] %s\n", contentToInsert);

    if (tree->root == NULL) {
        // first insertion
        tree->root = (node*) malloc(sizeof(node));
        tree->root->left = NULL;
        tree->root->right = NULL;
        strcpy(tree->root->content,contentToInsert);
        printf("[OK INSERTED NEW ROOT] %s\n", tree->root->content);
        tree->nodeQuantity++;
        return;
    }

    node* parentOfCurrent = tree->root;
    node* current = tree->root;
    int comparison = 0;
    while(current != NULL) {
        comparison = strcmp(contentToInsert, current->content); // 'abc' is lexicographically before 'bac' strcmp(abc,bac)
        if (comparison < 0) {
            // go to left subtree
            parentOfCurrent = current;
            current = current->left;
        }
        if (comparison > 0) {
            // go to right subtree
            parentOfCurrent = current;
            current = current->right;
        }
        if (comparison == 0) {
            // content is already in tree
            printf("[ERROR INSERTING - REPEATED CONTENT] %s\n", contentToInsert);
            return;
        }
    }

    // here, current (or newNode) should be NULL
    node* newNode = (node*) malloc(sizeof(node));
    strcpy(newNode->content,contentToInsert);
    newNode->left = NULL;
    newNode->right = NULL;

    // parent->left OR parent->right should point to him depending on last move direction
    updateParentPointer(parentOfCurrent, newNode, comparison);
    tree->nodeQuantity++;

    printf("[OK INSERTED] %s - current nodes: %d\n", newNode->content, tree->nodeQuantity);
}

void removeFromTree(tree* tree, char* contentToRemove)
{
    printf("[ATTEMPTING REMOVE] %s\n", contentToRemove);
    if (tree == NULL || tree->root == NULL) {
        printf("[ERROR REMOVING - EMPTY TREE] %s\n", contentToRemove);
        return;
    }

    node* current = tree->root;
    node* parentOfCurrent = tree->root;
    int comparison = 1;
    while (comparison != 0 && current != NULL) {
        comparison = strcmp(contentToRemove, current->content); // 'abc' is lexicographically before 'bac' strcmp(abc,bac)
        if (comparison < 0) {
            // go to left subtree
            parentOfCurrent = current;
            current = current->left;
        }
        if (comparison > 0) {
            // go to right subtree
            parentOfCurrent = current;
            current = current->right;
        }
    }

    // here, either comparison is 0 or current is null (didnt find)
    if (current == NULL) {
        printf("[ERROR REMOVING - ELEMENT NOT FOUND] %s\n", contentToRemove);
        return;
    }

    // here, the element is found
    // if this is 0, it means, the current = parent so they are both the root
    int updateLeftOrRightParentPointer =  strcmp(contentToRemove, parentOfCurrent->content);

    // the way to remove it depends on how many subtrees the element has

    // if its a leaf node, can just remove it
    if (current->left == NULL && current->right == NULL) {
        // if its also the only node (the root), could free the tree (the node itself will be freed later in the function)
        if (current == tree->root) {
            // could free the tree here, but its better to not so other functions have more control of it
            // and it's not necessary to reallocate the memory for it when trying to insert after emptying it
            free(tree->root);
            tree->root = NULL;
            tree->nodeQuantity--;
            return;
            // free(tree);
            // tree = NULL;
        } else {
            // parent of current should stop pointing to him
            updateParentPointer(parentOfCurrent, NULL, updateLeftOrRightParentPointer);
        }
    }

    // if it has one subtree (to the left), the parent points to that subtree.
    // but if its the tree root, the subtree becomes the new root
    if (current->left != NULL && current->right == NULL) {
        if (current == tree->root) {
            tree->root = current->left;
        } else {
            updateParentPointer(parentOfCurrent, current->left, updateLeftOrRightParentPointer);
        }
    }

    // if it has one subtree (to the right), the parent points to that subtree.
    // but if its the tree root, the subtree becomes the new root
    if (current->left == NULL && current->right != NULL) {
        if (current == tree->root) {
            tree->root = current->right;
        } else {
            updateParentPointer(parentOfCurrent, current->right, updateLeftOrRightParentPointer);
        }
    }

    // if it has two subtrees, the parent points to either the left-most element on the right subtree
    // OR the right-most element on the left subtree
    if (current->left != NULL && current->right != NULL) {
        // I'll point to the left-most on the right subtree

        node* leftMost = current->right; // go to right subtree
        node* parentOfLeftMost = current; // after shifting the leftMost, his parent cant point to him anymore

        // navigate as much as possible to left
        while(leftMost->left != NULL) {
            parentOfLeftMost = leftMost;
            leftMost = leftMost->left;
        }

        // if the root is being removed, the leftmost has to become the new root 
        if (current == tree->root) {
            tree->root = leftMost;
        }

        // else, now parent points to this element
        else {
            updateParentPointer(parentOfCurrent, leftMost, updateLeftOrRightParentPointer);
        }

        // the new subtree root has to point to the children of the node that's being removed
        leftMost->left = current->left; // current->left cannot be = leftMost because it starts with being = to current->right

        // but shouldnt point to itself - such as in the case [L:1, P:2, R:3] while removing P:2
        if (current->right != leftMost) { // shouldnt point to itself
            if (leftMost->right != NULL) { 
                // if this leftMost element has other elements to its right, his parent will have to inherit them
                // (it cant have more elements to its left, since its the left-most one)
                parentOfLeftMost->left = leftMost->right;
            } else {
                // parentOfLeftMost wont point to leftMost anymore since leftMost is being shifted
                parentOfLeftMost->left = NULL;
            }
            leftMost->right = current->right;
        }
    }

    // free and nullify removed element
    printf("[OK REMOVED] %s - current nodes: %d\n", current->content, tree->nodeQuantity-1);
    free(current);
    current = NULL;
    tree->nodeQuantity--;
}

void searchInTree(tree* tree, char* contentToSearch)
{

}

void printSubtreeInOrder(node* subtree) { //L-P-R
    if (subtree == NULL) {
        printf("[ERROR PRINTING InOrder - NULL SUBTREE]");
    }

    if (subtree->left != NULL) {
        printSubtreeInOrder(subtree->left);
    }

    printf("> %s\n", subtree->content);

    if (subtree->right != NULL) {
        printSubtreeInOrder(subtree->right);
    }
}

void printSubtreePreOrder(node* subtree) { //P-L-R
    if (subtree == NULL) {
        printf("[ERROR PRINTING PreOrder - NULL SUBTREE]");
    }

    printf("> %s\n" , subtree->content);

    if (subtree->left != NULL) {
        printSubtreePreOrder(subtree->left);
    }

    if (subtree->right != NULL) {
        printSubtreePreOrder(subtree->right);
    }
}

void printSubtreePostOrder(node* subtree) { //L-R-P
    if (subtree == NULL) {
        printf("[ERROR PRINTING PostOrder - NULL SUBTREE]");
    }

    if (subtree->left != NULL) {
        printSubtreePostOrder(subtree->left);
    }

    if (subtree->right != NULL) {
        printSubtreePostOrder(subtree->right);
    }

    printf("> %s\n", subtree->content);
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

void printTreeInOrder(tree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT IN ORDER - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQuantity);
    printSubtreeInOrder(tree->root);
}

void printTreePreOrder(tree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT PRE ORDER - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQuantity);
    printSubtreePreOrder(tree->root);
}

void printTreePostOrder(tree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT POST ORDER - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQuantity);
    printSubtreePostOrder(tree->root);
}