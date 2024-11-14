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
    prepareBuiltinSymtable();
    GT
    //prog();
    DEBPRINT("%d\n", prog(true));
    printf("INHERE %d\n", findSymNode(funSymtable->rootPtr, "main") == NULL);
    reset_scanner();
    GT
    DEBPRINT("%d\n", prog(false));
    //DEBPRINT("\n\n");
    astNode * rootTree = ASTree.root;
    //printf("\n\nHERE WE GO\n\n");
    //printf("here boy%d \n",rootTree->next->next->nodeRep.defFuncNode.body->next->nodeRep.funcCallNode.paramNum);

    generate_code(rootTree);
}