/**
 * @file   parser.c
 * 
 * @brief  Implementation of recursive parser for IFJ24
 * 
 *         Parser is based on LL1 grammar created for IFJ24 programming language, which is based on Zig.
 *         Each rule has it's own function and returns false if there was an unexpected token.
 * 
 * @see    scanner.h parser.h
 * 
 * @author xnovakf00
 * @date   06.11.2024
*/


#include "parser.h"

#define GT currentToken = getToken(); // encapsulating the assignment

#define DEBUG
#ifdef DEBUG
    #define DEBPRINT(...) \
        fprintf(stderr, "D: %s, %d: ", __func__ , __LINE__); \
        fprintf(stderr, __VA_ARGS__);
#else
    #define DEBPRINT(...) 
#endif

Token   currentToken;
AST     ASTree; 
astNode currentBlock;

bool prog(){
    bool correct = false;
    DEBPRINT("%d\n", currentToken.type);

    ASTree.root = createRootNode(); // initialize ASTree

    // RULE 1 <prog> -> <prolog> <code> <next_code> // EOF MBY
    if(currentToken.type == tokentype_keyword){ // TODO check if const
        if(prolog()){
            if(code()){
                DEBPRINT("%d\n", currentToken.type);
                if(next_code()){
                    correct = true;
                }
            }
        }
        // TODO Check eof idk how rn
    }
    DEBPRINT("%d\n", correct);
    return correct;
}

bool prolog(){
    bool correct = false;
    // RULE 2 <prolog> -> const id = @import ( expression ) ; 
    if(currentToken.type == tokentype_keyword){ // TODO check if const
        GT
        if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if id==ifj
            GT
            if(currentToken.type == tokentype_assign){
                GT
                if(currentToken.type == tokentype_import){
                    GT
                    if(currentToken.type == tokentype_lbracket){
                        GT
                        if(expression()){ // EXPRESSION TODO
                            if(currentToken.type == tokentype_rbracket){
                                GT
                                correct = (currentToken.type == tokentype_semicolon);
                                GT
                            }
                        }
                    }
                }
            }
        }
    }
    DEBPRINT("%d\n", correct);
    return correct;
}

bool code(){
    bool correct = false;
    DEBPRINT("%d\n",currentToken.type);
    // RULE 3 <code> -> <def_func>
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        correct = def_func();
    }
    DEBPRINT("%d %d\n", correct, currentToken.type);
    return correct;
}

bool next_code(){
    bool correct = false;
    DEBPRINT("%d\n", currentToken.type);
    // RULE 4 <next_code> -> <code>
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        correct = code();
    }
    // RULE 5 <next_code> -> ε
    else if(currentToken.type == tokentype_EOF){
        correct = true;
    }
    DEBPRINT("%d\n", correct);
    return correct;
}

bool def_func(){
    bool correct = false;
    DEBPRINT("%d \n",currentToken.type);

    // preparing information about defined function
    char    *funID;
    funData  entryData;
    symData  entrySymData;
    dataType *paramTypes = malloc(sizeof(dataType)*MAX_PARAM_NUM);
    char    **paramNames = malloc(sizeof(char *)*MAX_PARAM_NUM);
    int      paramNum = 0;
    dataType returnType;
    bool     nullable;
    astNode *bodyNode;
    astNode *funcAstNode = createAstNode();

    // RULE 6 <def_func> -> pub fn id ( <params> ) <type_func_ret> { <body> }
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        GT
        DEBPRINT("%d %s \n",currentToken.type, currentToken.value);
        if(currentToken.type == tokentype_keyword){ // TODO check if fn
        GT
        DEBPRINT("%d %s\n",currentToken.type, currentToken.value);
        if(currentToken.type == tokentype_id){
        
        funID = currentToken.value;
        // check for redefining already defined function
        symNode *functionEntry = findSymNode(funSymtable->rootPtr, funID);
        if(functionEntry != NULL){
            if(functionEntry->data.data.fData.defined){}
        } // TODO SEMANTIC error 5 redefinition and recheck of parameters and return type
        

        // create new scope and push it
        symtable *symtableNewF = createSymtable();
        push(&symtableStack, symtableNewF);

        GT
        if(currentToken.type == tokentype_lbracket){
        GT
        if(params(&paramNum, &paramTypes, &paramNames)){
        if(currentToken.type == tokentype_rbracket){
        GT
        if(type_func_ret(&nullable, &returnType)){
        if(currentToken.type == tokentype_lcbracket){
        GT
        if(body(returnType, funcAstNode)){
        correct = (currentToken.type == tokentype_rcbracket);
        GT
        }}}}}}}}
    }

    symtable *symtableFun = pop(&symtableStack); 
    entryData.defined       = true;
    entryData.paramNames    = paramNames;
    entryData.paramTypes    = paramTypes;
    entryData.tbPtr         = symtableFun;
    entryData.nullableRType = nullable;
    entryData.returnType    = returnType;

    entrySymData.data.fData = entryData;
    insertSymNode(funSymtable, funID, entrySymData);
   

    createDefFuncNode(funcAstNode ,funID, symtableFun, bodyNode, ASTree.root);
    
    connectToBlock(funcAstNode, ASTree.root);

    DEBPRINT("%d\n", correct);
    return correct;
}

