#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
 

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
    if(balance == -2){
        //TODO rotations
    }
    if(balance == 2){
        //TODO rotations
    }

    return rootPtr;
}
