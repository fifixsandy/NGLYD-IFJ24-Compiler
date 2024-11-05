#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"

bool prog();
bool prolog();
bool code();
bool next_code();
bool def_func();
bool params();
bool params_n();
bool def_variable();
bool varorconst();
bool unused_decl();
bool type_normal();
bool type_null();
bool type();
bool type_null();
bool type_func_ret();
bool type_var_def();
bool st();
bool body();
bool return_();
bool exp_func_ret();
bool id_without_null();
bool while_statement();
bool if_statement();
bool expr_params();
bool expr_params_n();
bool after_id();
bool assign_or_f_call();
bool builtin();

bool expression();





#endif