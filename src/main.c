#include <stdio.h>
#include "code_generator.h"
#include "parser.h"
#include "scanner.h"

AST   ASTree; 
Token currentToken;


int main(){

    
    input_file = fopen("file.txt", "r");
    astNode * rootTree = parser();
    generate_code(rootTree);
}