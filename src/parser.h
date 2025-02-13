/**
 *         Implementation of IFJ24 imperative language compiler.
 * 
 * @file   parser.h
 * 
 * @brief  Function declarations, macros and extern declarations for a recursive
 *         parser based on LL1 grammar.
 * 
 * @author xnovakf00 Filip Novák
 * @date   21.11.2024
*/


#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"
#include "symtable.h"
#include "ast.h"
#include "error.h"
#include "expression_parser.h"

//#define DEBUG
#ifdef DEBUG
    #define DEBPRINT(...) \
        fprintf(stderr, "D: %s, %d: ", __func__ , __LINE__); \
        fprintf(stderr, __VA_ARGS__);
#else
    #define DEBPRINT(...) 
#endif

#define GT currentToken = getToken(); // encapsulating the assignment

extern Token currentToken; // last token produced by scanner
extern AST   ASTree;       // AST for the whole program


astNode *parser();

/* LL1 NON-TERMINALS AS FUNCTIONS */

bool prog(bool firstTraverse);
bool prolog();
bool code(bool firstTraverse);
bool def_func(bool firstTraverse);
bool def_func_first(symNode *functionEntry, char *funID);
bool def_func_sec(symNode *functionEntry, char *funID);
bool params(int *paramNum, dataType **paramTypes, char ***paramNames, bool **paramNullable);
bool params_n(int *paramNum, dataType **paramTypes, char ***paramNames, bool **paramNullable);
bool def_variable(astNode *block);
bool varorconst(bool *isConst);
bool unused_decl(astNode *block);
bool type_normal(dataType *datatype);
bool type_null(dataType *datatype);
bool type(bool *nullable, dataType *datatype);
bool type_null(dataType *datatype);
bool type_func_ret(bool *nullable, dataType *datatype);
bool type_var_def(bool *nullable, dataType *datatype, bool *inheritedDType);
bool st(dataType expRetType, bool nullableRType, astNode *block, bool inMain);
bool body(dataType returnType,bool nullableRType, astNode *block, bool inMain);
bool return_(dataType expReturnType,bool nullableRType, astNode *block, bool inMain);
bool exp_func_ret(dataType expRetType,bool nullableRType, astNode **expr);
bool id_without_null(bool *withNull, char **id_wout_null);
bool while_statement(dataType expRetType,bool nullableRType, astNode *block, bool inMain);
bool if_statement(dataType expRetType, bool nullableRType, astNode *block, bool inMain);
bool expr_params(astNode **params, int *paramCnt);
bool expr_params_n(astNode **params, int *paramCnt);
bool after_id(char *id, astNode *block);
bool assign_or_f_call(astNode *block);
bool builtin(char *id, symNode **symtableNode, bool *builtinCall, char **betterID);

void funCallHandle(char *id, astNode *node, bool inExpr);
void assignmentHandle(char *id, astNode *block);

/* HELPER FUNCTIONS */

bool     mainDefined();
void     allUsed(symNode *root);
bool     wasDefined(char *ID, symNode **node);
dataType getReturnType(char *ID);
dataType getVarType(char *ID);
bool     checkParameterTypes(dataType *expected, astNode **given, int paramNum, int *badIndex);
bool     checkParameterNullability(bool *expected, astNode **given, int paramNum, int *badIndex);
symNode *checkBuiltinId(char *id);
bool     checkIfNullable(astNode *expr);
bool     checkIfExprLogic(astNode *expr);
void     extractValueToConst(dataType exprType, astNode *exprTree, varData *variData);
bool     allReturns(astNode *statement);


#endif //PARSER_H

/* EOF parser.h */