bool params(int *paramNum, dataType **paramTypes, char ***paramNames){
    bool correct = false;

    // preparing information about parameter
    char    *paramID;
    dataType paramType;
    bool     nullable;
    varData  entryVarData = {.inheritedType = 0, .isConst = 1}; // parameter has to be const and can't have inherited type
    symData  entryData    = {.varOrFun = 0};

    // RULE 7 <params> -> id : <type> <params_n>
    if(currentToken.type == tokentype_id){
        DEBPRINT("%s  %d\n", currentToken.value, currentToken.type);

        // check if redefining existing (two params with same name)
        paramID = currentToken.value;
        symNode *entry = findInStack(&symtableStack, paramID);
        if(entry != NULL){} // TODO SEMANTIC ERROR 5
        
        GT
        if(currentToken.type == tokentype_colon){
            GT

            if(type(&nullable, &paramType)){
          
                // all information are known, set them accordingly
                (*paramNames)[*paramNum]  = paramID;
                (*paramTypes)[*paramNum]  = paramType;
                entryVarData.type       = paramType;
                entryVarData.isNullable = nullable;
                (*paramNum)++;
                entryData.data.vData    = entryVarData,
                insertSymNode(symtableStack.top->tbPtr, paramID, entryData);

                correct = params_n(paramNum, paramTypes, paramNames);
            }
        }
    }

    // RULE 8 <params> -> ε
    else{
        correct = (currentToken.type == tokentype_rbracket);   
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

// TODO CHECK THIS WHOLE FUNCTION
bool params_n(int *paramNum, dataType **paramTypes, char ***paramNames){
    bool correct = false;
    // RULE 9 <params_n> -> , <params>
    if(currentToken.type == tokentype_comma){
        GT
        correct = params(paramNum, paramTypes, paramNames);
    }
    // RULE 10 <params_n> -> ε
    else{
        correct = (currentToken.type == tokentype_rbracket);
    }
    DEBPRINT("%d\n", correct);
    return correct;
}

bool def_variable(astNode *block){
    bool correct = false;

    // prepare information about defined variable
    char    *varName;
    dataType varType;
    bool     nullable;
    bool     isConst;
    bool     inheritedType;

    varData  variData;
    symData  entryData;
    astNode *initExpr;

    astNode *varAstNode = createAstNode();
    
    // RULE 11 <def_variable> -> <varorconst> id <type_var_def> = expression ;
    if(currentToken.type == tokentype_keyword){ // TODO check if const or var
        
        if(varorconst(&isConst)){
            DEBPRINT(" %d %s\n", currentToken.type, currentToken.value);
            if(currentToken.type == tokentype_id){

                varName = currentToken.value;
                symNode *varEntry = findInStack(&symtableStack, varName);
                if(varEntry != NULL){} // TODO SEMANTIC error 5 redefinition

                variData.isConst = isConst;
                entryData.used = false;
                entryData.varOrFun = 0;
        
                GT

                if(type_var_def(&nullable, &varType, &inheritedType)){
                    DEBPRINT("%d \n", currentToken.type); 

                    variData.inheritedType = inheritedType;
                    variData.isNullable    = nullable;
                    variData.type          = varType;

                if(currentToken.type == tokentype_assign){
                    GT
                    if(expression()){ // TODO EXPRESSION
                        correct = (currentToken.type == tokentype_semicolon);
                        GT
                    }
                }

                entryData.data.vData = variData;
                insertSymNode(symtableStack.top->tbPtr, varName, entryData);
                varEntry = findInStack(&symtableStack, varName);
                astNode *node;
                createDefVarNode(varAstNode ,varName, initExpr, varEntry, node);
                connectToBlock(varAstNode, block);

                }}}}
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool varorconst(bool *isConst){
    bool correct = false;
    DEBPRINT("   %d\n", currentToken.type);
    if(currentToken.type == tokentype_keyword){
        // RULE 12 <varorconst> -> const
       if(true){ // TODO check if keyword == const
            *isConst = 1;
             correct  = true;
            GT
        }
        // RULE 13 <varorconst> -> var
        else{ // TODO check if keyword == var
            *isConst = 0;
            correct = true;
            GT
        }
    }
    DEBPRINT(" %d %s\n", correct, currentToken.value);
    return correct;
}

bool unused_decl(){
    bool correct = false;
    // RULE 14 <unused_decl> -> _ = expression ;
    if(currentToken.type == tokentype_pseudovar){
        GT
        if(currentToken.type == tokentype_assign){
            GT
            if(expression()){ // TODO EXPRESSION
                correct = (currentToken.type == tokentype_semicolon);
                GT
            }
        }
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_normal(dataType *datatype){
    bool correct = false;
    // RULE 15 <type_normal> -> i32
    // RULE 16 <type_normal> -> f64
    DEBPRINT("  %d\n", currentToken.type);
    if(currentToken.type == tokentype_keyword){ // TODO check if keyword == i32 or f64
        // *datatype = typ TODO
        correct = true;
        GT
    }
    // RULE 17 <type_normal> -> [ ] u8
    else if(currentToken.type == tokentype_lsbracket){
        GT
        if(currentToken.type == tokentype_rsbracket)
        {
            GT
            if(currentToken.type == tokentype_keyword){ // TODO check if keyword == u8
                correct = true;
                *datatype = u8;
                GT
            }
        }
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_null(dataType *datatype){
    bool correct = false;
    // RULE 18 <type_null> -> ? <type_normal>
    if(currentToken.type == tokentype_nullid){
        GT
        correct = type_normal(datatype);
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool type(bool *nullable, dataType *datatype){
    bool correct = false;
    // RULE 19 <type> -> <type_normal>
    if(currentToken.type == tokentype_keyword || 
       currentToken.type == tokentype_lsbracket){ // TODO check if keyword == i32 or f64
            correct = type_normal(datatype);
    }
    // RULE 20 <type> -> <type_null>
    else if(currentToken.type == tokentype_nullid){
        *nullable = true;
        correct = type_null(datatype);
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_func_ret(bool *nullable, dataType *datatype){
    bool correct = false;
    // RULE 21 <type_func_ret> -> <type>
    DEBPRINT(" %d\n", currentToken.type);
    if(currentToken.type == tokentype_keyword){ // TODO add check i32 f64 [ ?
        correct = type(nullable , datatype);
    }
    // RULE 22 <type_func_ret> -> void
    else if(currentToken.type == tokentype_keyword){ // TODO add check void
        correct = true;
        GT
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_var_def(bool *nullable, dataType *datatype, bool *inheritedDType){
    bool correct = false;
    // RULE 23 <type_var_def> -> : <type>
    DEBPRINT(" %d\n", currentToken.type);
    if(currentToken.type == tokentype_colon){
        GT
        if(currentToken.type == tokentype_keyword 
        || currentToken.type == tokentype_lsbracket 
        || currentToken.type == tokentype_nullid){ // TODO add check i32 f64
            correct = type(nullable, datatype);
        }
    }
    // RULE 24 <type_var_def> -> ε
    else{
        correct = (currentToken.type == tokentype_assign);
        *inheritedDType = true;
        
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool st(dataType expReturnType, astNode *block){
    bool correct = false;
    // RULE 34 <st> -> <def_variable>
    if(currentToken.type == tokentype_keyword){ // TODO check if const/var
        correct = def_variable(block);
    }
    // RULE 35 <st> -> <assign_or_f_call>
    else if(currentToken.type == tokentype_id){
        correct = assign_or_f_call(block);
    }
    // RULE 36 <st> -> <unused_decl>
    else if(currentToken.type == tokentype_pseudovar){
        correct = unused_decl(block);
    }
    // <st> RULE 37 -> <while_statement>
    else if(currentToken.type == tokentype_keyword){ // TODO check if while
        correct = while_statement(expReturnType, block);
    }
    // <st> RULE 38 -> <if_statement>
    else if(currentToken.type == tokentype_keyword){ // TODO check if if
        correct = if_statement(expReturnType, block);
    }
    // <st> RULE 39 -> <return>
    else if(currentToken.type == tokentype_keyword){ // TODO check if return
        correct = return_(expReturnType, block);
    }


    return correct;
}

bool body(dataType returnType, astNode *block){
    bool correct = false;
    // RULE 40 <body> -> ε
    DEBPRINT("   %d \n", currentToken.type);
    if(currentToken.type == tokentype_rcbracket){
        correct = true;
    }
    // RULE 41 <body> -> <st> <body>
    else if(currentToken.type == tokentype_keyword || // TODO check keywords
            currentToken.type == tokentype_id ||
            currentToken.type == tokentype_pseudovar){
        
        if(st(returnType, block)){
            correct = body(returnType, block);
        }
    }

    DEBPRINT("  %d %d\n", correct, currentToken.type);
    return correct;
}

bool return_(dataType expReturnType, astNode *block){
    bool correct = false;
    // RULE 42 <return> -> return <exp_func_ret> ;
    if(currentToken.type == tokentype_keyword){ // TODO check if keyword == return
        GT
        if(exp_func_ret(expReturnType)){
            correct = (currentToken.type == tokentype_semicolon);
            GT
        }
    } 
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool exp_func_ret(dataType expRetType){
    bool correct = false;
    // RULE 43 <exp_func_ret> -> ε
    if(currentToken.type == tokentype_semicolon){
        if(expRetType == void_){
            correct = true;
        }
        else{
            // TODO ERROR UNEXPECTED RETURN TYPE
        }
    }
    // RULE 44 <exp_func_ret> -> expression
    else{
        dataType returnedDataType;
        correct = expression(); // TODO EXPRESSION
        if(returnedDataType != expRetType){
            // TODO ERROR UNEXPECTED RETURN TYPE
        }
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool id_without_null(){
    bool correct = false;
    // RULE 45 <id_without_null> -> | id |
    if(currentToken.type == tokentype_vbar){
        GT
        if(currentToken.type == tokentype_id){ // TODO SEMANTIC and add to symtable of while/if
            GT
            correct = (currentToken.type == tokentype_vbar);
            GT
        }
    }
    // RULE 46 <id_without_null> -> ε
    else if(currentToken.type == tokentype_lcbracket){
        correct = true;
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool while_statement(dataType expRetType, astNode *block){
    bool correct = false;
    astNode *whileAstNode = createAstNode(); 

    // RULE 47 <while_statement> -> while ( expression ) <id_without_null> { <body> }
    if(currentToken.type == tokentype_keyword){ // TODO check if == while
        GT
        if(currentToken.type == tokentype_lcbracket){
            GT
            if(expression()){ // TODO EXPRESSION
                if(currentToken.type == tokentype_rcbracket){
                    GT
                    if(id_without_null()){
                        if(currentToken.type == tokentype_lcbracket){
                            GT
                            if(body(expRetType, whileAstNode)){
                                correct = (currentToken.type == tokentype_rcbracket);
                                GT
                            }
                        }
                    }
                }
            }
        }
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool if_statement(dataType expRetType, astNode *block){
    bool correct = false;
    astNode *ifNode   = createAstNode();
    astNode *elseNode = createAstNode();

    // create new scope for if
    symtable *symtableForIf = createSymtable();
    push(&symtableStack, symtableForIf);

    // RULE 48 <if_statement> -> if ( expression ) <id_without_null> { <body> } else { <body> } 
    if(currentToken.type == tokentype_keyword){ // TODO check if if
            GT
        if(currentToken.type == tokentype_lbracket){
            GT
        if(expression()){ // TODO EXPRESSION
        if(currentToken.type == tokentype_rbracket){
            GT
        if(id_without_null()){
        if(currentToken.type == tokentype_lcbracket){
            GT
        if(body(expRetType, ifNode)){
        if(currentToken.type == tokentype_rcbracket){
            GT
        if(currentToken.type == tokentype_keyword){ // TODO check if else
            
            pop(&symtableStack); // TODO pop the symtable into while node 
            
            // create scope for else
            symtable *symtableForElse = createSymtable();
            push(&symtableStack, symtableForElse);
            
            GT
        if(currentToken.type == tokentype_lcbracket){
            GT
        if(body(expRetType, elseNode)){
        correct = (currentToken.type == tokentype_rcbracket);
            GT
        }}}}}}}}}}
        pop(&symtableStack); // TODO pop the symtable into else node 
    }
    DEBPRINT("%d\n", correct);
    return correct;
}

bool expr_params(){
    bool correct = false;
    // RULE 25 <expr_params> -> expression <expr_params_n>
    if(expression()){ // TODO EXPRESSION
        correct = expr_params_n();
    }
    // RULE 26 <expr_params> -> ε
    if(currentToken.type == tokentype_rbracket){
        correct = true;
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool expr_params_n(){
    bool correct = false;
    // RULE 27 <expr_params_n> -> , <expr_params_n>
    if(currentToken.type == tokentype_comma){
        GT
        correct = expr_params_n();
    }
    // RULE 28 <expr_params_n> -> ε 
    else if(currentToken.type == tokentype_rbracket){
        correct = true;
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool after_id(){
    bool correct = false;
    // RULE 29 <after_id> -> = expression ;
    if(currentToken.type == tokentype_assign){
        GT
        if(expression()){// TODO EXPRESSION
            correct = (currentToken.type == tokentype_semicolon);
            GT
        }
    }
    // RULE 30 <after_id> -> <builtin> ( <expr_params> )  ;
    else if(currentToken.type == tokentype_dot){
        if(builtin()){
            if(currentToken.type == tokentype_lbracket){
                GT
                if(expr_params()){
                    if(currentToken.type == tokentype_rbracket){
                        GT
                        correct = (currentToken.type == tokentype_semicolon);
                        GT
                    }
                }
            }
        }
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool assign_or_f_call(){
    bool correct = false;
    // RULE 31 <assign_or_f_call> -> id <after_id>
    if(currentToken.type == tokentype_id){
        GT
        correct = after_id();
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool builtin(){
    bool correct = false;
    // RULE 32 <builtin> -> . id
    if(currentToken.type == tokentype_dot){
        GT
        if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if correct builtin name
            correct = true;
            GT
        }
    }
    // RULE 33 <builtin> -> ε
    else{
        correct = (currentToken.type == tokentype_lbracket);
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

// this function is placeholder for expression parsing
bool expression(){
    GT
    return true;
}

int main(){
    initStack(&symtableStack);
    funSymtable = createSymtable();
    input_file = fopen("file.txt", "r");
    GT
    DEBPRINT("%d\n", prog());
    astNode *node = ASTree.root->next;
    DEBPRINT("this is the type %d, %s, %d\n", node->type, node->nodeRep.defVarNode.id, node->nodeRep.defVarNode.symtableEntry->data.used);
}

/* EOF parser.c */