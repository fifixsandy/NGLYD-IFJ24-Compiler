#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"

#define MAX_PARAM_NUMBER 256 // change

typedef enum {
    AST_NODE_WHILE,
    AST_NODE_BINOP,
    AST_NODE_ASSIGN,
    AST_NODE_IFELSE,
    AST_NODE_BLOCK,
    AST_NODE_DEFFUNC,
    AST_NODE_EXPR
}astNodeType;

typedef struct astNode {

    astNodeType     type;
    struct astNode *parent;
    struct astNode *next;

    union{
        astWhile  whileNode;
        astIfElse ifElseNode;
        astAssign assignNode;
        astBlock  blockNode;
    }nodeRep;

}astNode;

typedef struct astWhile {
    
    bool withNull;
    char *id_without_null;
    astNode  *condition;
    astNode  *block;

}astWhile;

typedef struct astIfElse {
    bool withNull;
    char *id_without_null;

    astNode *condition;
    astNode *ifPart;
    astNode *elsePart;

}astIfElse;

typedef struct astBlock {

    symtable *symtable;
    astNode  *bodyFirst;

}astBlock;

typedef struct astAssign {

    char *id;
    // type
    bool definiton;
    astNode *expression;

}astAssign;

typedef struct astDefFunc {

    char *id;
    // type paramTypes[MAX_PARAM_NUM];
    char *paramNames[MAX_PARAM_NUM];
    // type returnType;
    astNode *block;

}astDefFunc;

// TODO EXPRESSIONS