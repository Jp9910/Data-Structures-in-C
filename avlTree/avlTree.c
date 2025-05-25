#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define insertQnt 10
#define removeQnt 10
#define minContent -10
#define maxContent 10

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
node* removeFromSubtree(node* current, int contentToRemove);
void removeElementFromTree(node* element, node* parentOfElement, int updateLeftOrRightParentPointer);
node* rotateRight(node* subtreeRoot);
node* rotateLeft(node* subtreeRoot);
node* balanceSubtree(node* subtree);
int calculateBalancingFactor(node* node);
void recalcNodeHeight(node* node);
int getNodeHeight(node* subtree);
void updateParentPointer(node* parent, node* toBePointedAt, int leftOrRight);
void printTreeInOrder(avlTree* tree); //L-P-R
void printSubtreeInOrder(node* subtree);
void printTreePreOrder(avlTree* subtree); //P-L-R
void printSubtreePreOrder(node* subtree);
void printTreeGraphically(avlTree* tree);
void printSubtreeGraphically(node* subtree, int nodeQnt);
int getMax(int a, int b);

avlTree* tree = NULL;
int nodeNumberForPrinting = 0;

int main(int argc, char** argv)
{
    srand(time(NULL));
    tree = (avlTree*) malloc(sizeof(avlTree));
    tree->root = NULL;
    tree->nodeQnt = 0;
    tree->treeHeight = 0;

    for (int i=0; i<insertQnt; i++) {
        int randomNumber = (rand() % (maxContent + 1 - minContent) + minContent); // random between -5000 and 5000
        insertToAvlTree(tree, randomNumber);
    }
    printTreeInOrder(tree);

    for (int i=0; i<removeQnt; i++) {
        int randomNumber = (rand() % (maxContent + 1 - minContent) + minContent); // random between -5000 and 5000
        removeFromAvlTree(tree, randomNumber);
    }
    printTreeInOrder(tree);

    // printTreeGraphically(tree);
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
    // If there are repeated elements, it's going to remove only the first one that's found (closest to the root)

    printf("[ATTEMPTING REMOVE] %d\n", contentToRemove);
    if (tree == NULL || tree->root == NULL) {
        printf("[ERROR REMOVING - EMPTY TREE] %d\n", contentToRemove);
        return;
    }

    removeFromSubtree(tree->root, contentToRemove);
    tree->treeHeight = tree->root->nodeHeight;
}

node* removeFromSubtree(node* current, int contentToRemove) {
    // base case - null node is found
    if (current == NULL) {
        printf("[ERROR REMOVING - CONTENT NOT FOUND] %d\n", contentToRemove);
        return NULL;
    }

    int pathFromParentToCurrent = contentToRemove - current->content;
    node* removed;

    // recur on left or right (until a match or NULL is found)
    if (pathFromParentToCurrent == 0) { // found element
        return current;
    }
    if (pathFromParentToCurrent > 0) {
        removed = removeFromSubtree(current->right, contentToRemove); // recur until element is found
        if (removed != NULL) { // only remove it once during recursion
            removeElementFromTree(removed, current, 1); // +1 so parent's right pointer gets updated
        }
    }
    if (pathFromParentToCurrent < 0) {
        removed = removeFromSubtree(current->left, contentToRemove); // recur until element is found
        if (removed != NULL) { // only remove it once during recursion
            removeElementFromTree(removed, current, -1); // -1 so parent's left pointer gets updated
        }
    }

    // update height and balance all nodes on way back
    recalcNodeHeight(current);
    balanceSubtree(current);
    return NULL;
}

