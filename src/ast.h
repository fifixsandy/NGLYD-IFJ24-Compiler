#ifndef AST_H
#define AST_H


#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"

#define MAX_PARAM_NUMBER 256 // change

typedef enum {
    AST_NODE_WHILE,
    AST_NODE_IFELSE,

    AST_NODE_ASSIGN,
    AST_NODE_EXPR,
    AST_NODE_BINOP,
    AST_NODE_FUNC_CALL,
    AST_NODE_LITERAL,
    AST_NODE_VAR,

    AST_NODE_DEFFUNC,
    AST_NODE_RETURN,

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

        astWhile    whileNode;
        astIfElse   ifElseNode;
        astAssign   assignNode;
        astExpr     exprNode;

        astBinOp    binOpNode;
        astLiteral  literalNodel;
        astVar      varNode;
        astFuncCall funcCallNode;
        
        astDefFunc  defFuncNode;
        astReturn   returnNode;
        
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


typedef struct astAssign {

    char *id;
    // type
    bool definiton;
    astNode *expression;

}astAssign;

typedef struct astDefFunc {

    char *id;
    int paramNum;
    // type paramTypes[MAX_PARAM_NUM];
    char *paramNames[MAX_PARAM_NUM];
    // type returnType;
    symtable *symtableFun;
    astNode *body;

}astDefFunc;

typedef struct astReturn {

    astNode *returnExp;
    //type returnType;

}astReturn;


typedef struct astTree { 

    astNode *root;

}astTree;

typedef struct astExpr {

    // type type;
    astNode *exprTree;

}astExpr;

typedef struct astBinOp {
    
    binOpType op;
    astNode *left;
    astNode *right;

}astBinOp;

typedef struct astLiteral {
    // type
    union{
        float floatData;
        int   intData;
        char *charData;
    }value;
}astLiteral;

typedef struct astVar {
    // type
    char    *id;
    symNode *symtableEntry;
}astVar;

typedef struct astFuncCall {
    // ret type
    char *id;
    symNode *symtableEntry;
}astFuncCall;


#endif