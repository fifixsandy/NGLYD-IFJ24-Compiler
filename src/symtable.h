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
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

#define MAX_PARAM_NUM 300 // this can be changed

typedef struct symtable symtable;

typedef enum dataType{
   u8,
   i32,
   f64,
   void_,
   null_,
   unknown,
   any
} dataType;

typedef struct funData{
   bool      defined;
   dataType  returnType;                  
   bool      nullableRType;               // 1 if nullable, 0 if not
   dataType *paramTypes;
   bool    *paramNullable;
   char    **paramNames;
   int       paramNum;
   char     *intermediateCode;            // @todo change according to code generating
   symtable *tbPtr;
}funData;

typedef struct varData{
   dataType type;
   bool isConst; // 1 if const 0 if var
   bool isNullable; // 1 if nullable 0 if not
   bool inheritedType; // 1 if needs to be inherited from expression, 0 if set
   bool knownDuringCompile; // 1 if value is known during compile time, 0 if not
  
   union{
      float floatData;
      int   intData;
      char *charData;
   }value; // value in case it is known during compile time, used in expression parsing
    bool nullValue; // if it holds null
}varData;

 typedef struct symData{

    bool varOrFun; // 0 if var, 1 if fun
    bool used;
    bool changed;

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
 
 struct symtable{
    symNode *rootPtr;
    int      nodeCnt;
 };

typedef struct stackElem{
   symtable         *tbPtr;
   struct stackElem *next;
}stackElem;

typedef struct stack{
   stackElem *top;
   int       elemCnt;
}stack;

/* global variable, must be initialised in exactly one .c file, 
   accessed anywhere where symtable.h is included */
extern stack symtableStack; 

/* global variable of symtable for functions, 
   function createSymtable() should be called on it
   in exactly one file, accessible everywhere where
   symtable.h is included */
extern symtable* funSymtable;

extern symtable* builtinSymtable;
                           

/* Functions for working with symtable and stack of symtables (user) */

symtable*  createSymtable();
void       initSymtable  (symtable *tb);
void       deleteSymtable(symtable *tb);

symNode*   createSymNode (char *key, symData data);
void       insertSymNode (symtable *tb, char *key, symData data);
void       deleteSymNode (symtable *tb, char *key);

symNode*   insertSymNodeRec (symNode *rootPtr, char *key, symData data, symtable *tb);
symNode*   deleteSymNodeRec (symNode *rootPtr,  char *key, symtable *tb);
symNode*   findSymNode      (symNode *rootPtr, char *key);
void       freeSymNodes     (symNode *node);

void       initStack     (stack *st);
void       push          (stack *st, symtable *tb);
symtable*  pop           (stack *st);
symtable*  bottom        (stack *st);
bool       stackEmpty    (stack *st);
stackElem* createStElem  (symtable *tb);
void       freeStack     (stack *st);
bool       isStackBottom (stackElem *se);

symNode*   findInStack   (stack *st, char *key);


/* Helper functions used in implementations of the above */
symNode*   minSymNode    (symNode *rootPtr);
symNode*   rRotate       (symNode *node);
symNode*   lRotate       (symNode *node);
symNode*   rebalance     (symNode *node);
int        heightVal     (symNode *node);
int        balanceVal    (symNode *node);
int        max           (int a, int b);


void prepareBuiltinSymtable();

/* Functions for printing .dot file for debugging */
void printSymtable(FILE *file, symtable *tb);
void printNode(FILE *file, symNode *node);


#endif //SYMTABLE_H
/* EOF symtable.h */