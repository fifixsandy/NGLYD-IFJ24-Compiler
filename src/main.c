/**
 *         Implementace překladače imperativního jazyka IFJ24.
 * 
 * @file   main.c
 * 
 * @brief  This file encapsulates basic logic of compiler of IFJ24.
 * 
 * @author xnovakf00 Filip Novák
 * @author xmalegt00 Tibor Malega
 * @author xskovaj00 Ján Skovajsa
 * @author xfignam00 Matuš Fignár
 * @date   21.11.2024
*/

#include <stdio.h>
#include "code_generator.h"
#include "parser.h"
#include "scanner.h"

AST   ASTree; 
Token currentToken;

int main(){
    ASTree.root = parser();
    generate_code(ASTree.root);
    delete_all_allocated();
}

/* EOF main.c */