#ifndef AST_H
#define AST_H


#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"

#define MAX_PARAM_NUMBER 256 // change

typedef enum {
    AST_NODE_WHILE,
    AST_NODE_BINOP,
    AST_NODE_ASSIGN,
    AST_NODE_IFELSE,
    AST_NODE_ROOT,
    AST_NODE_DEFFUNC,
    AST_NODE_EXPR
}astNodeType;

typedef enum {
    BO_PLUS,
    BO_MINUS,
    BO_MUL,
    BO_DIV
}binOpType;

typedef struct astNode {

    astNodeType     type;
    struct astNode *parent;
    struct astNode *next;

    union{
        astWhile  whileNode;
        astIfElse ifElseNode;
        astAssign assignNode;
        astRoot  rootNode;
    }nodeRep;

}astNode;

typedef struct astWhile {
    
    bool withNull;
    char *id_without_null;
    astNode  *condition;
    astNode  *body;

}astWhile;

typedef struct astIfElse {
    bool withNull;
    char *id_without_null;

    astNode *condition;
    astNode *ifPart;
    astNode *elsePart;

}astIfElse;

typedef struct astRoot {

    symtable *symtable;
    astNode  *bodyFirst;

}astRoot;

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


typedef struct astTree { 

    astNode *root;

}astTree;


typedef struct astBinOp {
    
    binOpType op;
    astNode *left;
    astNode *right;

}astBinOp;

typedef struct astConst {
    // type
    union{
        float floatData;
        int   intData;
        char *charData;
    }value;
}astConst;

typedef struct astVar {
    // type
    char *id;
}astVar;

typedef struct astFuncCall {
    // ret type
    char *id;
}astFuncCall;


#endif