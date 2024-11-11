#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"
#include "symtable.h"
#include "ast.h"

bool prog();
bool prolog();
bool code();
bool next_code();
bool def_func();
bool params(int *paramNum, dataType **paramTypes, char ***paramNames);
bool params_n(int *paramNum, dataType **paramTypes, char ***paramNames);
bool def_variable(astNode *block);
bool varorconst(bool *isConst);
bool unused_decl(astNode *block);
bool type_normal(dataType *datatype);
bool type_null(dataType *datatype);
bool type(bool *nullable, dataType *datatype);
bool type_null(dataType *datatype);
bool type_func_ret(bool *nullable, dataType *datatype);
bool type_var_def(bool *nullable, dataType *datatype, bool *inheritedDType);
bool st(dataType expRetType, astNode *block);
bool body(dataType returnType, astNode *block);
bool return_(dataType expReturnType, astNode *block);
bool exp_func_ret(dataType expRetType, astNode *expr);
bool id_without_null(bool *withNull, char **id_wout_null);
bool while_statement(dataType expRetType, astNode *block);
bool if_statement(dataType expRetType, astNode *block);
bool expr_params();
bool expr_params_n();
bool after_id();
bool assign_or_f_call();
bool builtin();

bool expression();

bool mainDefined();

#endif