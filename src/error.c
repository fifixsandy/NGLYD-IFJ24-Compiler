/**
 *         Implementace překladače imperativního jazyka IFJ24.
 * 
 * @file   error.c
 * 
 * @author xnovakf00 Filip Novák
 * @date   21.11.2024
*/

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
 