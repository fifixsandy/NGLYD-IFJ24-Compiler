/**
 * @file symtable.c
 * 
 * @brief AVL-balanced binary search (vv-BVS) tree implementation of a table of symbols for a compiler.
 * 
 * File contains implementation for functions declared in symtable.h. Table of symbols is
 * implemented as a height-balanced binary search tree. Most of the functions are implemented recursively.
 * The tree balances when inserting or deleting a node, when necessary using rotations.
 * 
 * @todo Error handling, STACKING
 * @author xnovakf00
 * @date 03.10.2024
*/

#include "symtable.h"


/**
 * @brief  Creates an empty symtable.
 * 
 * @return Initialised table if allocation is successful, NULL otherwise.
 * @todo   Error handling
 */
symtable* createSymtable(){
    symtable *table = (symtable *)malloc(sizeof(symtable));
    if(table == NULL){
        // TODO HANDLE ERROR;
        return NULL;
    }
    initSymtable(table);
    return table;
}

/**
 * @brief Initialises symtable for further work.
 * 
 * @param symtable Symtable to be initialised.
 * @todo Error handling
 */

void initSymtable(symtable *tb){
    tb->nodeCnt = 0;
    tb->rootPtr = NULL;
}

/**
 * @brief Creates an element of a symtable, symNode with key and data.
 * 
 * @param key  Key on which the node will be located in the tree.
 * @param data Extra useful data describing the symbol element.
 * 
 * @todo Error handling.
 * 
 * @return Pointer to the created symNode.
 */
symNode *createSymNode(int key, symData data){
    symNode *newNode = (symNode *)malloc(sizeof(symNode));
    if(newNode == NULL){
        // TODO HANDLE ERROR;
        return NULL;
    }

    newNode->data   = data;
    newNode->key    = key;
    newNode->height = 0;
    newNode->l      = NULL;
    newNode->r      = NULL;

    return newNode;
}

/**
 * @brief Inserts a new node into a symtable.
 * 
 * When there is no node in the symtable with the same key as the inserted node, it is
 * created and added. If the tree includes a node with the same key, the data is rewritten.
 * After inserting, rotations are performed, if necessary to keep the tree balanced.
 * It is implemented recursively.
 * The pointer to the root node is necessary for recursion, that is why there is not just the 
 * pointer to the whole tree.
 * 
 * @param rootPtr Pointer to the root of the tree (subtree if in recursion).
 * @param key     Key of the node to be inserted.
 * @param data    Data for the node.
 * @param tb      Pointer to a symtable for increasing the count.
 * 
 * @return        The root of the updated tree (subtree) after insertion.
 * 
 */
symNode *insertSymNode(symNode *rootPtr, int key, symData data, symtable *tb){
    if(rootPtr == NULL){
        tb->nodeCnt++;
        return createSymNode(key, data);
    }
    else{
        if(key < rootPtr->key){ // going left
            rootPtr->l = insertSymNode(rootPtr->l, key, data, tb);
        }
        else if(key > rootPtr->key){ // going right
            rootPtr->r = insertSymNode(rootPtr->r, key, data, tb);
        }
        else{ // keys are identical, data rewrite
            rootPtr->data = data;
        }
    }

    rootPtr->height = max(heightVal(rootPtr->l),heightVal(rootPtr->r)) + 1;

    return rebalance(rootPtr);
}

/**
 * @brief Deletes a node from the symtable (AVL tree).
 * 
 * If a node with the specified key exists, it is removed from the tree and the tree is reconstructed,
 * if the deleted node had children. 
 * After deletion, rotations are performed if necessary.
 * 
 * Similar to insertion, the function is implemented recursively, requiring the pointer 
 * to the root node to traverse the tree. 
 * 
 * @param rootPtr Pointer to the root of the tree (or subtree).
 * @param key     Key of the node to be deleted.
 * @param tb      Pointer to a symtable for decreasing the count.
 * 
 * @return        The new root of the balanced tree after deletion.
 */

symNode *deleteSymNode(symNode *rootPtr, int key, symtable *tb){
    if(rootPtr == NULL){
        return NULL;
    }

    if(key < rootPtr->key){ // going left
        rootPtr->l = deleteSymNode(rootPtr->l, key, tb);
        return rootPtr;
    }
    else if(key > rootPtr->key){ // going right
        rootPtr->r = deleteSymNode(rootPtr->r, key, tb);
    }
    else{ // found
        if(rootPtr->r == NULL && rootPtr->l == NULL){ // no children
            free(rootPtr);
            tb->nodeCnt--;
            return NULL;
        }
        else if(rootPtr->r != NULL && rootPtr->l != NULL){ // both children
            symNode *min  = minSymNode(rootPtr->r);
            rootPtr->key  = min->key;
            rootPtr->data = min->data;
            rootPtr->r    = deleteSymNode(rootPtr->r, min->key, tb);
            return rootPtr;
        }
        else{ // only one child
            symNode *tmp = NULL;
            if(rootPtr->l == NULL){
                tmp = rootPtr->r;
            }
            else{
                tmp = rootPtr->l;
            }
            free(rootPtr);
            tb->nodeCnt--;
            return tmp;
        }
    }

    rootPtr->height = max(heightVal(rootPtr->l),heightVal(rootPtr->r)) + 1;

    return rebalance(rootPtr);
}

/**
 * @brief Recursively finds a node in the symtable based on the key.
 * 
 * As with the insertSymNode and deleteSymNode, the root is needed.
 * 
 * @param rootPtr Pointer to the tree (subtree) for the node to be found.
 * @param key     Key of the node to be found.
 * 
 * @return If the node is found, the pointer to it. If the node does not exist, NULL.
 */
