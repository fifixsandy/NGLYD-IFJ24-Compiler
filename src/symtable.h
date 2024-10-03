/**
 * @file symtable.h
 * 
 * @brief Header file for a table of symbols for a compiler.
 * 
 * File contains function declarations for working with symtable, implemented as a 
 * AVL-balanced binary search tree, such as insertion, deletion, search. It also includes 
 * data structs used in the implementation.
 * 
 * @todo complete symData struct to handle multiple types.
 * @author xnovakf00
 * @date 03.10.2024
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


 typedef struct symData{
    // @todo add whether float/int/string...
    bool isConst;
    union{
        int   intData;
        float floatData;
        char *stringData;
    }value;
 }symData;
 
 
 typedef struct symNode{
    int key;
    int height;

    symData data;

    struct symNode *r;
    struct symNode *l;
 }symNode;
 
 
 typedef struct symtable{
    symNode *rootPtr;
    int      nodeCnt;
 }symtable;


/* Functions for working with symtable (user)*/
symtable*  createSymtable();
void       initSymtable  (symtable *symtable);

symNode*   createSymNode (int key, symData data);
symNode*   insertSymNode (symNode *rootPtr, int key, symData data);
symNode*   deleteSymNode (symNode *rootPtr, int key);
symNode*   findSymNode   (symNode *rootPtr, int key);

/* Helper functions used in implementations of the above */
symNode*   minSymNode    (symNode *rootPtr);
symNode*   rRotate       (symNode *node);
symNode*   lRotate       (symNode *node);
int        heightVal     (symNode *node);
int        balanceVal    (symNode *node);
int        max           (int a, int b);

/* EOF symtable.h */