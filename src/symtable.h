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
 * @date   12.10.2024
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARAM_NUM 300 // this can be changed

extern stack symtableStack; /* global variable, must be initialised in exactly one .c file, 
                               accessed anywhere where symtable.h is included */ 
                           

typedef struct funData{
   int       returnType;                  // @todo represent with enums
   int       paramTypes[MAX_PARAM_NUM];   // @todo represent with enums
   char     *paramNames[MAX_PARAM_NUM];
   char     *intermediateCode;            // @todo change according to code generating
   symtable *tbPtr;
}funData;

typedef struct varData{
   int type; // @todo represent with enums
   union{
      float floatVal;
      int   intVal;
      char* stringVal;
   }value;
}varData;

 typedef struct symData{

    bool varOrFun; // 0 if var, 1 if fun
    bool isConst;
    bool defined;
    bool used;

    union{
      funData fData;
      varData vData;
    }data;

 }symData;
 
 typedef struct symNode{
    char *key;
    int height; // the maximum number of edges from the node to any of the leaves in its subtree

    symData data;

    struct symNode *r; // pointer to the right node (with greater key)
    struct symNode *l; // pointer to the left node (with lower key)
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

symNode*   createSymNode (char *key, symData data);
void       insertSymNode(symtable *tb, char *key, symData data);
void       deleteSymNode(symtable *tb, char *key);

symNode*   insertSymNodeRec (symNode *rootPtr, char *key, symData data, symtable *tb);
symNode*   deleteSymNodeRec (symNode *rootPtr,  char *key, symtable *tb);
symNode*   findSymNode   (symNode *rootPtr, char *key);
void       freeSymNodes  (symNode *node);

void       initStack     (stack *st);
void       push          (stack *st, symtable *tb);
symtable*  pop           (stack *st);
symtable*  bottom        (stack *st);
bool       stackEmpty    (stack *st);
stackElem* createStElem  (symtable *tb);
void       freeStack     (stack *st);
bool       isStackBottom   (stackElem *se);

symNode*   findInStack   (stack *st, char *key);


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