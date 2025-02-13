/**
 *         Implementation of IFJ24 imperative language compiler.
 * 
 * @file   ast.h
 * 
 * @brief  Header file for abstract syntactic tree (AST) representation.
 *   
 *         File contains struct declarations of AST nodes, function declarations
 *         for creating mentioned nodes and function declarations for editing and manipulating
 *         the nodes.
 *         Declarations for functions performing .dot format printing of AST are included as well.
 * 
 * @author xnovakf00 Filip Novák
 *         xfignam00 Matúš Fignár
 * 
 * @date   24.11.2024
*/

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"


typedef struct astNode astNode;

typedef enum{
    MULTIPLICATION,         //   *
    DIVISION,               //   /
    ADDITION,               //   +
    SUBSTRACTION,           //   -
    EQUAL,                  //   ==
    NOT_EQUAL,              //   !=
    LOWER,                  //   <
    GREATER,                //   >
    LOWER_OR_EQUAL,         //   <=
    GREATER_OR_EQUAL,       //   >=
    LBR,                    //   (
    RBR,                    //   )
    ID,                     //   i
    STOP,                   //   $

    NO_TERMINAL,            //   E
    ERROR                   
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


/** 
 * Next structures represent node representations based on the type of a node
 * with information specific to each type. Unclear data are commented.
*/

typedef struct astWhile {
    
    bool      withNull;
    char     *id_without_null;
    astNode  *condition;
    astNode  *body;
    symtable *symtableWhile;

}astWhile;

typedef struct astIfElse {

    bool     withNull;
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
    
    symtable *symtableElse;
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
    dataType  returnType; 
    bool      nullable;   // whether the returned expression can be null

}astDefFunc;

typedef struct astReturn {

    astNode *returnExp;
    dataType returnType;
    bool     inMain;

}astReturn;


typedef struct astExpr {

    dataType dataT;
    astNode *exprTree;
    bool     isNullable;
    bool     knownDuringCompile;

}astExpr;

typedef struct astBinOp {
    
    symbol_number op;
    astNode      *left;
    astNode      *right;
    dataType      dataT;

}astBinOp;

typedef struct astLiteral {

    dataType dataT;
    union{
        double floatData;
        int   intData;
        char *charData;
    }value;

}astLiteral;

typedef struct astVar {

    dataType dataT;
    char    *id;
    symNode *symtableEntry;
    bool isNullable;

}astVar;

typedef struct astFuncCall {

    dataType  retType;
    char     *id;
    symNode  *symtableEntry;
    bool      builtin;
    astNode **paramExpr;
    int       paramNum;
    bool      nullableRetType;

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

/**
 * @brief General representation of astNode.
 */
struct astNode {

    astNodeType     type;
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

/** Functions for creating and manipulating AST nodes and AST */

void createWhileNode(astNode *dest, bool withNull, char *id_without_null, astNode *cond, astNode *body, symtable *symtableW);
void createIfElseNode(astNode *dest, astNode *cond, astNode *ifPart, astNode *elsePart, bool withNull);
void createIfNode(astNode *dest, char *id_without_null, symtable *symtable, astNode *body);
void createElseNode(astNode *dest, symtable *symtableIf, astNode *body);
void createAssignNode(astNode *dest, char *id, astNode *expression, dataType dataT);
void createDefVarNode(astNode *dest, char *id, astNode *initExpr, symNode *symtableEntry);
void createDefFuncNode(astNode *dest, char *id, symtable *symtableFun, astNode *body, char **paranNames, int paramNum, dataType returnType, bool nullable);
void createReturnNode(astNode *dest, astNode *returnExp, dataType returnType, bool inMain);
void createBinOpNode(astNode *dest, symbol_number op, astNode *left, astNode *right, dataType dataT);
void createLiteralNode(astNode *dest, dataType dataT, void *value);
void createVarNode(astNode *dest, char *id, dataType dataT, symNode *symtableEntry);
void createFuncCallNode(astNode *dest, char *id, dataType retType, bool builtin, symNode *symtableEntry, astNode **exprParams, int paramNum, bool isNullable);
void createUnusedNode(astNode *dest, astNode *expr);
void createExpressionNode(astNode *dest, dataType type, astNode *exprRoot, bool isNullable, bool duringCompile);
astNode *createRootNode();

void connectToBlock(astNode *toAdd, astNode *blockRoot);
void freeASTNode(astNode *node);

astNode *createAstNode();

/** Functions for printing .dot representation of AST */

void printASTree(FILE *file, astNode *tree);
void printASTNode(FILE *file, astNode *node);
void printASTNodeLabel(FILE *file, astNode *node);
void printASTEdges(FILE *file, astNode *node);
void printASTNext(FILE *file, astNode *node);


void printIdWithoutNull(FILE *file, bool null, char *id);
void printLiteralInfo(FILE *file, astLiteral node);
void printDefFuncInfo(FILE *file, astDefFunc node);
void printBinopType(FILE *file, symbol_number type);
void printDefVarInfo(FILE *file, astDefVar node);


#endif //AST_H

/* EOF ast.h */