symNode *findSymNode(symNode *rootPtr, int key){
    if(rootPtr == NULL){
        return NULL;
    }
    else{
        if(rootPtr->key != key){
            if(rootPtr->key > key){
                return(findSymNode(rootPtr->l, key));
            }
            else{
                return(findSymNode(rootPtr->r, key));
            }
        }
        else{
            return rootPtr;
        }
    }
}


/**
 * @brief      Frees all nodes in the tree starting from the given node.
 * 
 * @param node Pointer to the root of the subtree to be freed.
 * 
 * @warning    Don't confuse it with deleteSymNode. This function does not rebalance the tree
 *             but frees all nodes in it. Should only be used when removing the tree.
 */
void freeSymNodes(symNode *node){
    if(node == NULL){
        return;
    }
    freeSymNodes(node->r);
    freeSymNodes(node->l);
    free(node);
}

/**
 * @brief Frees the symbol table and all its nodes.
 * 
 * @param tb Pointer to the symbol table to be deleted.
 */
void deleteSymtable(symtable *tb){
    if(tb == NULL){
        return;
    }
    freeSymNode(tb->rootPtr);
    free(tb);
}
/************************************************************************************************************** 
                                    SECTION Helper functions 
                         Used for implementation of the above functions
                      (if you are just using the ADT, you can ignore these)
*************************************************************************************************************/

/**
 * @brief Finds the leftenmost node in a subtree, the node with the smallest key.
 * 
 * @param rootPtr Pointer to the root of the subtree.
 * 
 * @see deleteSymNode
 * 
 * @return Leftenmost node.
 */
symNode *minSymNode(symNode *rootPtr){
    if(rootPtr->l == NULL){
        return rootPtr;
    }
    else{
        return(minSymNode(rootPtr->l));
    }
}

/**
 * @brief Performs right-rotation of the subtree from the node.
 * 
 * Used for rebalancing the tree if the left side is heavier than the right.
 * 
 * @param node Pointer to the critical node around which the rotation is performed.
 * 
 * @see symNodeInsert, symNodeDelete
 * 
 * @return Pointer to a new root of a rotated subtree.
 */
symNode *rRotate(symNode *node){

    symNode *x = node->l;
    node->l    = x->r;
    x->r       = node;

    node->height = max(heightVal(node->l), heightVal(node->r)) + 1;
    x->height    = max(heightVal(x->l), heightVal(x->r)) + 1;
    
    return x;
}

/**
 * @brief Performs left-rotation of the subtree from the node.
 * 
 * Used for rebalancing the tree if the righht side is heavier than the right.
 * 
 * @param node Pointer to the critical node around which the rotation is performed.
 * 
 * @see symNodeInsert, symNodeDelete
 * 
 * @return Pointer to a new root of a rotated subtree.
 */
symNode *lRotate(symNode *node){

    symNode *x = node->r; 
    node->r    = x->l;     
    x->l       = node;

    node->height = max(heightVal(node->l), heightVal(node->r)) + 1;
    x->height    = max(heightVal(x->l), heightVal(x->r)) + 1;

    return x;
}

/**
 * @brief Gets the height of a node.
 * 
 * Height is the maximum number of edges from the node to any of leaf nodes in its subtree.
 * 
 * @see balanceVal
 * 
 * @param node Pointer to a node we want height of.
 * 
 * @return Height of the node or 0 if node does not exist.
 */
int heightVal(symNode *node){
    if(node == NULL){
        return 0;
    }
    return(node->height);
}

/**
 * @brief Calculates the balance of a node.
 * 
 * Balance is the difference between the height of the right and the left node.
 * If its negative, the node is heavier on the left, if positive, on the right and if 0 it is balanced.
 * In AVL height-balanced trees, the height of each node should be <-1,1>. If it is out of these bounds, 
 * rotation is required.
 * 
 * @param node Node to check the balance of.
 * 
 * @return Balance number based on the heights of the children.
 */
int balanceVal(symNode *node){
    return(heightVal(node->r) - heightVal(node->l));
}

/**
 * @brief Rebalances the node according to AVL tree balancing rules
 * 
 * This function firstly checks, whether the rotations for rebalancing are needed. If yes,
 * it rotates the nodes according to 4 different scenarios: LL, LR, RR, RL.
 * 
 * @param node Pointer to a node to be rebalanced.
 * 
 * @return New root after rebalancing.
 */
symNode *rebalance(symNode *node){
    int balance = balanceVal(node);
    if(balance < -1){ // The node is left-heavy (out of <-1,1> bounds)
        if(balanceVal(node->l) <= 0){ // Left child is left-heavy (or balanced after deletion) -> LL
           return rRotate(node);
        }
        else if(balanceVal(node->l) > 0){ // Left child is right-heavy -> LR
            node->l = lRotate(node->l);
            return rRotate(node);
        }
    }
    else if(balance > 1){ // The node is right-heavy (out of <-1,1> bounds)
        if(balanceVal(node->r) >= 0){ // Right child is right-heavy (or balanced after deletion) -> RR
            return lRotate(node);
        }
        else if(balanceVal(node->r) < 0){ // Right child is right-heavy -> RL
            node->r = rRotate(node->r);
            return lRotate(node);
        }
    }

    return node; // The node is balanced
}


int max(int a, int b){
    return(a > b) ? a : b;
}

/* EOF symtable.c */