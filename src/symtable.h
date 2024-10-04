/**
 * @file symtable.h
 * 
 * @brief Header file for a table of symbols for a compiler.
 * 
 * File contains function declarations for working with symtable, implemented as a 
 * AVL-balanced binary search tree (vv-BVS), such as insertion, deletion, search. It also includes 
 * data structs used in the implementation.
 * 
 * @todo complete symData struct to handle multiple types, STACKING
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
    int height; // the maximum number of edges from the node to any of the leaves in its subtree

    symData data;

    struct symNode *r; // pointer to the right node (with greater key)
    struct symNode *l; // pointer to the right node (with lower key)
 }symNode; // structure representing the entry in the table of symbols (node of a tree)
 
 
 typedef struct symtable{
    symNode *rootPtr;
    int      nodeCnt;
 }symtable;


typedef struct stackElem{
   symtable         *tb;
   struct stackElem *next;
}stackElem;

typedef struct stack{
   stackElem *top;
   int       elemCnt;
}stack;

/* Functions for working with symtable (user) */
symtable*  createSymtable();
void       initSymtable  (symtable *tb);
void       deleteSymtable(symtable *tb);

symNode*   createSymNode (int key, symData data);
symNode*   insertSymNode (symNode *rootPtr, int key, symData data, symtable *tb);
symNode*   deleteSymNode (symNode *rootPtr, int key, symtable *tb);
symNode*   findSymNode   (symNode *rootPtr, int key);
void       freeSymNodes   (symNode *node);

void       initStack     (stack *st);
void       push          (stack *st, symtable *tb);
void       pop           (stack *st);
bool       stackEmpty    (stack *st);


/* Helper functions used in implementations of the above */
symNode*   minSymNode    (symNode *rootPtr);
symNode*   rRotate       (symNode *node);
symNode*   lRotate       (symNode *node);
symNode*   rebalance     (symNode *node);
int        heightVal     (symNode *node);
int        balanceVal    (symNode *node);
int        max           (int a, int b);

/* EOF symtable.h */