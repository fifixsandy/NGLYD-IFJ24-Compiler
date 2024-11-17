#include <stdio.h>

#include "error.h"
#include "symtable.h"
#include "scanner.h"
#include "parser.h"


void delete_all_allocated(){

    freeASTNode(ASTree.root);
    deleteSymtable(builtinSymtable);
    deleteSymtable(funSymtable);
    free_all_values();
    
}
 