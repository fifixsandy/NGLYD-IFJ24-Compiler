#ifndef ERROR_H
#define ERROR_H

#include "symtable.h"
#include "scanner.h"


/*
controls all allocations in program and deletes everything
*/
void delete_all_allocated();
void lexikal_error();
void syntax_error();
void semantic_error();
void intern_error();
void errorFun();

#endif //ERROR_H