void removeElementFromTree(node* element, node* parentOfElement, int updateLeftOrRightParentPointer) {
    // the way to remove it depends on how many subtrees the element has

    // if its a leaf node, can just remove it
    if (element->left == NULL && element->right == NULL) {
        // if its also the only node (the root), could free the tree (the node itself will be freed later in the function)
        if (element == tree->root) {
            free(tree->root);
            tree->root = NULL;
            tree->nodeQnt--;
            return;
        } else {
            // parent of current should stop pointing to him
            updateParentPointer(parentOfElement, NULL, updateLeftOrRightParentPointer);
        }
    }

    // if it has one subtree (to the left), the parent points to that subtree.
    // but if its the tree root, the subtree becomes the new root
    if (element->left != NULL && element->right == NULL) {
        if (element == tree->root) {
            tree->root = element->left;
        } else {
            updateParentPointer(parentOfElement, element->left, updateLeftOrRightParentPointer);
        }
    }

    // if it has one subtree (to the right), the parent points to that subtree.
    // but if its the tree root, the subtree becomes the new root
    if (element->left == NULL && element->right != NULL) {
        if (element == tree->root) {
            tree->root = element->right;
        } else {
            updateParentPointer(parentOfElement, element->right, updateLeftOrRightParentPointer);
        }
    }

    // if it has two subtrees, the parent points to either the left-most element on the right subtree
    // OR the right-most element on the left subtree
    if (element->left != NULL && element->right != NULL) {
        // I'll point to the left-most on the right subtree

        node* leftMost = element->right; // go to right subtree
        node* parentOfLeftMost = element; // after shifting the leftMost, his parent cant point to him anymore

        // navigate as much as possible to left
        while(leftMost->left != NULL) {
            parentOfLeftMost = leftMost;
            leftMost = leftMost->left;
        }

        // if the root is being removed, the leftmost has to become the new root 
        if (element == tree->root) {
            tree->root = leftMost;
        }
        // else, now parent points to this element
        else {
            updateParentPointer(parentOfElement, leftMost, updateLeftOrRightParentPointer);
        }

        // the new subtree root has to point to the children of the node that's being removed
        leftMost->left = element->left; // element->left cannot be = leftMost because it starts with being = to element->right

        // but shouldnt point to itself - such as in the case [L:1, P:2, R:3] while removing P:2
        if (element->right != leftMost) { // shouldnt point to itself
            if (leftMost->right != NULL) { 
                // if this leftMost element has other elements to its right, his parent will have to inherit them
                // (it cant have more elements to its left, since its the left-most one)
                parentOfLeftMost->left = leftMost->right;
            } else {
                // parentOfLeftMost wont point to leftMost anymore since leftMost is being shifted
                parentOfLeftMost->left = NULL;
            }
            leftMost->right = element->right;
        }
    }

    printf("[OK REMOVED] %d - current nodes: %d\n", element->content, tree->nodeQnt);
    free(element);
    tree->nodeQnt--;
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

    printf("\n[PRINT PRE ORDER]\n");
    printf("[Quantity of tree nodes]: %d\n", tree->nodeQnt);
    printf("[Height of tree]: %d\n", tree->treeHeight);
    nodeNumberForPrinting = 0;
    printSubtreePreOrder(tree->root);
    nodeNumberForPrinting = 0;
}

void printSubtreePreOrder(node* subtree) { //P-L-R
    if (subtree == NULL) {
        printf("[ERROR PRINTING PreOrder - NULL SUBTREE]");
    }

    printf("> %d [height: %d] [%d]\n" , subtree->content, subtree->nodeHeight, ++nodeNumberForPrinting);

    if (subtree->left != NULL) {
        printSubtreePreOrder(subtree->left);
    }

    if (subtree->right != NULL) {
        printSubtreePreOrder(subtree->right);
    }
}

void printTreeInOrder(avlTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT IN ORDER - TREE IS NULL]\n");
        return;
    }

    printf("\n[PRINT IN ORDER]\n");
    printf("[Quantity of tree nodes]: %d\n", tree->nodeQnt);
    printf("[Height of tree]: %d\n", tree->treeHeight);
    nodeNumberForPrinting = 0;
    printSubtreeInOrder(tree->root);
    nodeNumberForPrinting = 0;
}

void printSubtreeInOrder(node* subtree) { //L-P-R
    if (subtree == NULL) {
        printf("[ERROR PRINTING InOrder - NULL SUBTREE]");
    }

    if (subtree->left != NULL) {
        printSubtreeInOrder(subtree->left);
    }

    printf("> %d [height: %d] [%d]\n" , subtree->content, subtree->nodeHeight, ++nodeNumberForPrinting);

    if (subtree->right != NULL) {
        printSubtreeInOrder(subtree->right);
    }
}

void printTreeGraphically(avlTree* tree) {
    // Not working...
    if (tree == NULL || tree->root == NULL) {
        printf("[PRINT GRAPHICALLY - TREE IS NULL]\n");
        return;
    }

    printf("[Quantity of tree nodes]: %d\n", tree->nodeQnt);
    printSubtreeGraphically(tree->root, tree->nodeQnt);
}

void printSubtreeGraphically(node* subtree, int nodeQnt) {
    // Not working...

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