/**
 * @file   ast.h
 * 
 * @brief  Header file for abstract syntactic tree representation.
 * 
 * 
 * @author xnovakf00
 * @date   08.11.2024
*/

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"


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

    AST_NODE_ROOT, // only contains next node, everything else is invalid 

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
    struct astNode *parent;  // in which block it is
    struct astNode *next;   // next in block of code

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
        
    }nodeRep;// exact representaion of the current node

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
    
    char     *id_without_null;
    symtable *symtableIf;
    astNode  *body;

}astIf;

typedef struct astElse {
    
    symtable *symtableIf;
    astNode  *body;

}astElse;

typedef struct astAssign {

    char    *id;
    dataType dataT;
    astNode *expression;

}astAssign;

typedef struct astDefFunc {

    char     *id;
    symtable *symtableFun;
    astNode  *body;

}astDefFunc;

typedef struct astReturn {

    astNode *returnExp;
    dataType returnType;

}astReturn;


typedef struct astExpr {

    dataType dataT;
    astNode *exprTree;

}astExpr;

typedef struct astBinOp {
    
    binOpType op;
    astNode  *left;
    astNode  *right;
    dataType  dataT;

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
    char    *id;
    symNode *symtableEntry;
}astFuncCall;

typedef struct astDefVar {
    char    *id;             
    astNode *initExpr;       
    symNode *symtableEntry;  
} astDefVar;

typedef struct AST { 

    astNode *root;

}AST;

astNode *createWhileNode(bool withNull, char *id_without_null, astNode *cond, astNode *body, symtable *symtableW, astNode *parent);
astNode *createIfElseNode(astNode *cond, astNode *ifPart, astNode *elsePart, bool withNull, astNode *parent);
astNode *createIfNode(char *id_without_null, symtable *symtable, astNode *body, astNode *parent);
astNode *createElseNode(symtable *symtableIf, astNode *body, astNode *parent);
astNode *createAssignNode(char *id, astNode *expression, astNode *parent, dataType dataT);
astNode *createDefVarNode(char *id, astNode *initExpr, symtable *symtableEntry, astNode *parent);
astNode *createDefFuncNode(char *id, symtable *symtableFun, astNode *body, astNode *parent);
astNode *createReturnNode(astNode *returnExp, dataType returnType, astNode *parent);
astNode *createBinOpNode(binOpType op, astNode *left, astNode *right, dataType dataT, astNode *parent);
astNode *createLiteralNode(dataType dataT, void *value, astNode *parent);
astNode *createVarNode(char *id, dataType dataT, symNode *symtableEntry, astNode *parent);
astNode *createFuncCallNode(char *id, dataType retType, symNode *symtableEntry, astNode *parent);

void connectToBlock(astNode *toAdd, astNode *blockRoot);
void addNext(astNode *prev, astNode *next);
void freeASTNode(astNode *node);

astNode *createAstNode();


#endif

/* EOF ast.h */