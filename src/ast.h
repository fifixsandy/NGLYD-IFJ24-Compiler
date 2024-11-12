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


typedef struct astNode astNode;

typedef enum{
    MULTIPLICATION,         // 0
    DIVISION,               // 1
    ADDITION,               // 2
    SUBSTRACTION,           // 3
    EQUAL,                  // 4
    NOT_EQUAL,              // 5
    LOWER,                  // 6
    GREATER,                // 7
    LOWER_OR_EQUAL,         // 8
    GREATER_OR_EQUAL,       // 9
    LBR,                    // 10
    RBR,                    // 11

    ID,                     // 12
    STOP,                   // 13
    NO_TERMINAL,            // 14 - neterminálny symbol, pri vyhodnocovaní skip
    ERROR                   // 15
} symbol_number;


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
    AST_UNUSED,

    AST_NODE_DEFFUNC,
    AST_NODE_RETURN,

    AST_NODE_ROOT, // only contains next node, everything else is invalid 

    AST_INVALID

}astNodeType;

typedef enum {
    BO_MUL,
    BO_DIV,
    BO_PLUS,
    BO_MINUS,
    BO_EQL,
    BO_NOT_EQL,
    BO_LWR,
    BO_GRT
}binOpType;

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
    char    **paramNames;  // added for easier access for codegen
    int       paramNum; 

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
    
    symbol_number op;
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
    bool     builtin;
}astFuncCall;

typedef struct astDefVar {
    char    *id;             
    astNode *initExpr;       
    symNode *symtableEntry;  
} astDefVar;

typedef struct astUnused {

    astNode *expr;
    
}astUnused;


typedef struct AST { 

    astNode *root;

}AST;


struct astNode {

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
        astUnused   unusedNode;

        astDefFunc  defFuncNode;
        astReturn   returnNode;
        
    }nodeRep;// exact representaion of the current node

};



void createWhileNode(astNode *dest, bool withNull, char *id_without_null, astNode *cond, astNode *body, symtable *symtableW, astNode *parent);
void createIfElseNode(astNode *dest, astNode *cond, astNode *ifPart, astNode *elsePart, bool withNull, astNode *parent);
void createIfNode(astNode *dest, char *id_without_null, symtable *symtable, astNode *body, astNode *parent);
void createElseNode(astNode *dest, symtable *symtableIf, astNode *body, astNode *parent);
void createAssignNode(astNode *dest, char *id, astNode *expression, astNode *parent, dataType dataT);
void createDefVarNode(astNode *dest, char *id, astNode *initExpr, symNode *symtableEntry, astNode *parent);
void createDefFuncNode(astNode *dest, char *id, symtable *symtableFun, astNode *body, astNode *parent, char **paranNames, int paramNum);
void createReturnNode(astNode *dest, astNode *returnExp, dataType returnType, astNode *parent);
void createBinOpNode(astNode *dest, symbol_number op, astNode *left, astNode *right, dataType dataT, astNode *parent);
void createLiteralNode(astNode *dest, dataType dataT, void *value, astNode *parent);
void createVarNode(astNode *dest, char *id, dataType dataT, symNode *symtableEntry, astNode *parent);
void createFuncCallNode(astNode *dest, char *id, dataType retType, bool builtin, symNode *symtableEntry, astNode *parent);
void createUnusedNode(astNode *dest, astNode *expr, astNode *parent);
void createExpressionNode(astNode *dest, dataType type, astNode *exprRoot);
astNode *createRootNode();

void connectToBlock(astNode *toAdd, astNode *blockRoot);
void addNext(astNode *prev, astNode *next);
void freeASTNode(astNode *node);

astNode *createAstNode();


#endif

/* EOF ast.h */