#include <stdio.h>
#include "code_generator.h"
#include "parser.h"
#include "scanner.h"

AST   ASTree; 
Token currentToken;


int main(){
    initStack(&symtableStack);
    funSymtable = createSymtable();
    input_file = fopen("file.txt", "r");
    GT
    prog();
    //DEBPRINT("%d\n", prog());
    //DEBPRINT("\n\n");

    astNode * rootTree = ASTree.root;

    generate_code(rootTree);
}