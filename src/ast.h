#ifndef AST_H
#define AST_H


#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"

#define MAX_PARAM_NUMBER 256 // change

typedef enum {
   u8,
   i32,
   f64
} dataType; // DELETE FROM HERE


typedef enum {
    AST_NODE_WHILE,
    AST_NODE_IFELSE,
    AST_NODE_IF,
    AST_NODE_ELSE,

    AST_NODE_ASSIGN,
    AST_NODE_EXPR,
    AST_NODE_BINOP,
    AST_NODE_FUNC_CALL,
    AST_NODE_LITERAL,
    AST_NODE_VAR,
    AST_NODE_DEFVAR,

    AST_NODE_DEFFUNC,
    AST_NODE_RETURN,

    AST_INVALID

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
        astIf       ifNode;
        astElse     elseNode;
        astAssign   assignNode;
        astExpr     exprNode;

        astBinOp    binOpNode;
        astLiteral  literalNode;
        astVar      varNode;
        astFuncCall funcCallNode;
        astDefVar   defVarNode;

        astDefFunc  defFuncNode;
        astReturn   returnNode;
        
    }nodeRep;

}astNode;

typedef struct astWhile {
    
    bool      withNull;
    char     *id_without_null;
    astNode  *condition;
    astNode  *body;
    symtable *symtableWhile;

}astWhile;

typedef struct astIfElse {
    bool withNull;

    astNode *condition;
    astNode *ifPart;
    astNode *elsePart;

}astIfElse;

typedef struct astIf {
    
    char *id_without_null;
    symtable *symtableIf;
    astNode  *body;

}astIf;

typedef struct astElse {
    
    symtable *symtableIf;
    astNode  *body;
}astElse;

typedef struct astAssign {

    char *id;
    dataType dataT;
    astNode *expression;

}astAssign;

typedef struct astDefFunc {

    char *id;
    int paramNum;
    dataType paramTypes[MAX_PARAM_NUM];
    char *paramNames[MAX_PARAM_NUM];
    dataType returnType;
    symtable *symtableFun;
    astNode *body;

}astDefFunc;

typedef struct astReturn {

    astNode *returnExp;
    dataType returnType;

}astReturn;


typedef struct astTree { 

    astNode *root;

}astTree;

typedef struct astExpr {

    dataType dataT;
    astNode *exprTree;

}astExpr;

typedef struct astBinOp {
    
    binOpType op;
    astNode *left;
    astNode *right;
    dataType dataT;

}astBinOp;

typedef struct astLiteral {
    dataType dataT;
    union{
        float floatData;
        int   intData;
        char *charData;
    }value;
}astLiteral;

typedef struct astVar {
    dataType dataT;
    char    *id;
    symNode *symtableEntry;
}astVar;

typedef struct astFuncCall {
    dataType retType;
    char *id;
    symNode *symtableEntry;
}astFuncCall;

typedef struct astDefVar {
    char    *id;             
    astNode *initExpr;       
    symNode *symtableEntry;  
} astDefVar;


#endif