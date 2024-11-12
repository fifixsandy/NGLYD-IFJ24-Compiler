#include <stdio.h>
#include "code_generator.h"
#include "parser.h"

int main(){
    funSymtable = createSymtable();
    input_file = fopen("file.txt", "r");
    prog();
    astNode * rootTree = ASTree.root;
    generate_code(rootTree);
}