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


bool prog(bool firstTraverse){
    GT
    bool correct = false;
    DEBPRINT("%d\n", currentToken.type);

    ASTree.root = createRootNode(); // initialize ASTree

    // RULE 1 <prog> -> <prolog> <code> <next_code> // EOF MBY
    if(currentToken.type == tokentype_kw_const){
        if(prolog()){
            code(firstTraverse);
                DEBPRINT("%d HIER HOER HIER\n", currentToken.type);
                if(next_code(firstTraverse)){
                    correct = true;
                }
        }
    }
    else if(currentToken.type == tokentype_EOF){
        return true;
    }
    else{ERROR(ERR_SYNTAX, "Expected: \"const\".\n");}
    correct = mainDefined(); // check if main function is defined and has correct data
    if(!firstTraverse){
        allUsed(funSymtable->rootPtr); // check if all functions defined were also used in program
    }
    
    return correct;
}

bool prolog(){
    bool correct = false;
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
    if (currentToken.type == tokentype_string && strcmp(currentToken.value, "ifj24.zig") == 0) { 
        GT
        correct = false;
    }else{ERROR(ERR_SYNTAX, "Incorrect expression in prologue. Expected: \"ifj24.zig\".\n");}
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


bool code(bool firstTraverse){
    bool correct = false;
    DEBPRINT("%d\n",currentToken.type);
    // RULE 3 <code> -> <def_func>
    if(currentToken.type == tokentype_kw_pub){
        correct = def_func(firstTraverse);
    }
    else{
        ERROR(ERR_SYNTAX, "Expected: \"pub\"\n");
    }
    DEBPRINT("%d %d\n", correct, currentToken.type);
    return correct;
}

bool next_code(bool firstTraverse){
    bool correct = false;
    DEBPRINT("%d\n", currentToken.type);
    // RULE 4 <next_code> -> <code>
    if(currentToken.type == tokentype_kw_pub){ 
        correct = code(firstTraverse);
    }
    // RULE 5 <next_code> -> ε
    else if(currentToken.type == tokentype_EOF){
        correct = true;
    }
    else{ERROR(ERR_SYNTAX, "Expected: \"pub\" or EOF\n");}
    DEBPRINT("%d\n", correct);
    return correct;
}

bool def_func(bool firstTraverse){
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
    astNode  *bodyAstRoot = createAstNode(); // create root node for body (statements in body will be connected to this)
    bodyAstRoot->type = AST_NODE_ROOT;

    symNode *functionEntry = NULL;

    // RULE 6 <def_func> -> pub fn id ( <params> ) <type_func_ret> { <body> }
    if(currentToken.type != tokentype_kw_pub){
        ERROR(ERR_SYNTAX, "Expected: \"pub\".\n");
    }

    GT
    DEBPRINT("%d %s \n",currentToken.type, currentToken.value);

    if(currentToken.type != tokentype_kw_fn){
        ERROR(ERR_SYNTAX, "Expected: \"fn\".\n");
    }
    
    GT
    DEBPRINT("%d %s\n",currentToken.type, currentToken.value);

    if(currentToken.type != tokentype_id){
        ERROR(ERR_SYNTAX, "Expected: id.\n");
    }
        
    funID = currentToken.value;
    // check for redefining already defined function when first traverse
    functionEntry = findSymNode(funSymtable->rootPtr, funID);
    if(functionEntry != NULL && firstTraverse){
        ERROR(ERR_SEM_REDEF, "Redefining function (%s) is not allowed.\n",funID);
    }
        

    // create new scope and push it
    symtable *symtableNewF = createSymtable();
    push(&symtableStack, symtableNewF);

    GT
    
    if(currentToken.type != tokentype_lbracket){
        ERROR(ERR_SYNTAX, "Expected: \"(\".\n");
    }
    
    GT
        
    params(&paramNum, &paramTypes, &paramNames);

    if(currentToken.type != tokentype_rbracket){
        ERROR(ERR_SYNTAX, "Expected: \")\".\n");
    }

    GT
        
    type_func_ret(&nullable, &returnType);

    if(currentToken.type != tokentype_lcbracket){
        ERROR(ERR_SYNTAX, "Expected: \"{\".\n");
    }

    GT
    
    if(firstTraverse){
        while(currentToken.type != tokentype_kw_pub && currentToken.type != tokentype_EOF){
            GT
        }
    }
    else{
        body(returnType, bodyAstRoot);
        if(currentToken.type != tokentype_rcbracket){
            ERROR(ERR_SYNTAX, "aExpected: %d\"}\".\n",currentToken.type);
        }

        GT
    }

    // information is now known, set it
    symtable *symtableFun   = pop(&symtableStack); // pop from stack so it can be added to symNode
    if(!firstTraverse){
        allUsed(symtableFun->rootPtr); // perform semantic check of used variables
    }

    if(firstTraverse){

        entryData.tbPtr         = symtableFun;
        entryData.defined       = true;
        entryData.paramNames    = paramNames;
        entryData.paramTypes    = paramTypes;
        entryData.nullableRType = nullable;
        entryData.returnType    = returnType;
        entryData.paramNum      = paramNum;

        entrySymData.data.fData = entryData;
        entrySymData.varOrFun   = 1;

        if(strcmp(funID, "main") == 0){
            entrySymData.used = true;
        }
        else{
            entrySymData.used = false;
        }
        
        insertSymNode(funSymtable, funID, entrySymData);
    }



    // add correct data to astnode previously created
    if(!firstTraverse){
        createDefFuncNode(funcAstNode, funID, symtableFun, bodyAstRoot, ASTree.root, paramNames, paramNum); 
        connectToBlock(funcAstNode, ASTree.root);
    }

    //symNode *test = findSymNode(funSymtable->rootPtr, funID);
    //DEBPRINT(" %s WAS USED %d\n",funID, test->data.used);
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

        if(currentToken.type != tokentype_colon){
            ERROR(ERR_SYNTAX, "Expected: \":\"\n");
        }

        GT

        type(&nullable, &paramType) ;       
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

    // RULE 8 <params> -> ε
    else if(currentToken.type == tokentype_rbracket){
        return true;
    }
    else{ERROR(ERR_SYNTAX, "Expected: id or \")\".\n");}
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
    else{
        ERROR(ERR_SYNTAX, "Expected: \",\" or \")\".\n");
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

    astNode *varAstNode = createAstNode(); // allocate new ast node with no representation yet
    astNode *exprNode   = createAstNode();
    
    // RULE 11 <def_variable> -> <varorconst> id <type_var_def> = expression ;
    if(currentToken.type != tokentype_kw_const && currentToken.type != tokentype_kw_var){
        ERROR(ERR_SYNTAX, "Expected: \"const\" or \"var\".\n");
    } 
        
    varorconst(&isConst);
    DEBPRINT(" %d %s\n", currentToken.type, currentToken.value);
    if(currentToken.type != tokentype_id){
        ERROR(ERR_SYNTAX, "Expected id.\n");
    }

        varName = currentToken.value;
        symNode *varEntry = findInStack(&symtableStack, varName);
        if(varEntry != NULL){ERROR(ERR_SEM_REDEF, "Redefining variable (%s) is not allowed.\n",varName);} // TODO SEMANTIC error 5 redefinition

        variData.isConst = isConst;
        entryData.used = false;
        entryData.changed = false;
        entryData.varOrFun = 0;

        GT

        type_var_def(&nullable, &varType, &inheritedType);

        DEBPRINT("%d \n", currentToken.type); 

        variData.inheritedType = inheritedType;
        variData.isNullable    = nullable;
        variData.type          = varType;

        if(currentToken.type != tokentype_assign){
            ERROR(ERR_SYNTAX, "Expected: \"=\".\n");
        }
        
        GT

        expression(exprNode); // TODO EXPRESSION

        if(currentToken.type != tokentype_semicolon){
            ERROR(ERR_SYNTAX, "Expected \";\".\n");
        }

        GT
        
        if(variData.inheritedType == true){
            variData.type = exprNode->nodeRep.exprNode.dataT;
            variData.inheritedType = false;
        }
        else{
            if(variData.type != exprNode->nodeRep.exprNode.dataT){
                ERROR(ERR_SEM_TYPE, "Incompatible data types when assigning to \"%s\". %d\n", varName , exprNode->nodeRep.exprNode.dataT);
            }
        }

        entryData.data.vData = variData;
        insertSymNode(symtableStack.top->tbPtr, varName, entryData);

        varEntry = findInStack(&symtableStack, varName); // get the pointer to entry in symtable

        createDefVarNode(varAstNode ,varName, exprNode, varEntry, block); // create the correct representation
        connectToBlock(varAstNode, block); // connect it to create subtree (root will be the body of block)

    DEBPRINT(" %d\n", correct);
    return true;
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
    }
    else{
        ERROR(ERR_SYNTAX, "Expected: \"const\" or \"var\".\n");
    }
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
            GT
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
        *nullable = false;
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
        *inheritedDType = false;
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
        DEBPRINT("%s\n", currentToken.value);
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
        DEBPRINT("%s\n", currentToken.value);
        st(returnType, block);
        correct = body(returnType, block);
        
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
        if(exp_func_ret(expReturnType, &exprNode)){
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

bool exp_func_ret(dataType expRetType, astNode **exprNode){
    bool correct = false;
    // RULE 43 <exp_func_ret> -> ε
    DEBPRINT("expected %d\n", expRetType);
    if(currentToken.type == tokentype_semicolon){
        if(expRetType == void_){
            correct = true;
            *exprNode = NULL;
        }
        else{
            ERROR(ERR_SEM_RETURN, "Missing return value in non-void function.\n");
        }
        
    }
    // RULE 44 <exp_func_ret> -> expression
    else{
        correct = expression(*exprNode); // TODO EXPRESSION
        dataType exprType = (*exprNode)->nodeRep.exprNode.dataT;
        if(exprType != expRetType){
            ERROR(ERR_SEM_FUN, "Returning expression data type does not match function return type.\n");
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
            GT
            if(expression(condExprAstNode)){ // TODO EXPRESSION
                if(currentToken.type == tokentype_rbracket){
                    GT
                    if(id_without_null(&withNull, &id_wout_null)){

                        // TODO check correct expression in condExprNode

                        if(currentToken.type == tokentype_lcbracket){
                            GT
                            if(body(expRetType, bodyAstNode)){
                                if(currentToken.type == tokentype_rcbracket){
                                    correct = true;
                                }else{ERROR(ERR_SYNTAX, "Expected: \"}\" at the end of \"while\" .\n");}
                                GT
                            }
                        }else{
                            ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");
                        }
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
            GT
        if(expression(condExrpNode)){ // TODO EXPRESSION

        if(currentToken.type == tokentype_rbracket){
            GT
        if(id_without_null(&withNull, &id_wout_null)){

            // TODO check correct expression in condExprNode

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
        }else{ERROR(ERR_SYNTAX, "cExpected: \"}\" .\n");}
            GT
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");};
        }else{ERROR(ERR_SYNTAX, "Expected: \"else\" .\n");};
        }else{ERROR(ERR_SYNTAX, "dExpected: \"}\" .\n");}
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

bool expr_params(astNode **params, int *paramCnt){
    bool correct = false;
    astNode *expr = createAstNode();
    // RULE 26 <expr_params> -> ε
    if(currentToken.type == tokentype_rbracket){
        correct = true;
    }
    // RULE 25 <expr_params> -> expression <expr_params_n>
    else if(expression(expr)){ // TODO EXPRESSION

        params[*paramCnt] = expr;
        (*paramCnt)++;

        correct = expr_params_n(params, paramCnt);
    }
DEBPRINT(" %d\n", correct);
    return correct;
}

bool expr_params_n(astNode **params, int *paramCnt){
    bool correct = false;
    // RULE 27 <expr_params_n> -> , <expr_params> // TODO CHECK THIS
    if(currentToken.type == tokentype_comma){
        GT
        correct = expr_params(params, paramCnt);
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

        GT
        if(expression(newAssExpNode)){// TODO EXPRESSION
            correct = (currentToken.type == tokentype_semicolon);
            GT
        }
        expressionDT = newAssExpNode->nodeRep.exprNode.dataT;
        if(expressionDT != varDataType){ERROR(ERR_SEM_TYPE, "Incompatible data types when assigning to \"%s\".\n", id);}

        entry->data.used    = true;
        entry->data.changed = true;
        createAssignNode(newAssNode, id, newAssExpNode, NULL, varDataType);
        connectToBlock(newAssNode, block);
    }
    // RULE 30 <after_id> -> <builtin> ( <expr_params> )  ; 
    else if(currentToken.type == tokentype_dot || currentToken.type == tokentype_lbracket){
        DEBPRINT("KAKAKA %d %s\n", currentToken.type, id);
        astNode *newFCallNode = createAstNode();
        funCallHandle(id, newFCallNode, false);
        GT
        if(currentToken.type != tokentype_semicolon){
            ERROR(ERR_SYNTAX, "Expected: \";\" after calling function \"%s\".\n", id);
        }
        GT
        connectToBlock(newFCallNode, block);
        correct = true;
        
        DEBPRINT("Created call %s\n",id);
    }else{ERROR(ERR_SYNTAX, "Expected: \"=\" or \".\" or \"(\" after id \"%s\".\n",id);}


DEBPRINT(" %d\n", correct);
    return correct;
}



/**
 * @brief Handles function call logic.
 * 
 *        This function is used, when there is a statement that calls function. 
 *        Logic is separated from after_id function, because
 *        statement can be standalone when it is void-returning function, or 
 *        in expressions. This separation allows using this function both in
 *        top-down parsing as well as in bottom-up parsing of expressions.
 * 
 * @param id Name of function to process.
 * @param node Pointer to astNode where to save created funcCallNode
 * 
 * @note Function exits the whole program with suitable error message and code when encountering error. 
 */
void funCallHandle(char *id, astNode *node, bool inExpr){
        symNode *entry   = NULL;
        bool builtinCall = false;
        
        astNode **exprParamsArr = malloc(sizeof(astNode*)*MAX_PARAM_NUM);
        int paramCnt            = 0;
        char *betterID = NULL;
        builtin(id, &entry, &builtinCall, &betterID);
            if(currentToken.type == tokentype_lbracket){
                GT
                if(expr_params(exprParamsArr, &paramCnt)){
                    if(currentToken.type == tokentype_rbracket){
                    }
                }
            }
        
        
        if(entry == NULL){
            ERROR(ERR_SEM_UNDEF, "Function \"%s\" called but never defined.\n", id);
        }
        // already defined user function or builtin function was called, semantic check of the parameters and return type
        if(entry->data.data.fData.returnType != void_ && !inExpr){
            ERROR(ERR_SEM_FUN, "Non-void function \"%s\" called without storing the return value.\n", id);
        }
        
        if(entry->data.data.fData.returnType == void_ && inExpr){
            ERROR(ERR_SEM_FUN, "Void function \"%s\" called in expression.\n", id);
        }

        if(entry->data.data.fData.paramNum != paramCnt){
            ERROR(ERR_SEM_FUN, "Calling \"%s\" with wrong number of parameters.\nExpected: %d\nGot: %d\n", 
                                id, entry->data.data.fData.paramNum, paramCnt);
        }
        int badIndex = 0;
        if(!checkParameterTypes(entry->data.data.fData.paramTypes, exprParamsArr, paramCnt, &badIndex)){
            ERROR(ERR_SEM_FUN, "Parameter number %d in \"%s\" function call has wrong type.\n", badIndex, id);
        }
        DEBPRINT("BETTER %s\n", betterID);
        createFuncCallNode(node, betterID, entry->data.data.fData.returnType, builtinCall, entry, NULL, exprParamsArr, paramCnt);
        DEBPRINT("DEBAG %s\n", node->nodeRep.funcCallNode.id);
        DEBPRINT("Made %d\n ", entry->data.used );
}


bool assign_or_f_call(astNode *block){
    bool correct = false;
    // RULE 31 <assign_or_f_call> -> id <after_id>
    DEBPRINT("AJ EM ID %d\n", currentToken.type);
    if(currentToken.type == tokentype_id){
        char *id = currentToken.value;
        GT
        DEBPRINT("ajdi %s\n", id);
        correct = after_id(id, block);
    }else{ERROR(ERR_SYNTAX, "Expected: id .\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
}

bool builtin(char *id, symNode **symtableNode, bool *builtinCall, char **betterID){
    bool correct = false;
    // RULE 32 <builtin> -> . id
    DEBPRINT("NOT DOT %d\n", currentToken.type);
    if(currentToken.type == tokentype_dot){
        
        if(strcmp(id, "ifj") != 0){
            ERROR(ERR_SYNTAX, "Incorrect namespace for builtin functions. Expected: \"ifj\" Got: \"%s\" .\n", id);
        }
        GT
        if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if correct builtin name
            char *builtinName = currentToken.value;
            *betterID = builtinName;
            *symtableNode = checkBuiltinId(builtinName); // if there is no builtin with id, it exits with error
            correct = true;
            *builtinCall = true;
            GT
        }else{ERROR(ERR_SYNTAX, "Expected: builtin id.\n");}
    }
    // RULE 33 <builtin> -> ε
    else if(currentToken.type == tokentype_lbracket){
        *symtableNode = findSymNode(funSymtable->rootPtr, id);
        correct = true;
        *builtinCall = false;
        *betterID = id;
    }else{ERROR(ERR_SYNTAX, "Expected: \"(\" or \".\".\n");}
    DEBPRINT(" %d\n", correct);
    return correct;
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
        ERROR(ERR_SEM_UNDEF, "Definition for function \"main\" was not found.\n");
        return false;
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
 * @brief        Validates that all entries that were defined are also used in scope and checks if all called
 *               functions were defined.
 * 
 *               Function uses resursive preorder traversal to go through the BST.
 *               When an entry of a variable or function which used flag is false is found, function
 *               triggers error.
 *               When an entry of a function with defined flag is false, function triggers error.
 * 
 * @see          symData
 * 
 * @param root   Root of a (sub)tree to traverse through.
 * 
 */
void allUsed(symNode *root){

    if(root != NULL){

        if(root->data.varOrFun == 1 && !root->data.data.fData.defined){
            ERROR(ERR_SEM_UNDEF, "Function \"%s\" called but never defined.\n", root->key);
        }

        if(!root->data.used){
            if(root->data.varOrFun == 0){
                ERROR(ERR_SEM_UNUSED, "Variable \"%s\" defined but not used within block.\n", root->key);
            }
            else{
                ERROR(ERR_SEM_UNUSED, "Function \"%s\" defined but not used in a program.\n", root->key);
            }
        }

        if(root->data.varOrFun == 0 && !root->data.data.vData.isConst && !root->data.changed){
            ERROR(ERR_SEM_UNUSED, "Modifiable variable \"%s\" has no chance of changing after initialization.\n", root->key);
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
 * @brief    Returns return type of needed user defined function.
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


/**
 * @brief          Checks that an array of expressions have the correct dataTypes as
 *                 they are expected to have.
 * 
 * @param expected An array of expected data types from definiton of a function.
 * @param given    An array of pointers to astNode that represent the expression.
 * @param paramNum Number of parameters.
 * @param badIndex Pointer to a int flag signalising position of first incorrect parameter type.
 * 
 * @warning         Similar function is compareDataTypesArray. Make sure to use
 *                  the one with intended behaviour. 
 * 
 * @note            Used in funcCalls to validate input parameters given to the function.
 * 
 * @return          False if there is a mismatch, true if all valid.
 */
bool checkParameterTypes(dataType *expected, astNode **given, int paramNum, int *badIndex){
    dataType givenDataType;

    for(int i = 0; i < paramNum; i++){
        givenDataType = given[i]->nodeRep.exprNode.dataT;
        if(expected[i] != givenDataType && expected[i] != any){
            *badIndex = i;
            return false;
        }
    }
    return true;
}

/**
 * @brief          Compares two arrays of dataTypes.
 * 
 * @param expected An array of expected data types.
 * @param given    An array of given data types to compare with expected.
 * @param paramNum Number of parameters.
 * @param badIndex Pointer to a int flag signalising position of first incorrect parameter type.
 * 
 * @warning         Similar function is checkParameterTypes.. Make sure to use
 *                  the one with intended behaviour.           
 * 
 * @return         False if there is a mismatch, true if all valid.
 */
bool compareDataTypesArray(dataType *expected, dataType *given, int paramNum, int *badIndex){

    for(int i = 0; i < paramNum; i++){

        if(expected[i] != given[i]){
            *badIndex = i;
            return false;
        }
    }
    return true;
}

symNode *checkBuiltinId(char *id){
    DEBPRINT("hjehehehe %d \n", id==NULL);
    symNode *symtableNode = findSymNode(builtinSymtable->rootPtr, id);
    if(symtableNode == NULL){
        ERROR(ERR_SEM_UNDEF, "Builtin function with id \"%s\" does not exist.", id);
    }
    else{
        
        return symtableNode;
    }

}

astNode *parser(){
    initStack(&symtableStack);
    prepareBuiltinSymtable();
    funSymtable = createSymtable();

    prog(true);
    reset_scanner();
    prog(false);

    return ASTree.root;
}


/* EOF parser.c */