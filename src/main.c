#include <stdio.h>
#include "code_generator.h"
#include "parser.h"
#include "scanner.h"

AST   ASTree; 
Token currentToken;


int main(){
    astNode * rootTree = parser();
    generate_code(rootTree);
    delete_all_allocated();
}