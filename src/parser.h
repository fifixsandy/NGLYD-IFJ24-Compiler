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
bool params();
bool params_n();
bool def_variable();
bool varorconst(bool *isConst);
bool unused_decl();
bool type_normal(dataType *datatype);
bool type_null(dataType *datatype);
bool type(bool *nullable, dataType *datatype);
bool type_null(dataType *datatype);
bool type_func_ret(bool *nullable, dataType *datatype);
bool type_var_def(bool *nullable, dataType *datatype, bool *inheritedDType);
bool st(dataType expRetType);
bool body(dataType returnType);
bool return_(dataType expReturnType);
bool exp_func_ret(dataType expRetType);
bool id_without_null();
bool while_statement(dataType expRetType);
bool if_statement(dataType expRetType);
bool expr_params();
bool expr_params_n();
bool after_id();
bool assign_or_f_call();
bool builtin();

bool expression();


#endif