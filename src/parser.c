/**
 * @file   parser.c
 * 
 * @brief  Implementation of recursive parser for IFJ24
 * 
 *         Parser is based on LL1 grammar created for IFJ24 programming language, which is based on Zig.
 *         Each rule has it's own function and returns false if there was an unexpected token.
 *         
 *         Functionalities: 
 *                          - checking correct order of tokens created from input by scanner
 *                          - building abstract semantic tree described in ast.h for code generator
 *                          - performing semantic checks (such as undefined variable, correctness of main function...)
 * 
 *         When an expression is encountered, control is given to expression parser.
 *         
 * @see    scanner.h ast.h
 * 
 * @author xnovakf00
 * @date   11.11.2024
*/

#include "parser.h"


AST     ASTree; 
astNode currentBlock;
Token currentToken;

bool prog(){
    bool correct = false;
    DEBPRINT("%d\n", currentToken.type);

    ASTree.root = createRootNode(); // initialize ASTree

    // RULE 1 <prog> -> <prolog> <code> <next_code> // EOF MBY
    if(currentToken.type == tokentype_kw_const){
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
    else if(currentToken.type == tokentype_EOF){return true;}
    else{ERROR(ERR_SYNTAX, "Expected: \"const\".\n");}
    DEBPRINT("%d\n", correct);
    return correct;
}

bool prolog(){
    bool correct = false;

    astNode *expr = createAstNode();
    // RULE 2 <prolog> -> const id = @import ( expression ) ;
    if (currentToken.type != tokentype_kw_const) {
        ERROR(ERR_SYNTAX, "Expected: \"const\".\n");
    }
    GT
    if (currentToken.type != tokentype_id) {
        ERROR(ERR_SYNTAX, "Expected: identifier \"ifj\" .\n");
    }
    if (strcmp(currentToken.value, "ifj") != 0) {
        ERROR(ERR_SYNTAX, "Wrong namespace in prologue section.\nExpected: \"ifj\"\nGot: \"%s\"\n",currentToken.value);
    }
    GT
    if (currentToken.type != tokentype_assign) {
        ERROR(ERR_SYNTAX, "Expected: \"=\" .\n");
    }
    GT
    if (currentToken.type != tokentype_import) {
        ERROR(ERR_SYNTAX, "Expected: \"@import\" .\n");
    }
    GT
    if (currentToken.type != tokentype_lbracket) {
        ERROR(ERR_SYNTAX, "Expected: \"(\" .\n");
    }
    GT
    if (true) { // TODO CHECK THE EXPRESSION IN IMPORT
        GT
        correct = false;
    }
    if (currentToken.type != tokentype_rbracket) {
        ERROR(ERR_SYNTAX, "Expected: \")\"  .\n");
    }
    GT
    if (currentToken.type != tokentype_semicolon) {
        ERROR(ERR_SYNTAX, "Expected: \";\" (check line above as well).\n");
    }
    GT

    correct = true;
    DEBPRINT("%d\n", correct);
    return correct;
}


bool code(){
    bool correct = false;
    DEBPRINT("%d\n",currentToken.type);
    // RULE 3 <code> -> <def_func>
    if(currentToken.type == tokentype_kw_pub){
        correct = def_func();
    }
    else{
        ERROR(ERR_SYNTAX, "Expected: \"pub\"\n");
    }
    DEBPRINT("%d %d\n", correct, currentToken.type);
    return correct;
}

bool next_code(){
    bool correct = false;
    DEBPRINT("%d\n", currentToken.type);
    // RULE 4 <next_code> -> <code>
    if(currentToken.type == tokentype_kw_pub){ 
        correct = code();
    }
    // RULE 5 <next_code> -> ε
    else if(currentToken.type == tokentype_EOF){
        correct = mainDefined(); // check if main function is defined and has correct data
        allUsed(funSymtable->rootPtr); // check if all functions defined were also used in program
    }
    else{ERROR(ERR_SYNTAX, "Expected: \"pub\" or EOF\n");}
    DEBPRINT("%d\n", correct);
    return correct;
}

bool def_func(){
    bool correct = false;
    DEBPRINT("%d \n",currentToken.type);

    // preparing information about defined function
    char     *funID;
    funData   entryData;
    symData   entrySymData;
    dataType *paramTypes = malloc(sizeof(dataType)*MAX_PARAM_NUM);
    char    **paramNames = malloc(sizeof(char *)*MAX_PARAM_NUM);
    int       paramNum = 0;
    dataType  returnType;
    bool      nullable;

    astNode  *funcAstNode = createAstNode();  // allocate node with no representation yet
    astNode  *bodyAstRoot = createRootNode(); // create root node for body (statements in body will be connected to this)

    // RULE 6 <def_func> -> pub fn id ( <params> ) <type_func_ret> { <body> }
    if(currentToken.type == tokentype_kw_pub){ 
        GT
        DEBPRINT("%d %s \n",currentToken.type, currentToken.value);
        if(currentToken.type == tokentype_kw_fn){ 
        GT
        DEBPRINT("%d %s\n",currentToken.type, currentToken.value);
        if(currentToken.type == tokentype_id){
        
        funID = currentToken.value;
        // check for redefining already defined function
        symNode *functionEntry = findSymNode(funSymtable->rootPtr, funID);
        if(functionEntry != NULL){
            if(functionEntry->data.data.fData.defined){ERROR(ERR_SEM_REDEF, "Redefining function (%s) is not allowed.\n",funID);}
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
        if(body(returnType, bodyAstRoot)){
        if(currentToken.type == tokentype_rcbracket){
            correct = true;
        }else{ERROR(ERR_SYNTAX, "Expected: \"}\".\n");}
        GT
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"{\".\n");}
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \")\".\n");}
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"(\".\n");}
        }else{ERROR(ERR_SYNTAX, "Expected: id.\n");}
        }else{ERROR(ERR_SYNTAX, "Expected: \"fn\".\n");}
    }else{ERROR(ERR_SYNTAX, "Expected: \"pub\".\n");}


    // information is now known, set it
    symtable *symtableFun   = pop(&symtableStack); // pop from stack so it can be added to symNode
    allUsed(symtableFun->rootPtr); // perform semantic check of used variables


    entryData.tbPtr         = symtableFun;
    entryData.defined       = true;
    entryData.paramNames    = paramNames;
    entryData.paramTypes    = paramTypes;
    entryData.nullableRType = nullable;
    entryData.returnType    = returnType;
    entryData.paramNum      = paramNum;

    entrySymData.data.fData = entryData;
    entrySymData.varOrFun   = 1;

    // to avoid error raised by allUsed() that main was not used
    if(strcmp(funID, "main") == 0){
        entrySymData.used = true;
    }
    else{
        entrySymData.used = false;
    }

    insertSymNode(funSymtable, funID, entrySymData);

    // add correct data to astnode previously created
    createDefFuncNode(funcAstNode, funID, symtableFun, bodyAstRoot, ASTree.root, paramNames, paramNum); 
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
        if(entry != NULL){ERROR(ERR_SEM_REDEF, "Redefining variable (%s) is not allowed.\n",paramID);} // TODO SEMANTIC ERROR 5
        
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
    else if(currentToken.type == tokentype_rbracket){
        correct = true; 
    }else{ERROR(ERR_SYNTAX, "Expected: id or \")\".\n");}
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
    else if(currentToken.type == tokentype_rbracket){
        correct = true;
    }
    else{ERROR(ERR_SYNTAX, "Expected: \",\" or \")\".\n");}
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

    astNode *varAstNode = createAstNode(); // allocate new ast node with no representation yet
    astNode *exprNode   = createAstNode();
    
    // RULE 11 <def_variable> -> <varorconst> id <type_var_def> = expression ;
    if(currentToken.type == tokentype_kw_const || currentToken.type == tokentype_kw_var){ 
        
        if(varorconst(&isConst)){
            DEBPRINT(" %d %s\n", currentToken.type, currentToken.value);
            if(currentToken.type == tokentype_id){

                varName = currentToken.value;
                symNode *varEntry = findInStack(&symtableStack, varName);
                if(varEntry != NULL){ERROR(ERR_SEM_REDEF, "Redefining variable (%s) is not allowed.\n",varName);} // TODO SEMANTIC error 5 redefinition

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
                    
                    if(expression(exprNode)){ // TODO EXPRESSION
                        correct = (currentToken.type == tokentype_semicolon);
                        GT
                    }
                }

                entryData.data.vData = variData;
                insertSymNode(symtableStack.top->tbPtr, varName, entryData);

                varEntry = findInStack(&symtableStack, varName); // get the pointer to entry in symtable

                createDefVarNode(varAstNode ,varName, exprNode, varEntry, block); // create the correct representation
                connectToBlock(varAstNode, block); // connect it to create subtree (root will be the body of block)

                }
                }else{ERROR(ERR_SYNTAX, "Expected: id .\n");}
                }
                }else{ERROR(ERR_SYNTAX, "Expected: \"const\" or \"var\".\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool varorconst(bool *isConst){
    bool correct = false;
    DEBPRINT("   %d\n", currentToken.type);
    // RULE 12 <varorconst> -> const
    if(currentToken.type == tokentype_kw_const){ 
        *isConst = 1;
            correct  = true;
        GT
    }
    // RULE 13 <varorconst> -> var
    else if(currentToken.type == tokentype_kw_var){ 
        *isConst = 0;
        correct = true;
        GT
    }else{ERROR(ERR_SYNTAX, "Expected: \"const\" or \"var\".\n");}
    DEBPRINT(" %d %s\n", correct, currentToken.value);
    return correct;
}

bool unused_decl(astNode *block){
    bool correct = false;

    // prepare empty nodes
    astNode *newUnused = createAstNode();
    astNode *expr      = createAstNode();

    // RULE 14 <unused_decl> -> _ = expression ;
    if(currentToken.type == tokentype_pseudovar){
        GT
        if(currentToken.type == tokentype_assign){
            
            if(expression(expr)){ // TODO EXPRESSION
                if(currentToken.type == tokentype_semicolon){
                    correct = true;
                }else{ERROR(ERR_SYNTAX, "Expected: \";\" (check line above as well).\n");}
                GT
            }
        }else{ERROR(ERR_SYNTAX, "Expected: \"=\" .\n");}
    }else{ERROR(ERR_SYNTAX, "Expected: \"_\".\n");}

    // create node with correct data and connect it into block
    createUnusedNode(newUnused, expr, block);
    connectToBlock(newUnused, block);

    DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_normal(dataType *datatype){
    bool correct = false;
    DEBPRINT("  %d\n", currentToken.type);
    // RULE 15 <type_normal> -> i32
    if(currentToken.type == tokentype_kw_i32){ 
        *datatype = i32;
        correct = true;
        GT
    }
    // RULE 16 <type_normal> -> f64
    else if(currentToken.type == tokentype_kw_f64){
        *datatype = f64;
        correct = true;
        GT
    }
    // RULE 17 <type_normal> -> [ ] u8
    else if(currentToken.type == tokentype_lsbracket){
        GT
        if(currentToken.type == tokentype_rsbracket)
        {
            GT
            if(currentToken.type == tokentype_kw_u8){ 
                correct = true;
                *datatype = u8;
                GT
            }else{ERROR(ERR_SYNTAX, "Expected: \"u8\" .\n");}
        }else{ERROR(ERR_SYNTAX, "Expected: \"]\" .\n");}
    }else{ERROR(ERR_SYNTAX, "Expected: \"i32\" or \"f64\" or \"[\" .\n");}
DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_null(dataType *datatype){
    bool correct = false;
    // RULE 18 <type_null> -> ? <type_normal>
    if(currentToken.type == tokentype_nullid){
        GT
        correct = type_normal(datatype);
    }else{ERROR(ERR_SYNTAX, "Expected: \"?\" .\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool type(bool *nullable, dataType *datatype){
    bool correct = false;
    // RULE 19 <type> -> <type_normal>
    if(currentToken.type == tokentype_kw_f64 || 
       currentToken.type == tokentype_kw_i32 ||
       currentToken.type == tokentype_lsbracket){ 
            correct = type_normal(datatype);
    }
    // RULE 20 <type> -> <type_null>
    else if(currentToken.type == tokentype_nullid){
        *nullable = true;
        correct = type_null(datatype);
    }else{ERROR(ERR_SYNTAX, "Expected: \"f64\" or \"i32\" or \"[\" or \"?\" .\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_func_ret(bool *nullable, dataType *datatype){
    bool correct = false;
    // RULE 21 <type_func_ret> -> <type>
    DEBPRINT(" %d\n", currentToken.type);
    if(currentToken.type  == tokentype_kw_i32    ||
        currentToken.type == tokentype_kw_f64    ||
        currentToken.type == tokentype_lsbracket ||
        currentToken.type == tokentype_nullid){

        correct = type(nullable , datatype);

    }
    // RULE 22 <type_func_ret> -> void
    else if(currentToken.type == tokentype_kw_void){ 
        correct = true;
        *datatype = void_;
        GT
    }else{ERROR(ERR_SYNTAX, "Expected: \"f64\" or \"i32\" or \"[\" or \"?\" or \"void\".\n");}
DEBPRINT(" %d\n", correct);
    return correct;
}

bool type_var_def(bool *nullable, dataType *datatype, bool *inheritedDType){
    bool correct = false;
    // RULE 23 <type_var_def> -> : <type>
    DEBPRINT(" %d\n", currentToken.type);
    if(currentToken.type == tokentype_colon){
        GT
        if(currentToken.type == tokentype_kw_i32
        || currentToken.type == tokentype_kw_f64
        || currentToken.type == tokentype_lsbracket 
        || currentToken.type == tokentype_nullid){ 
            correct = type(nullable, datatype);
        }
    }
    // RULE 24 <type_var_def> -> ε
    else if(currentToken.type == tokentype_assign){
        correct = true;
        *inheritedDType = true;
        
    }else{ERROR(ERR_SYNTAX, "Expected: \"f64\" or \"i32\" or \"[\" or \"?\" or \"=\".\n");}
DEBPRINT(" %d\n", correct);
    return correct;
}

bool st(dataType expReturnType, astNode *block){
    bool correct = false;
    // RULE 34 <st> -> <def_variable>
    if(currentToken.type == tokentype_kw_const ||
       currentToken.type == tokentype_kw_var){

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
    else if(currentToken.type == tokentype_kw_while){ 
        correct = while_statement(expReturnType, block);
    }
    // <st> RULE 38 -> <if_statement>
    else if(currentToken.type == tokentype_kw_if){ 
        correct = if_statement(expReturnType, block);
    }
    // <st> RULE 39 -> <return>
    else if(currentToken.type == tokentype_kw_return){ 
        correct = return_(expReturnType, block);
    }
    else{ERROR(ERR_SYNTAX, "Unexpected start of a statement.\n");}


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
    else if(currentToken.type == tokentype_kw_const  || 
            currentToken.type == tokentype_kw_var    ||
            currentToken.type == tokentype_kw_while  ||
            currentToken.type == tokentype_kw_if     ||
            currentToken.type == tokentype_kw_return ||
            currentToken.type == tokentype_id        ||
            currentToken.type == tokentype_pseudovar){
        
        if(st(returnType, block)){
            correct = body(returnType, block);
        }
    }else if(currentToken.type == tokentype_EOF){ERROR(ERR_SYNTAX, "Unexpected EOF, source code unfinished.\n");}
    else{ERROR(ERR_SYNTAX, "Unexpected token.\n");}

    DEBPRINT("  %d %d\n", correct, currentToken.type);
    return correct;
}

bool return_(dataType expReturnType, astNode *block){
    bool correct = false;

    astNode *exprNode   = createAstNode(); // allocate empty node
    astNode *returnNode = createAstNode(); 

    // RULE 42 <return> -> return <exp_func_ret> ;
    if(currentToken.type == tokentype_kw_return){ 
        GT
        if(exp_func_ret(expReturnType, exprNode)){
            if(currentToken.type == tokentype_semicolon){
                correct = true;
            }else{ERROR(ERR_SYNTAX, "Expected: \";\" (check line above as well).\n");}
            GT
        }
    }else{ERROR(ERR_SYNTAX, "Expected: \"return\".\n");}

    createReturnNode(returnNode, exprNode, expReturnType, block);
    connectToBlock(returnNode, block);
    
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool exp_func_ret(dataType expRetType, astNode *exprNode){
    bool correct = false;
    // RULE 43 <exp_func_ret> -> ε
    DEBPRINT("expected %d\n", expRetType);
    if(currentToken.type == tokentype_semicolon){
        if(expRetType == void_){
            correct = true;
        }
        else{
            ERROR(ERR_SEM_RETURN, "Missing return value in non-void function.\n");
        }
        
    }
    // RULE 44 <exp_func_ret> -> expression
    else{
        dataType returnedDataType;
        correct = expression(exprNode); // TODO EXPRESSION
        if(returnedDataType != expRetType){
            // TODO ERROR UNEXPECTED RETURN TYPE
        }
    }
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool id_without_null(bool *withNull, char **id_wout_null){
    bool correct = false;
    // RULE 45 <id_without_null> -> | id |
    if(currentToken.type == tokentype_vbar){
        GT
        if(currentToken.type == tokentype_id){
            *withNull     = true;
            *id_wout_null = currentToken.value;
            symNode *symEntry = findInStack(&symtableStack, currentToken.value);
            if(symEntry != NULL){ERROR(ERR_SEM_REDEF, "Redefining variable (%s) is not allowed.\n",*id_wout_null);}// TODO ERROR 5 redefinition

            // add the ID_WITHOUT_NULL to symtable for if/while
            varData variData = {.inheritedType = true, .isConst = false, .isNullable = false}; // TODO CHECK THIS
            symData data = {.varOrFun = 0, .used = false, .data.vData = variData};
            insertSymNode(symtableStack.top->tbPtr, currentToken.value, data);

            GT
            if(currentToken.type == tokentype_vbar){
                correct = true;
            }else{ERROR(ERR_SYNTAX, "Expected: \"|\" .\n");}
            GT
        }else{ERROR(ERR_SYNTAX, "Expected: id .\n");}
    }
    // RULE 46 <id_without_null> -> ε
    else if(currentToken.type == tokentype_lcbracket){
        *withNull     = false;
        *id_wout_null = NULL;
        correct       = true;
    }else{ERROR(ERR_SYNTAX, "Expected: \"{\" or \"|\".\n");}
DEBPRINT(" %d\n", correct);
    return correct;
}

bool while_statement(dataType expRetType, astNode *block){
    bool correct = false;

    // prepare empty nodes
    astNode *whileAstNode    = createAstNode(); 
    astNode *condExprAstNode = createAstNode();
    astNode *bodyAstNode     = createAstNode();

    // prepare info needed for correct construction of ast node while
    bool withNull;
    char *id_wout_null;

    // create new symtable for while and push it
    symtable *whileSymTable = createSymtable();
    push(&symtableStack, whileSymTable);

    // RULE 47 <while_statement> -> while ( expression ) <id_without_null> { <body> }
    if(currentToken.type == tokentype_kw_while){ 
        GT
        if(currentToken.type == tokentype_lbracket){
            
            if(expression(condExprAstNode)){ // TODO EXPRESSION
                if(currentToken.type == tokentype_rbracket){
                    GT
                    if(id_without_null(&withNull, &id_wout_null)){
                        if(currentToken.type == tokentype_lcbracket){
                            GT
                            if(body(expRetType, bodyAstNode)){
                                if(currentToken.type == tokentype_rcbracket){
                                    correct = true;
                                }else{ERROR(ERR_SYNTAX, "Expected: \"}\" .\n");}
                                GT
                            }
                        }ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");
                    }
                }else{ERROR(ERR_SYNTAX, "Expected: \")\" .\n");}
            }
        }else{ERROR(ERR_SYNTAX, "Expected: \"(\" .\n");}
    }else{ERROR(ERR_SYNTAX, "Expected: \"while\" .\n");}


    
    allUsed(symtableStack.top->tbPtr->rootPtr); // perform semantic check for used variables in block while
    // create node with correct info and connect it to block
    pop(&symtableStack); // pop, so scopes are not disturbed
    createWhileNode(whileAstNode, withNull, id_wout_null, condExprAstNode, bodyAstNode, whileSymTable, block);
    connectToBlock(whileAstNode, block);

DEBPRINT(" %d\n", correct);
    return correct;
}

bool if_statement(dataType expRetType, astNode *block){
    bool correct = false;
    
    // prepare empty nodes
    astNode *ifElseNode   = createAstNode();
    astNode *ifNode       = createAstNode();
    astNode *elseNode     = createAstNode();
    astNode *bodyIfNode   = createAstNode();
    astNode *bodyElseNode = createAstNode(); 
    astNode *condExrpNode = createAstNode();

    // prepare information
    bool withNull;
    char *id_wout_null;

    // create new scope for if
    symtable *symtableForIf = createSymtable();
    push(&symtableStack, symtableForIf);

    // create new scope for else, don't push it yet, will be pushed after IF is processed
    symtable *symtableForElse = createSymtable();

    // RULE 48 <if_statement> -> if ( expression ) <id_without_null> { <body> } else { <body> } 
    if(currentToken.type == tokentype_kw_if){ 
            GT
        if(currentToken.type == tokentype_lbracket){
            
        if(expression(condExrpNode)){ // TODO EXPRESSION
        if(currentToken.type == tokentype_rbracket){
            GT
        if(id_without_null(&withNull, &id_wout_null)){
        if(currentToken.type == tokentype_lcbracket){
            GT
        if(body(expRetType, ifNode)){
        if(currentToken.type == tokentype_rcbracket){
            GT
        if(currentToken.type == tokentype_kw_else){ 
            allUsed(symtableStack.top->tbPtr->rootPtr); // perform semantic check for used variables in block if
            pop(&symtableStack); // pop the symtable for if so scopes are not disturbed
            push(&symtableStack, symtableForElse); // push the symtable for else 
            GT
        if(currentToken.type == tokentype_lcbracket){
            GT
        if(body(expRetType, elseNode)){
        if(currentToken.type == tokentype_rcbracket){
            correct = true;
        }else{ERROR(ERR_SYNTAX, "Expected: \"}\" .\n");}
            GT
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");};
        }else{ERROR(ERR_SYNTAX, "Expected: \"else\" .\n");};
        }else{ERROR(ERR_SYNTAX, "Expected: \"}\" .\n");}
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");}
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \")\" %d.\n", currentToken.type);}
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"(\" .\n");} 
    }else{ERROR(ERR_SYNTAX, "Expected: \"if\" .\n");}
    allUsed(symtableStack.top->tbPtr->rootPtr); // perform semantic check for used variables in block else
    pop(&symtableStack); // pop the else stack so scopes are not disturbed


    // create nodes with correct information and connect it to block
    createIfNode(ifNode, id_wout_null, symtableForIf, bodyIfNode, block);
    createElseNode(elseNode, symtableForElse, bodyElseNode, block);
    createIfElseNode(ifElseNode, condExrpNode, ifNode, elseNode, withNull, block);

    connectToBlock(ifElseNode, block);

    DEBPRINT("%d\n", correct);
    return correct;
}

bool expr_params(){
    bool correct = false;
    astNode *expr = createAstNode();
    // RULE 26 <expr_params> -> ε
    if(currentToken.type == tokentype_rbracket){
        correct = true;
    }
    // RULE 25 <expr_params> -> expression <expr_params_n>
    else if(expression(expr)){ // TODO EXPRESSION
        correct = expr_params_n();
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
    }else{ERROR(ERR_SYNTAX, "Expected: \",\" or \")\" .\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool after_id(char *id, astNode *block){
    bool correct = false;
    // RULE 29 <after_id> -> = expression ;
    if(currentToken.type == tokentype_assign){
        astNode *newAssNode    = createAstNode();
        astNode *newAssExpNode = createAstNode();
        dataType varDataType   = unknown;
        dataType expressionDT  = unknown;
        symNode *entry         = findInStack(&symtableStack, id);

        if(entry == NULL){ERROR(ERR_SEM_UNDEF, "Assigning to undefined variable \"%s\".\n",id);}
        if(entry->data.data.vData.isConst){ERROR(ERR_SEM_REDEF, "Assigning to unmodifiable (const) variable \"%s\".\n", id);}

        varDataType = entry->data.data.vData.type;

        
        if(expression(newAssExpNode)){// TODO EXPRESSION
            correct = (currentToken.type == tokentype_semicolon);
            GT
        }

        if(expressionDT != varDataType){ERROR(ERR_SEM_TYPE, "Incompatible data types when assigning to \"%s\".\n", id);}

        createAssignNode(newAssNode, id, newAssExpNode, NULL, varDataType);
        connectToBlock(newAssNode, block);
    }
    // RULE 30 <after_id> -> <builtin> ( <expr_params> )  ; 
    else if(currentToken.type == tokentype_dot || currentToken.type == tokentype_lbracket){
        if(builtin(id)){
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
    }else{ERROR(ERR_SYNTAX, "Expected: \"=\" or \".\" or \"(\" .\n");}
DEBPRINT(" %d\n", correct);
    return correct;
}

bool assign_or_f_call(astNode *block){
    bool correct = false;
    // RULE 31 <assign_or_f_call> -> id <after_id>
    if(currentToken.type == tokentype_id){
        char *id = currentToken.value;
        GT
        correct = after_id(id, block);
    }else{ERROR(ERR_SYNTAX, "Expected: id .\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool builtin(char *id){
    bool correct = false;
    // RULE 32 <builtin> -> . id
    if(currentToken.type == tokentype_dot){
        if(strcmp(id, "ifj") != 0){
            ERROR(ERR_SYNTAX, "Incorrect namespace for builtin functions. Expected: \"ifj\" Got: \"%s\" .\n", id);
        }
        GT
        if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if correct builtin name
            correct = true;
            GT
        }else{ERROR(ERR_SYNTAX, "Expected: builtin id .\n");}
    }
    // RULE 33 <builtin> -> ε
    else if(currentToken.type == tokentype_lbracket){
        correct = true;
    }else{ERROR(ERR_SYNTAX, "Expected: \"(\" or \".\".\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
}

int main(){
    initStack(&symtableStack);
    funSymtable = createSymtable();
    input_file = fopen("file.txt", "r");
    GT
    //prog();
    DEBPRINT("%d\n", prog());
    DEBPRINT("\n\n");

    astNode * rootTree = ASTree.root;

}



/* HELPER SEMANTIC FUNCTIONS */

/**
 * @brief  Function validates that a function main was defined correctly.
 * 
 *         funSymtable is searched by findSymNode for a fuction with id "main".
 *         When no such entry is found, function triggers error. Similarly,
 *         when the return type is not void or there are defined parameters of a
 *         function.
 * 
 * @see findSymNode
 * 
 * @return True if all correct, false if an error occurs.
 */
bool mainDefined(){

    symNode *found = findSymNode(funSymtable->rootPtr, "main");
    if(found == NULL){
        return false;
        ERROR(ERR_SEM_UNDEF, "Definition for function \"main\" was not found.\n");
    }

    funData data = found->data.data.fData;

    if(data.returnType != void_){
        ERROR(ERR_SEM_FUN, "Function \"main\" must have return type \"void\".\n");
        return false;
    }

    if(data.paramNum != 0){
        ERROR(ERR_SEM_FUN, "Function \"main\" must not have any parameters.\n");
        return false;
    }

    return true;
}

/**
 * @brief        Validates that all variables that were defined are also used in scope.
 * 
 *               Function uses resursive preorder traversal to go through the BST.
 *               When a entry of a variable which used flag is false is found, function
 *               triggers error.
 * 
 * @see          symData
 * 
 * @param root   Root of a (sub)tree to traverse through.
 * 
 */
void allUsed(symNode *root){

    if(root != NULL){
        if(!root->data.used){
            ERROR(ERR_SEM_UNUSED, "Variable \"%s\" defined but not used within block.\n", root->key);
        }
        allUsed(root->l);
        allUsed(root->r);
    }

}


/**
 * @brief           Function checks whether variable was defined in scope or before current scope.
 * 
 * @param ID        Name of the function or variable.
 * 
 * @return          True if was defined, false if not.
 */
bool wasDefined(char *ID, symNode **node){

    symNode *entry = findInStack(&symtableStack, ID);

    if(entry != NULL){
        entry->data.used = true; // is used, set the flag for semantic check later in allUsed()
        *node = entry;
        return true;
    }  
    else{
        *node = NULL;
        return false;
    }
}


/**
 * @brief    Returns return type of needed function.
 * 
 * @param ID Id of the function.
 * 
 * @warning  Must be called on a function you are sure exists in funSymtable.
 * 
 * @return   dataType
 */
dataType getReturnType(char *ID){
    symNode *entry = findSymNode(funSymtable->rootPtr, ID);

    return entry->data.data.fData.returnType;
}

/**
 * @brief    Returns return type of needed variable.
 * 
 * @param ID Id of the variable.
 * 
 * @warning  Must be called on a function you are sure exists in symtable in current stack.
 * 
 * @return   dataType
 */
dataType getVarType(char *ID){
    symNode *entry = findInStack(&symtableStack, ID);

    return entry->data.data.vData.type;
}

/* EOF parser.c */