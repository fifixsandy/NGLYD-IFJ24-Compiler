#include <stdio.h>

#include "error.h"
#include "symtable.h"
#include "scanner.h"


void delete_all_allocated(){
    freeStack(&symtableStack);
    /*
    cleanup_value(); //zo scanner.c
    freeStack();
    */
}
 