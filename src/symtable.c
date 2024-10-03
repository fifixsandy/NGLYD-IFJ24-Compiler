#include "symtable.h"
 

symtable* createSymtable(){
    symtable *table = (symtable *)malloc(sizeof(symtable));
    if(table == NULL){
        // TODO HANDLE ERROR;
        return NULL;
    }
    initSymtable(table);
    return table;
}

void initSymtable(symtable *symtable){
    symtable->nodeCnt = 0;
    symtable->rootPtr = NULL;
}

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

symNode *insertSymNode(symNode *rootPtr, int key, symData data){
    if(rootPtr == NULL){
        return createSymNode(key, data);
    }
    else{
        if(key < rootPtr->key){
            rootPtr->l = insertSymNode(rootPtr->l, key, data);
        }
        else if(key > rootPtr->key){
            rootPtr->r = insertSymNode(rootPtr->r, key, data);
        }
        else{
            rootPtr->data = data;
        }
    }
    rootPtr->height = max(heightVal(rootPtr->l),heightVal(rootPtr->r)) + 1;
    int balance = balanceVal(rootPtr);
    if(balance < -1){
        if(balanceVal(rootPtr->l) < -1){ // CHECK 0 OR 1
            rRotate(rootPtr);
        }
        else if(balanceVal(rootPtr->l > 1)){ // CHECK 0 OR 1
            lRotate(rootPtr);
            rRotate(rootPtr);
        }
    }
    else if(balance > 1){
        if(balanceVal(rootPtr->r > 1)){ // CHECK 0 OR 1
            lRotate(rootPtr);
        }
        else if(balanceVal(rootPtr->r < -1)){ // CHECK 0 OR 1
            rRotate(rootPtr);
            lRotate(rootPtr);
        }
    }

    return rootPtr;
}

symNode *deleteSymNode(symNode *rootPtr, int key){
    if(rootPtr == NULL){
        return NULL;
    }

    if(key < rootPtr->key){ // goes to left
        rootPtr->l = deleteSymNode(rootPtr->l, key);
        return rootPtr;
    }
    else if(key > rootPtr->key){ // goes to right
        rootPtr->r = deleteSymNode(rootPtr->r, key);
    }
    else{ // found
        if(rootPtr->r == NULL && rootPtr->l == NULL){ // no children
            free(rootPtr);
            return NULL;
            // TODO add -- of number of nodes
        }
        else if(rootPtr->r != NULL && rootPtr->l != NULL){ // both children
            symNode *min  = minSymNode(rootPtr->r);
            rootPtr->key  = min->key;
            rootPtr->data = min->data;
            rootPtr->r    = deleteSymNode(rootPtr->r, min->key);
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
            // TODO add -- of number of nodes
            return tmp;
        }
    }
    rootPtr->height = max(heightVal(rootPtr->l),heightVal(rootPtr->r)) + 1;
    int balance = balanceVal(rootPtr);
    if(balance < -1){
        if(balanceVal(rootPtr->l) < -1){
            rRotate(rootPtr);
        }
        else if(balanceVal(rootPtr->l > 1)){
            lRotate(rootPtr);
            rRotate(rootPtr);
        }
        else{
            rRotate(rootPtr);
        }
    }
    else if(balance > 1){
        if(balanceVal(rootPtr->r > 1)){
            lRotate(rootPtr);
        }
        else if(balanceVal(rootPtr->r < -1)){
            rRotate(rootPtr);
            lRotate(rootPtr);
        }
        else{
            lRotate(rootPtr);
        }
    }
    return rootPtr;
}

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

// helper functions
symNode *minSymNode(symNode *rootPtr){
    if(rootPtr->l == NULL){
        return rootPtr;
    }
    else{
        return(minSymNode(rootPtr->l));
    }
}

symNode *rRotate(symNode *node){

    symNode *x = node->l;
    node->l    = x->r;
    x->r       = node;

    node->height = max(heightVal(node->l), heightVal(node->r)) + 1;
    x->height    = max(heightVal(x->l), heightVal(x->r)) + 1;
    
    return x;
}

symNode *lRotate(symNode *node){

    symNode *x = node->r; 
    node->r    = x->l;     
    x->l       = node;

    node->height = max(heightVal(node->l), heightVal(node->r)) + 1;
    x->height    = max(heightVal(x->l), heightVal(x->r)) + 1;

    return x;
}

int heightVal(symNode *node){
    return(node->height);
}

int balanceVal(symNode *node){
    return(heightVal(node->r) - heightVal(node->l));
}

int max(int a, int b){
    return(a > b) ? a : b;
}