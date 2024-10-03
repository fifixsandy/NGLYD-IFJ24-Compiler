#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
 

symtable* createSymtable(){
    symtable *table = malloc(sizeof(symtable));
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

