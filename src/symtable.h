/**
 * @file   symtable.h
 * 
 * @brief  Header file for a table of symbols for a compiler.
 * 
 *         File contains function declarations for working with symtable, implemented as a 
 *         AVL-balanced binary search tree (vv-BVS), such as insertion, deletion, search. It also includes 
 *         data structs used in the implementation.
 *         Another part is the stack of symtables, which represents different scopes in code. Basic operations as
 *         init, pop, push are present, but also a very important function findInStack (for more information check
 *         function documentation in symtable.c).
 *         Includes also functions for printing out .dot representation of symtable.
 * 
 * @todo   complete symData struct to handle multiple types
 * @author xnovakf00
 * @date   04.10.2024
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
   symtable         *tbPtr;
   struct stackElem *next;
}stackElem;

typedef struct stack{
   stackElem *top;
   int       elemCnt;
}stack;

/* Functions for working with symtable and stack of symtables (user) */
symtable*  createSymtable();
void       initSymtable  (symtable *tb);
void       deleteSymtable(symtable *tb);

symNode*   createSymNode (int key, symData data);
symNode*   insertSymNode (symNode *rootPtr, int key, symData data, symtable *tb);
symNode*   deleteSymNode (symNode *rootPtr, int key, symtable *tb);
symNode*   findSymNode   (symNode *rootPtr, int key);
void       freeSymNodes  (symNode *node);

void       initStack     (stack *st);
void       push          (stack *st, symtable *tb);
symtable*  pop           (stack *st);
bool       stackEmpty    (stack *st);
stackElem* createStElem  (symtable *tb);
void       freeStack     (stack *st);
bool       stackBottom   (stackElem *se);

symNode*   findInStack   (stack *st, int key);


/* Helper functions used in implementations of the above */
symNode*   minSymNode    (symNode *rootPtr);
symNode*   rRotate       (symNode *node);
symNode*   lRotate       (symNode *node);
symNode*   rebalance     (symNode *node);
int        heightVal     (symNode *node);
int        balanceVal    (symNode *node);
int        max           (int a, int b);

/* Functions for printing .dot file for debugging */
void printSymtable(FILE *file, symtable *tb);
void printNode(FILE *file, symNode *node);
/* EOF symtable.h */