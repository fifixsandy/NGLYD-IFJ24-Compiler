#include <stdio.h>

#include "error.h"
#include "symtable.h"
#include "scanner.h"


//naimplementujte funkcie ktoré by v prípade chyby boli zavolané a odstránili úplne všetko táto funkcia bude potom volaná v error.h za každým keď víde nejaká chyba


//global definition of elements struct
// použi free stack pri vymazavaní 

void delete_all_allocated(){
    freeStack(&symtableStack);
    /*
    cleanup_value(); //zo scanner.c
    freeStack();
    */
}


void lexikal_error(){
    //mam aj vypísať aktuálny lexém ktorý je chybný??
    delete_all_allocated();
    fprintf(stderr, "Error: 1,  Wrong structure of actuall lexem\n");
}

void syntax_error(){
    //mam vypísať čo je cyhbou alebo len že syntaktická chyba
    delete_all_allocated();
    fprintf(stderr, "Error: 2, Wrong syntax in program\n");
}

void semantic_error(){
    delete_all_allocated();

}


void internal_error(){
    delete_all_allocated();
    fprintf(stderr, "Error 99, Internal error of compiler\n");
}

void errorFun(int code_of_error){
    switch(code_of_error){
        case 1:
            lexikal_error();
            break;
        case 99:
            internal_error();
            break;
    }
        
}