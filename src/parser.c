/**
 *         Implementace překladače imperativního jazyka IFJ24.
 * 
 * @file   parser.c
 * 
 * @brief  Implementation of recursive parser for IFJ24
 * 
 *         Parser is based on LL1 grammar created for IFJ24 programming language, which is based on Zig.
 *         Each non-terminal has it's own function (using snake_case), in which syntacic and semantic checks are performed.
 *         Abstract syntactic tree (AST, defined in ast.h and ast.c) is built.
 *         
 *         Functions for rules return true when everything was valid, however, they will never
 *         return false, as in current implementation, when an error occurs, ERROR(...) defined in error.h 
 *         is called and the program exits with given error code.
 *         In future, this logic might easily be changed to perform recoveries when errors are
 *         detected. This was not required in the assignment of the project, however, ways and means
 *         are prepared for this extension.
 * 
 *         When an expression is encountered, control is given to expression parser 
 *         (expression_parser.h, expression_parser.c). 
 * 
 * @author xnovakf00 Filip Novák
 * @date   27.11.2024
*/

#include "parser.h"

/**
 * @brief               Processes the program.
 * 
 * @param firstTraverse Signalizes, whether the parser is conducting its first traverse.
 *                      
 *                      Semantic checks in this function:
 * 
 *                          - main function defined with correct parameters?
 * 
 *                          - all user-defined functions used in program?
 * 
 * @return              True if program processing is successful.
 */
bool prog(bool firstTraverse){
    GT

    if(!firstTraverse){
        ASTree.root = createRootNode(); // initialize ASTree
    }

    // RULE 1 <prog> -> <prolog> <code>
    if(currentToken.type != tokentype_kw_const){
        ERROR(ERR_SYNTAX, "Expected: \"const\".\n");
    }

    prolog();

    code(firstTraverse);

    mainDefined(); // check if main function is defined and has correct data
    if(!firstTraverse){
        allUsed(funSymtable->rootPtr); // check if all functions defined were also used in program
    }
    
    return true;
}

/**
 * @brief  Processes the compulsory prolog of input program to compile.
 * 
 * @return True if prolog processing is successful.
 */
bool prolog(){

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
    if (currentToken.type != tokentype_string || strcmp(currentToken.value, "ifj24.zig") != 0) { 
        ERROR(ERR_SYNTAX, "Incorrect expression in prologue. Expected: \"ifj24.zig\".\n");
    }
    GT
    if (currentToken.type != tokentype_rbracket) {
        ERROR(ERR_SYNTAX, "Expected: \")\"  .\n");
    }
    GT
    if (currentToken.type != tokentype_semicolon) {
        ERROR(ERR_SYNTAX, "Expected: \";\" (check line above as well).\n");
    }
    GT

    return true;
}

/**
 * @brief  Processes the code part of input program.
 * 
 * @param firstTraverse Signalizes, whether the parser is conducting its first traverse.
 * 
 * @return True if code processing is successful.
 */
bool code(bool firstTraverse){

    // RULE 3 <code> -> <def_func> <code>
    if(currentToken.type == tokentype_kw_pub){
        def_func(firstTraverse);
        code(firstTraverse);
    }

    // RULE 4 <code> -> ε
    else if(currentToken.type == tokentype_EOF){
        return true;
    }
    else{
        ERROR(ERR_SYNTAX, "Expected: \"pub\" %d\n", firstTraverse);
    }

    return true;
}

/**
 * @brief Processes function definitions in first traversal.
 * 
 *        Following information about user defined functions are collected:
 * 
 *              - ID of the function
 * 
 *              - IDs, types and number of parameters
 * 
 *              - return type
 * 
 *        The body part of function is skipped.
 * 
 *        Semantic checks in this function:
 * 
 *              - redefining already user-defined function?
 * 
 * @param functionEntry Pointer to a symNode representing the function in function symtable.
 * @param funID         ID of the function.
 * 
 * @return              True if processing is successful.
 */
bool def_func_first(symNode *functionEntry, char *funID){
    
    funData   entryData;
    symData   entrySymData;
    dataType *paramTypes     = malloc(sizeof(dataType)*MAX_PARAM_NUM);
    char    **paramNames     = malloc(sizeof(char *)*MAX_PARAM_NUM);
    bool     *paramNullable  = malloc(sizeof(bool)*MAX_PARAM_NUM);
    if(paramTypes == NULL || paramNames == NULL || paramNullable == NULL){
        ERROR(ERR_INTERNAL,"Malloc fail in def_func.\n");
    }
    int       paramNum = 0;
    dataType  returnType;
    bool      nullable;

    if(functionEntry != NULL){
        ERROR(ERR_SEM_REDEF, "Redefining function (%s) is not allowed.\n",funID);
    }
    
    symtable *symtableNewF = createSymtable();
    push(&symtableStack, symtableNewF);

    GT

    if(currentToken.type != tokentype_lbracket){
        ERROR(ERR_SYNTAX, "Expected: \"(\".\n");
    }
    
    GT
        
    params(&paramNum, &paramTypes, &paramNames, &paramNullable);

    if(currentToken.type != tokentype_rbracket){
        ERROR(ERR_SYNTAX, "Expected: \")\".\n");
    }

    GT
        
    type_func_ret(&nullable, &returnType);
    
    if(currentToken.type != tokentype_lcbracket){
        ERROR(ERR_SYNTAX, "Expected: \"{\".\n");
    }
    entryData.tbPtr         = pop(&symtableStack);
    entryData.defined       = true;
    entryData.paramNames    = paramNames;
    entryData.paramTypes    = paramTypes;
    entryData.paramNullable = paramNullable;
    entryData.nullableRType = nullable;
    entryData.returnType    = returnType;
    entryData.paramNum      = paramNum;

    entrySymData.data.fData = entryData;
    entrySymData.varOrFun   = 1;

    if(strcmp(funID, "main") == 0){
        entrySymData.used = true; // avoid false negative by allUsed() semantic check
    }
    else{
        entrySymData.used = false;
    }
    
    insertSymNode(funSymtable, funID, entrySymData);

    GT

    while(currentToken.type != tokentype_kw_pub && currentToken.type != tokentype_EOF){ // skip through the body
        GT
    }

    return true;

}

/**
 * @brief Processes function definitions in second traversal.
 * 
 *        Function header is skipped, body is processed.
 *        
 *        Semantic checks in this function:
 * 
 *            - non-void function has return statement in all executable paths?
 * 
 *            - all defined variables used within function?
 * 
 *            - all non-constant variables have a possibility of change?
 * 
 *        AST node of function definition is built.
 * 
 * @param functionEntry Pointer to a symNode representing the function in function symtable.
 * @param funID         ID of the function.
 * 
 * @return True if processing of function body is successful.
 */
bool def_func_sec(symNode *functionEntry, char *funID){

    astNode  *funcAstNode   = createAstNode();  // allocate node with no representation yet
    astNode  *bodyAstRoot   = createRootNode(); // create root node for body (statements in body will be connected to this)

    symtable *symtableFun   = functionEntry->data.data.fData.tbPtr;
    dataType  returnType    = getReturnType(funID);
    char    **paramNames    = functionEntry->data.data.fData.paramNames;
    int       paramNum      = functionEntry->data.data.fData.paramNum;
    bool      nullableRType = functionEntry->data.data.fData.nullableRType;
    bool      inMain        = (strcmp(funID, "main") == 0); 

    while(currentToken.type != tokentype_lcbracket){ // skip the function header (already processed in first traverse)
        GT
    }

    GT

    push(&symtableStack, symtableFun);

    body(returnType,nullableRType, bodyAstRoot, inMain);
    if(currentToken.type != tokentype_rcbracket){
        ERROR(ERR_SYNTAX, "Expected: %d\"}\".\n",currentToken.type);
    }

    symtableFun = pop(&symtableStack);

    allUsed(symtableFun->rootPtr); // perform semantic check of used variables
    if(returnType != void_){
        if(!allReturns(bodyAstRoot)){ // semantic check that all executable paths have return statement
                ERROR(ERR_SEM_RETURN, "Function \"%s\" include path with no \"return\" statement.", funID);
        }
    }
    
    // build AST node
    createDefFuncNode(funcAstNode, funID, symtableFun, bodyAstRoot, ASTree.root, paramNames, paramNum, returnType, nullableRType); 
    connectToBlock(funcAstNode, ASTree.root);
    GT
    return true;
}

/**
 * @brief Processes user-function definitions.
 * 
 * @param firstTraverse Signalizes, whether the parser is conducting its first traverse.
 * 
 * @return True if processing of function definition is successful.
 */
bool def_func(bool firstTraverse){

    // RULE 5 <def_func> -> pub fn id ( <params> ) <type_func_ret> { <body> }
    if(currentToken.type != tokentype_kw_pub){
        ERROR(ERR_SYNTAX, "Expected: \"pub\" .\n");
    }

    GT

    if(currentToken.type != tokentype_kw_fn){
        ERROR(ERR_SYNTAX, "Expected: \"fn\".\n");
    }
    
    GT

    if(currentToken.type != tokentype_id){
        ERROR(ERR_SYNTAX, "Expected: id.\n");
    }


    char *funID = currentToken.value;
    symNode *functionEntry = findSymNode(funSymtable->rootPtr, funID);
    
    if(firstTraverse){
        def_func_first(functionEntry, funID);
    }
    else{
        def_func_sec(functionEntry, funID);
    }

    
    return true;
}

/**
 * @brief Processes the params part of user-function definition.
 * 
 *        Semantic checks in this function:
 * 
 *              - parameters redefining?
 * 
 *              - parameters shadowing function?
 * 
 * @param paramNum      Pointer to variable representing number of parameters to set.
 * @param paramTypes    Pointer to an array of parameter data types to set.
 * @param paramNames    Pointer to an array of parameter names to set.
 * @param paramNullable Pointer to an array of flags whether parameters are nullable to set.
 * 
 * @return True if processing of parameters in function definition is successful.
 */
bool params(int *paramNum, dataType **paramTypes, char ***paramNames, bool **paramNullable){

    // preparing information about parameter
    char    *paramID;
    dataType paramType;
    bool     nullable;
    varData  entryVarData = {.inheritedType = 0, .isConst = 1}; // parameter has to be const and can't have inherited type
    symData  entryData    = {.varOrFun = 0};

    // RULE 6 <params> -> id : <type> <params_n>
    if(currentToken.type == tokentype_id){

        // check if redefining existing (two params with same name)
        paramID = currentToken.value;
        symNode *entry = findInStack(&symtableStack, paramID);
        if(entry != NULL){ERROR(ERR_SEM_REDEF, "Redefining variable (%s) is not allowed.\n",paramID);}
        entry = findSymNode(funSymtable->rootPtr, paramID);
        if(entry != NULL){ERROR(ERR_SEM_REDEF, "Shadowing function (%s) is not allowed.\n",paramID);}
        
        GT

        if(currentToken.type != tokentype_colon){
            ERROR(ERR_SYNTAX, "Expected: \":\"\n");
        }

        GT

        type(&nullable, &paramType) ;       
        // all information are known, set them accordingly
        (*paramNames)[*paramNum]    = paramID;
        (*paramTypes)[*paramNum]    = paramType;
        (*paramNullable)[*paramNum] = nullable;

        entryVarData.type       = paramType;
        entryVarData.isNullable = nullable;
        (*paramNum)++;
        entryData.data.vData    = entryVarData,
        insertSymNode(symtableStack.top->tbPtr, paramID, entryData);

        params_n(paramNum, paramTypes, paramNames, paramNullable);

    }

    // RULE 7 <params> -> ε
    else if(currentToken.type == tokentype_rbracket){
        return true;
    }
    else{ERROR(ERR_SYNTAX, "Expected: id or \")\".\n");}

    return true;
}

/**
 * @brief Function for processing subsequent parameters in definitons of functions.
 * 
 * @see   params()
 * 
 * @param paramNum      Pointer to variable representing number of parameters to set.
 * @param paramTypes    Pointer to an array of parameter data types to set.
 * @param paramNames    Pointer to an array of parameter names to set.
 * @param paramNullable Pointer to an array of flags whether parameters are nullable to set.
 * 
 * @return True if processing is successful.       
 */
bool params_n(int *paramNum, dataType **paramTypes, char ***paramNames, bool **paramNullable){
    bool correct = false;
    // RULE 8 <params_n> -> , <params>
    if(currentToken.type == tokentype_comma){
        GT
        correct = params(paramNum, paramTypes, paramNames, paramNullable);
    }
    // RULE 9 <params_n> -> ε
    else if(currentToken.type == tokentype_rbracket){
        correct = true;
    }
    else{
        ERROR(ERR_SYNTAX, "Expected: \",\" or \")\".\n");
    }
    return correct;
}

/**
 * @brief Processes variable definition statement.
 * 
 *        Semantic checks in function:
 * 
 *          - redefining variable?
 * 
 *          - variable shadowing a function?
 *          
 *          - data type of expression match?
 * 
 *          - deductable data type from expression if omitted?
 * 
 *        astNode for variable definition is built.
 * 
 * @param block Pointer to a astNode block to connect the new node to.
 *
 * @return      True if processing was successful.
 */
bool def_variable(astNode *block){

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
    
    // RULE 10 <def_variable> -> <varorconst> id <type_var_def> = expression ;
    if(currentToken.type != tokentype_kw_const && currentToken.type != tokentype_kw_var){
        ERROR(ERR_SYNTAX, "Expected: \"const\" or \"var\".\n");
    } 
        
    varorconst(&isConst);
    if(currentToken.type != tokentype_id){
        ERROR(ERR_SYNTAX, "Expected id.\n");
    }

        // perform semantic checks for redefinition and shadowing
        varName = currentToken.value;
        symNode *varEntry = findInStack(&symtableStack, varName);
        if(varEntry != NULL){ERROR(ERR_SEM_REDEF, "Redefining variable (%s) is not allowed.\n",varName);}
        varEntry = findSymNode(funSymtable->rootPtr, varName);
        if(varEntry != NULL){ERROR(ERR_SEM_REDEF, "Shadowing function (%s) is not allowed.\n",varName);}

        GT

        type_var_def(&nullable, &varType, &inheritedType);

        // set known data
        variData.isConst       = isConst;
        variData.inheritedType = inheritedType;
        variData.isNullable    = nullable;
        variData.type          = varType;
        entryData.used         = false;
        entryData.changed      = false;
        entryData.varOrFun     = 0;


        if(currentToken.type != tokentype_assign){
            ERROR(ERR_SYNTAX, "Expected: \"=\".\n");
        }
        
        GT

        expression(exprNode);

        // valid expression?
        if(checkIfExprLogic(exprNode)){
                ERROR(ERR_SEM_TYPE, "Expression in definition for variable \"%s\" cannot be logical.\n", varName);
        }

        if(currentToken.type != tokentype_semicolon){
            ERROR(ERR_SYNTAX, "Expected \";\".\n");
        }

        GT
        
        bool literalIsNull = false;
        bool isString = false;

        // is the value explicitly null?
        if(exprNode->nodeRep.exprNode.exprTree->type == AST_NODE_LITERAL){
            if(exprNode->nodeRep.exprNode.exprTree->nodeRep.literalNode.dataT == null_){
                literalIsNull = true;
            }
            if(exprNode->nodeRep.exprNode.exprTree->nodeRep.literalNode.dataT == string){
                isString = true;
            }
        }

        // if needed to inherit type from expression
        if(variData.inheritedType == true){
            if(literalIsNull){
                ERROR(ERR_SEM_INHERIT, "Data type for variable \"%s\" cannot be deduced from \"null\".\n", varName);
            }
            if(isString){
                ERROR(ERR_SEM_INHERIT, "Cannot assign string literal to variable \"%s\". Use ifj.string()\n", varName);
            }

            variData.type          = exprNode->nodeRep.exprNode.dataT;
            variData.isNullable    = exprNode->nodeRep.exprNode.isNullable;
            variData.inheritedType = false;
        }
        else{
            if(!variData.isNullable && exprNode->nodeRep.exprNode.isNullable){
                ERROR(ERR_SEM_TYPE, "Incompatible data types when assigning to \"%s\".\n", varName);
            }
            if(literalIsNull){}
            else if(variData.type != exprNode->nodeRep.exprNode.dataT){
                ERROR(ERR_SEM_TYPE, "Incompatible data types when assigning to \"%s\".\n", varName);
            }

        }

        // put the value from expression to symNode if possible
        if(isConst && exprNode->nodeRep.exprNode.knownDuringCompile){
            extractValueToConst(exprNode->nodeRep.exprNode.dataT, exprNode->nodeRep.exprNode.exprTree, &variData);
            variData.knownDuringCompile = true;
        }
        else{
            variData.knownDuringCompile = false;
        }

        entryData.data.vData = variData;
        insertSymNode(symtableStack.top->tbPtr, varName, entryData);

        varEntry = findInStack(&symtableStack, varName); // get the pointer to entry in symtable

        createDefVarNode(varAstNode ,varName, exprNode, varEntry, block); // create the correct representation
        connectToBlock(varAstNode, block); // connect it to create subtree (root will be the body of block)

    return true;
}

/**
 * @brief Processes the var/const part of variable definition.
 * 
 * @param isConst Pointer to a flag indicating that it is unchangable to set.
 * 
 * @return True if process is successful.
*/
bool varorconst(bool *isConst){

    // RULE 11 <varorconst> -> const
    if(currentToken.type == tokentype_kw_const){ 
        *isConst = 1;
        GT
    }
    // RULE 12 <varorconst> -> var
    else if(currentToken.type == tokentype_kw_var){ 
        *isConst = 0;
        GT
    }
    else{
        ERROR(ERR_SYNTAX, "Expected: \"const\" or \"var\".\n");
    }

    return true;
}

/**
 * @brief Processes unused declaration statement.
 * 
 *        astNode of unused declaration is built.
 * 
 * @param block Pointer to a block to connect the statement to.
 * 
 * @return True if processing was successful.
 */
bool unused_decl(astNode *block){

    // prepare empty nodes
    astNode *newUnused = createAstNode();
    astNode *expr      = createAstNode();

    // RULE 13 <unused_decl> -> _ = expression ;
    if(currentToken.type != tokentype_pseudovar){
        ERROR(ERR_SYNTAX, "Expected: \"_\".\n");
    }
    GT

    if(currentToken.type != tokentype_assign){
        ERROR(ERR_SYNTAX, "Expected: \"=\" .\n");
    }
    GT
     
    expression(expr);

    if(currentToken.type != tokentype_semicolon){
        ERROR(ERR_SYNTAX, "Expected: \";\" (check line above as well).\n");
    }
    GT
            

    // create node with correct data and connect it into block
    createUnusedNode(newUnused, expr, block);
    connectToBlock(newUnused, block);

    return true;
}

/**
 * @brief Processes core type specification. 
 * 
 * @param datatype Pointer to a datatType to set.
 * 
 * @return True if processing is successful.
*/
bool type_normal(dataType *datatype){

    // RULE 14 <type_normal> -> i32
    if(currentToken.type == tokentype_kw_i32){ 
        *datatype = i32;
        GT
    }
    // RULE 15 <type_normal> -> f64
    else if(currentToken.type == tokentype_kw_f64){
        *datatype = f64;
        GT
    }
    // RULE 16 <type_normal> -> [ ] u8
    else if(currentToken.type == tokentype_lsbracket){
        GT
        if(currentToken.type == tokentype_rsbracket)
        {
            GT
            if(currentToken.type == tokentype_kw_u8){ 
                *datatype = u8;
                GT
            }else{ERROR(ERR_SYNTAX, "Expected: \"u8\" .\n");}
        }else{ERROR(ERR_SYNTAX, "Expected: \"]\" .\n");}
    }else{ERROR(ERR_SYNTAX, "Expected: \"i32\" or \"f64\" or \"[\" .\n");}

    return true;
}

/**
 * @brief Processes nullable data type specification.
 * 
 * @param datatype Pointer to a datatType to set.
 * 
 * @return True if processing successful.
 */
bool type_null(dataType *datatype){

    // RULE 17 <type_null> -> ? <type_normal>
    if(currentToken.type != tokentype_nullid){
        ERROR(ERR_SYNTAX, "Expected: \"?\" .\n");
    }
   
    GT
    type_normal(datatype);

    return true;
}

/**
 * @brief Processes type specification (either nullable or not).
 * 
 * @param nullable Pointer to a flag to set if datatype can include null or not.
 * @param datatype Pointer to a datatype to set.
 * 
 * @return True if processing successful.
 */
bool type(bool *nullable, dataType *datatype){

    // RULE 18 <type> -> <type_normal>
    if(currentToken.type == tokentype_kw_f64 || 
       currentToken.type == tokentype_kw_i32 ||
       currentToken.type == tokentype_lsbracket){ 
        type_normal(datatype);
        *nullable = false;
    }
    // RULE 19 <type> -> <type_null>
    else if(currentToken.type == tokentype_nullid){
        *nullable = true;
        type_null(datatype);
    }else{ERROR(ERR_SYNTAX, "Expected: \"f64\" or \"i32\" or \"[\" or \"?\" .\n");}
    
    return true;
}

/**
 * @brief Function to process return type of function.
 * 
 * @param nullable Pointer to a flag to set if datatype can include null or not.
 * @param datatype Pointer to a datatype to set.
 * 
 * @return True if processing successful. 
 */
bool type_func_ret(bool *nullable, dataType *datatype){

    // RULE 20 <type_func_ret> -> <type>
    if( currentToken.type  == tokentype_kw_i32   ||
        currentToken.type == tokentype_kw_f64    ||
        currentToken.type == tokentype_lsbracket ||
        currentToken.type == tokentype_nullid){

        type(nullable , datatype);

    }
    // RULE 21 <type_func_ret> -> void
    else if(currentToken.type == tokentype_kw_void){ 

        *datatype = void_;
        *nullable = false;
        GT

    }else{ERROR(ERR_SYNTAX, "Expected: \"f64\" or \"i32\" or \"[\" or \"?\" or \"void\".\n");}

    return true;
}

/**
 * @brief Processes type specification when defining variable.
 * 
 * @param nullable       Pointer to a flag to set if datatype can include null or not.
 * @param datatype       Pointer to a datatype to set.
 * @param inheritedDType Pointer to a flag if inheritance from expression is needed to set.
 * 
 * @return True if processing was successful. 
 */
bool type_var_def(bool *nullable, dataType *datatype, bool *inheritedDType){

    // RULE 22 <type_var_def> -> : <type>
    if(currentToken.type == tokentype_colon){
        *inheritedDType = false;
        GT
        if(currentToken.type == tokentype_kw_i32
        || currentToken.type == tokentype_kw_f64
        || currentToken.type == tokentype_lsbracket 
        || currentToken.type == tokentype_nullid){ 

            type(nullable, datatype);

        }else{ERROR(ERR_SYNTAX, "Expecting valid data type.\n");}
    }
    // RULE 23 <type_var_def> -> ε
    else if(currentToken.type == tokentype_assign){

        *inheritedDType = true;
        
    }else{ERROR(ERR_SYNTAX, "Expected: \":\" or \"=\".\n");}

    return true;
}

/**
 * @brief Start of processing statements based on token on from scanner.
 * 
 * @param expReturnType Return type of a function for return_, ifElse and while statements.
 * @param nullableRType Nullability of a return type for return_, ifElse and while statements.
 * @param block         Pointer to a block to connect the current statement to.
 * @param inMain        Flag, whether return statement is in main function or a different one.
 * 
 * @return True if processing successful.
 */
bool st(dataType expReturnType, bool nullableRType, astNode *block, bool inMain){

    // RULE 33 <st> -> <def_variable>
    if(currentToken.type == tokentype_kw_const ||
       currentToken.type == tokentype_kw_var){
        def_variable(block);
    }
    // RULE 34 <st> -> <assign_or_f_call>
    else if(currentToken.type == tokentype_id){
        assign_or_f_call(block);
    }
    // RULE 35 <st> -> <unused_decl>
    else if(currentToken.type == tokentype_pseudovar){
        unused_decl(block);
    }
    // <st> RULE 36 -> <while_statement>
    else if(currentToken.type == tokentype_kw_while){ 
        while_statement(expReturnType,nullableRType, block, inMain);
    }
    // <st> RULE 37 -> <if_statement>
    else if(currentToken.type == tokentype_kw_if){ 
        if_statement(expReturnType, nullableRType, block, inMain);
    }
    // <st> RULE 38 -> <return>
    else if(currentToken.type == tokentype_kw_return){ 
        return_(expReturnType,nullableRType, block, inMain);
    }
    else{ERROR(ERR_SYNTAX, "Unexpected start of a statement.\n");}


    return true;
}

/**
 * @brief Processes the body part of function, ifElse or while statements.
 * 
 * @param expReturnType Return type of a function for return_, ifElse and while statements.
 * @param nullableRType Nullability of a return type for return_, ifElse and while statements.
 * @param block         Pointer to a block to connect the current statement to.
 * @param inMain        Flag, whether return statement is in main function or a different one.
 * 
 * @return True if processing is successful.
 */
bool body(dataType returnType, bool nullableRType, astNode *block, bool inMain){

    // RULE 39 <body> -> ε
    if(currentToken.type == tokentype_rcbracket){
        return true;
    }
    // RULE 40 <body> -> <st> <body>
    else if(currentToken.type == tokentype_kw_const  || 
            currentToken.type == tokentype_kw_var    ||
            currentToken.type == tokentype_kw_while  ||
            currentToken.type == tokentype_kw_if     ||
            currentToken.type == tokentype_kw_return ||
            currentToken.type == tokentype_id        ||
            currentToken.type == tokentype_pseudovar){
        st(returnType, nullableRType, block, inMain);
        body(returnType, nullableRType, block, inMain);
        
    }else if(currentToken.type == tokentype_EOF){ERROR(ERR_SYNTAX, "Unexpected EOF, source code unfinished.\n");}
    else{ERROR(ERR_SYNTAX, "Unexpected token.\n");}

    return true;
}

/**
 * @brief Processes the return statement in function bodies.
 * 
 *        astNode for return statement is built.
 * 
 * @param expReturnType Expected return type.
 * @param nullableRType Expected nullability of a return type.
 * @param block         Pointer to a block to connect the return statement to.
 * @param inMain        Flag, whether return statement is in main function or a different one.
 * 
 * @return True if processing is successful.
 */
bool return_(dataType expReturnType, bool nullableRetType, astNode *block, bool inMain){

    astNode *exprNode   = createAstNode(); // allocate empty node
    astNode *returnNode = createAstNode(); 

    // RULE 41 <return> -> return <exp_func_ret> ;
    if(currentToken.type != tokentype_kw_return){ 
        ERROR(ERR_SYNTAX, "Expected: \"return\".\n");
    }
    GT

    exp_func_ret(expReturnType, nullableRetType, &exprNode);

    if(currentToken.type != tokentype_semicolon){
        ERROR(ERR_SYNTAX, "Expected: \";\" (check line above as well).\n");
    }
    GT

    createReturnNode(returnNode, exprNode, expReturnType, block, inMain);
    connectToBlock(returnNode, block);
    
    return true;
}

/**
 * @brief Processes return expression.
 * 
 *        Semantic checks in function:
 *  
 *          - expression present when expected?
 * 
 *          - no extra expression in void functions?
 * 
 *          - data types of expression and return type match?
 * 
 * @param expReturnType   Expected return type.
 * @param nullableRetType Expected nullability of a return type.
 * @param exprNode        Pointer to an allocated astNode for expression.
 * 
 * @return True if processing successful.
 */
bool exp_func_ret(dataType expRetType, bool nullableRetType, astNode **exprNode){

    // RULE 42 <exp_func_ret> -> ε
    if(currentToken.type == tokentype_semicolon){
        if(expRetType == void_){
            free(*exprNode); // expression node will not be needed
            *exprNode = NULL;
        }
        else{
            ERROR(ERR_SEM_RETURN, "Missing return value in non-void function.\n");
        }
        
    }
    // RULE 43 <exp_func_ret> -> expression
    else{
        expression(*exprNode); 
        dataType exprType     = (*exprNode)->nodeRep.exprNode.dataT;
        bool     exprNullable = (*exprNode)->nodeRep.exprNode.isNullable;
        if(expRetType == void_){
            ERROR(ERR_SEM_RETURN, "Extra return value in void function.\n");
        }
        if(exprType != expRetType){
            if( !(nullableRetType && exprType == null_) ){ // check whether returning "null" and if it is possible
                ERROR(ERR_SEM_FUN, "Returning expression data type does not match function return type.\n");
            }
        }
        if(!nullableRetType && exprNullable){
            ERROR(ERR_SEM_FUN, "Returning expression can be null, but function cannot return null.\n");
        }
    }
    return true;
}

/**
 * @brief Processes |id_without_null| part of while and ifElse statements.
 * 
 *        Semantic checks in function:
 *          
 *          - id_without_null redefinition or function shadowing?
 * 
 * @param withNull     Pointer to flag to set if statement includes nullability part.
 * @param id_wout_null Pointer to a ID of the constant without null to set.
 * 
 * @return True if processing successful.
 */
bool id_without_null(bool *withNull, char **id_wout_null){

    // RULE 44 <id_without_null> -> | id |
    if(currentToken.type == tokentype_vbar){
        GT

        if(currentToken.type != tokentype_id){
            ERROR(ERR_SYNTAX, "Expected: id .\n");
        }

        *withNull     = true;
        *id_wout_null = currentToken.value;
        // semantic checks for redefinition
        symNode *symEntry = findInStack(&symtableStack, currentToken.value);
        if(symEntry != NULL){ERROR(ERR_SEM_REDEF, "Redefining variable (%s) is not allowed.\n",*id_wout_null);}
        symEntry = findSymNode(funSymtable->rootPtr, currentToken.value);
        if(symEntry != NULL){ERROR(ERR_SEM_REDEF, "Shadowing function (%s) is not allowed.\n",*id_wout_null);}

        // add the ID_WITHOUT_NULL to symtable for if/while
        varData variData = {.inheritedType = true, .isConst = true, .isNullable = false};
        symData data = {.varOrFun = 0, .used = false, .data.vData = variData};
        insertSymNode(symtableStack.top->tbPtr, currentToken.value, data);

        GT
        
        if(currentToken.type != tokentype_vbar){
            ERROR(ERR_SYNTAX, "Expected: \"|\" .\n");
        }
        
        GT

    }
    // RULE 45 <id_without_null> -> ε
    else if(currentToken.type == tokentype_lcbracket){
        *withNull     = false;
        *id_wout_null = NULL;
    }else{ERROR(ERR_SYNTAX, "Expected: \"{\" or \"|\".\n");}

    return true;
}

/**
 * @brief Processes while statement.
 * 
 *        Semantic checks in function:
 *
 *          - expression in condition has expected type?
 * 
 *          - are all defined variables used within block?
 * 
 *          - do all changable variables have a chance of changing?
 * 
 *        astNode for while is built.
 * 
 * @param expReturnType Expected return type if return statement appears in body.
 * @param nullableRType Expected nullability of a return type.
 * @param block         Pointer to a block to connect the while statement to.
 * @param inMain        Flag, whether while statement is in main function or a different one for possible return.
 *
 * @return True if process is successful.
 */
bool while_statement(dataType expRetType, bool nullableRType, astNode *block, bool inMain){

    // prepare empty nodes
    astNode *whileAstNode = createAstNode(); 
    astNode *condExprNode = createAstNode();
    astNode *bodyAstNode  = createRootNode();

    // prepare info needed for correct construction of ast node while
    bool withNull;
    char *id_wout_null;

    // create new symtable for while and push it
    symtable *whileSymTable = createSymtable();
    push(&symtableStack, whileSymTable);

    // RULE 46 <while_statement> -> while ( expression ) <id_without_null> { <body> }
    if(currentToken.type != tokentype_kw_while){
        ERROR(ERR_SYNTAX, "Expected: \"while\" .\n");
    }
    GT
    if(currentToken.type != tokentype_lbracket){
        ERROR(ERR_SYNTAX, "Expected: \"(\" .\n");
    }
    GT

    expression(condExprNode);

    if(currentToken.type != tokentype_rbracket){
        ERROR(ERR_SYNTAX, "Expected: \")\" .\n");
    }
    GT
    
    id_without_null(&withNull, &id_wout_null);

    if(!withNull){
        if(!checkIfExprLogic(condExprNode)){
            ERROR(ERR_SEM_TYPE, "Expression in while statement is not of logic type.\n");
        }
    }
    else{
        findInStack(&symtableStack, id_wout_null)->data.data.vData.type = condExprNode->nodeRep.exprNode.dataT;
        if(!checkIfNullable(condExprNode)){
            ERROR(ERR_SEM_TYPE, "Expression in while statement with null is not nullable.\n");
        }
    }

    if(currentToken.type != tokentype_lcbracket){
        ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");
    }
    GT

    body(expRetType, nullableRType, bodyAstNode, inMain);

    if(currentToken.type != tokentype_rcbracket){
        ERROR(ERR_SYNTAX, "Expected: \"}\" at the end of \"while\" .\n");
    }
    GT

    allUsed(symtableStack.top->tbPtr->rootPtr); // perform semantic check for used variables in block while
    // create node with correct info and connect it to block
    pop(&symtableStack); // pop, so scopes are not disturbed
    createWhileNode(whileAstNode, withNull, id_wout_null, condExprNode, bodyAstNode, whileSymTable, block);
    connectToBlock(whileAstNode, block);

    return true;
}

/**
 * @brief Processes ifElse statement.
 * 
 *        Semantic checks in function:
 *
 *          - expression in condition has expected type?
 * 
 *          - are all defined variables used within block?
 * 
 *          - do all changable variables have a chance of changing?
 * 
 *        astNode for ifElse is built.
 * 
 * @param expReturnType Expected return type if return statement appears in body.
 * @param nullableRType Expected nullability of a return type.
 * @param block         Pointer to a block to connect the ifElse statement to.
 * @param inMain        Flag, whether while statement is in main function or a different one for possible return.
 *
 * @return True if process is successful.
 */
bool if_statement(dataType expRetType, bool nullableRType, astNode *block, bool inMain){
    
    // prepare empty nodes
    astNode *ifElseNode   = createAstNode();
    astNode *ifNode       = createAstNode();
    astNode *elseNode     = createAstNode();
    astNode *bodyIfNode   = createRootNode();
    astNode *bodyElseNode = createRootNode(); 
    astNode *condExrpNode = createAstNode();

    // prepare information
    bool withNull;
    char *id_wout_null;

    // create new scope for if
    symtable *symtableForIf = createSymtable();
    push(&symtableStack, symtableForIf);

    // create new scope for else, don't push it yet, will be pushed after IF is processed
    symtable *symtableForElse = createSymtable();

    // RULE 47 <if_statement> -> if ( expression ) <id_without_null> { <body> } else { <body> } 
    if(currentToken.type == tokentype_kw_if){ 
            GT
        if(currentToken.type == tokentype_lbracket){
            GT
        if(expression(condExrpNode)){ 

        
        if(currentToken.type == tokentype_rbracket){
            GT
        if(id_without_null(&withNull, &id_wout_null)){
            if(!withNull){
                if(!checkIfExprLogic(condExrpNode)){
                    ERROR(ERR_SEM_TYPE, "Expression in if statement is not of logic type.\n");
                }
            }
            else{
                // inherit datatype of id_wout_null from expression in condition 
                findInStack(&symtableStack, id_wout_null)->data.data.vData.type = condExrpNode->nodeRep.exprNode.dataT; 
                if(!checkIfNullable(condExrpNode)){
                    ERROR(ERR_SEM_TYPE, "Expression in if statement with null is not nullable.\n");
                }
            }

        if(currentToken.type == tokentype_lcbracket){
            GT
        if(body(expRetType, nullableRType, bodyIfNode, inMain)){
        if(currentToken.type == tokentype_rcbracket){
            GT
        if(currentToken.type == tokentype_kw_else){ 
            allUsed(symtableStack.top->tbPtr->rootPtr); // perform semantic check for used variables in block if
            pop(&symtableStack); // pop the symtable for if so scopes are not disturbed
            push(&symtableStack, symtableForElse); // push the symtable for else 
            GT
        if(currentToken.type == tokentype_lcbracket){
            GT
        if(body(expRetType, nullableRType, bodyElseNode, inMain)){
        if(currentToken.type == tokentype_rcbracket){
        }else{ERROR(ERR_SYNTAX, "Expected: \"}\" .\n");}
            GT
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");};
        }else{ERROR(ERR_SYNTAX, "Expected: \"else\" .\n");};
        }else{ERROR(ERR_SYNTAX, "Expected: \"}\" .\n");}
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \"{\" .\n");}
        }
        }else{ERROR(ERR_SYNTAX, "Expected: \")\".\n");}
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

    return true;
}

/**
 * @brief Processes expression in parameters in function calls.
 * 
 * @param params   Array of parameter expressions to set.
 * @param paramCnt Pointer to a number of parameters to increase.
 * 
 * @return True if processing successful.
 */
bool expr_params(astNode **params, int *paramCnt){
    astNode *expr = createAstNode();
    // RULE 25 <expr_params> -> ε
    if(currentToken.type == tokentype_rbracket){
        free(expr); // expression node not needed
    }
    // RULE 24 <expr_params> -> expression <expr_params_n>
    else if(expression(expr)){ 

        params[*paramCnt] = expr;
        (*paramCnt)++;

        expr_params_n(params, paramCnt);
    }

    return true;
}

/**
 * @brief Processing the subsequent expressions as parameters in function calls.
 * 
 * @param params   Array of parameter expressions to set.
 * @param paramCnt Pointer to a number of parameters to increase.
 * 
 * @return True if processing successful.
 */
bool expr_params_n(astNode **params, int *paramCnt){
    // RULE 26 <expr_params_n> -> , <expr_params>
    if(currentToken.type == tokentype_comma){
        GT
        expr_params(params, paramCnt);
    }
    // RULE 27 <expr_params_n> -> ε 
    else if(currentToken.type == tokentype_rbracket){
        return true;
    }else{ERROR(ERR_SYNTAX, "Expected: \",\" or \")\" .\n");}

    return true;
}

/**
 * @brief Processes assignment and function call based on tokens.
 * 
 *        Based solely on id token it is not possible to decide, 
 *        whether assignment is performed or function called.
 *        This function is present to keep the grammar LL1.
 *  
 *
 * @param id    Id of called function or variable assigning to.
 * @param block Pointer to astNode of block to connect the statement to.
 * 
 * @see assignmentHandle()
 * @see funCallHandle()
 * 
 * @return True if processing successful.
 */
bool after_id(char *id, astNode *block){

    // RULE 28 <after_id> -> = expression ;
    if(currentToken.type == tokentype_assign){
        assignmentHandle(id, block);
    }
    // RULE 29 <after_id> -> <builtin> ( <expr_params> )  ; 
    else if(currentToken.type == tokentype_dot || currentToken.type == tokentype_lbracket){
        astNode *newFCallNode = createAstNode();
        funCallHandle(id, newFCallNode, false);
        GT
        if(currentToken.type != tokentype_semicolon){
            ERROR(ERR_SYNTAX, "Expected: \";\" after calling function \"%s\".\n", id);
        }
        GT
        connectToBlock(newFCallNode, block);
        
    }else{ERROR(ERR_SYNTAX, "Expected: \"=\" or \".\" or \"(\" after id \"%s\".\n",id);}

    return true;
}

/**
 * @brief       Handles assignment to variable logic.
 * 
 *              Semantic checks in function:
 *                  
 *                  - is variable defined?
 *                 
 *                  - is variable non-constant?
 * 
 *                  - are types of variable and expression compatible?
 *               
 *              astNode for assignment statement is built.
 * 
 * @param id    Id of called function or variable assigning to.
 * @param block Pointer to astNode of block to connect the statement to.
 */
void assignmentHandle(char *id, astNode *block){

    astNode *newAssNode     = createAstNode();
    astNode *newAssExpNode  = createAstNode();
    dataType varDataType    = unknown;
    bool     isNullable     = false;
    dataType expressionDT   = unknown;
    symNode *entry          = findInStack(&symtableStack, id);

    if(entry == NULL){ERROR(ERR_SEM_UNDEF, "Assigning to undefined variable \"%s\".\n",id);}
    if(entry->data.data.vData.isConst){ERROR(ERR_SEM_REDEF, "Assigning to unmodifiable (const) variable \"%s\".\n", id);}

    varDataType = entry->data.data.vData.type;
    isNullable  = entry->data.data.vData.isNullable;

    GT
    expression(newAssExpNode);

    // perform semantic checks
    if(checkIfExprLogic(newAssExpNode)){
        ERROR(ERR_SEM_TYPE, "Expression in assignment for variable \"%s\" cannot be logical.\n", id);
    }
    if(currentToken.type != tokentype_semicolon){
        ERROR(ERR_SYNTAX, "Expected \";\"");
    };
    GT
    
    expressionDT = newAssExpNode->nodeRep.exprNode.dataT;
    if(!isNullable && checkIfNullable(newAssExpNode) ){ // nullable expression cannot be assigned to nonnullable variable
        ERROR(ERR_SEM_TYPE, "Incompatible data types when assigning to \"%s\". \"%s\" is not nullable.\n", id, id);
    }

    if(!(isNullable && expressionDT == null_)){ // this condition is here to avoid datatype mismatch by next if condition
                                                // null_ is a different dataType and it would give false error
        if(expressionDT != varDataType){       
            ERROR(ERR_SEM_TYPE, "Incompatible data types when assigning to  \"%s\".\n", id);
        }
    }        

    // set data and build node
    entry->data.used    = true;
    entry->data.changed = true;
    createAssignNode(newAssNode, id, newAssExpNode, NULL, varDataType);
    connectToBlock(newAssNode, block);
    
    return;
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
 *        Semantic checks in this function:
 * 
 *              - is called function defined?
 * 
 *              - is void function called in expression?
 * 
 *              - is non-void function return value discarted?
 * 
 *              - correct number and types of parameters?
 * 
 * @param id     Name of function to process.
 * @param node   Pointer to astNode where to save created funcCallNode
 * @param inExpr Flag indicating whether function call is in expression or as standalone statement.
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
        
        if(entry == NULL){
            ERROR(ERR_SEM_UNDEF, "Function \"%s\" called but never defined.\n", id);
        }

            if(currentToken.type == tokentype_lbracket){
                GT
                if(expr_params(exprParamsArr, &paramCnt)){
                }
            }
        

        // already defined user function or builtin function was called, semantic check of the parameters and return type
        if(entry->data.data.fData.returnType != void_ && !inExpr){
            ERROR(ERR_SEM_FUN, "Non-void function \"%s\" called without storing the return value.\n", betterID);
        }
        
        if(entry->data.data.fData.returnType == void_ && inExpr){
            ERROR(ERR_SEM_FUN, "Void function \"%s\" called in expression.\n", betterID);
        }

        if(entry->data.data.fData.paramNum != paramCnt){
            ERROR(ERR_SEM_FUN, "Calling \"%s\" with wrong number of parameters.\nExpected: %d\nGot: %d\n", 
                                betterID, entry->data.data.fData.paramNum, paramCnt);
        }
        int badIndex = 0; // index on which wrong parameter is
        if(!checkParameterTypes(entry->data.data.fData.paramTypes, exprParamsArr, paramCnt, &badIndex)){
            ERROR(ERR_SEM_FUN, "Parameter number %d in \"%s\" function call has wrong type.\n", badIndex, betterID);
        }
        if(!checkParameterNullability(entry->data.data.fData.paramNullable, exprParamsArr, paramCnt, &badIndex)){
            ERROR(ERR_SEM_FUN, "Parameter number %d in \"%s\" function call cannot be nullable.\n", badIndex, betterID);
        }
        createFuncCallNode(node, betterID, entry->data.data.fData.returnType, builtinCall, entry, NULL, exprParamsArr, paramCnt, entry->data.data.fData.nullableRType);
}

/**
 * @brief Processes statement that start with ID.
 * 
 * @param block Pointer to a astNode of a block to connect statements to.
 * 
 * @return True if processing successful.
 */
bool assign_or_f_call(astNode *block){

    // RULE 30 <assign_or_f_call> -> id <after_id>
    if(currentToken.type == tokentype_id){
        char *id = currentToken.value;
        GT
        after_id(id, block);
    }else{ERROR(ERR_SYNTAX, "Expected: id .\n");}

    return true;
}
/**
 * @brief Processes optional namespace for builtin functions when function calling.
 * 
 *        Function decides whether a builtin function is called or user-defined function is called.
 *        Based on this in performs syntactic checks and sets flags accordingly.
 * 
 * @param id          Id of the function called or ifj in case it is namespace.
 * @param builtinCall Pointer to a flag indicating that builtin function is called to set.
 * @param betterID    Pointer to an id of a function or builtin function to set after processing.
 * 
 * @return True if processing successful.
 */
bool builtin(char *id, symNode **symtableNode, bool *builtinCall, char **betterID){
    // RULE 31 <builtin> -> . id
    if(currentToken.type == tokentype_dot){
        
        if(strcmp(id, "ifj") != 0){
            ERROR(ERR_SYNTAX, "Incorrect namespace for builtin functions. Expected: \"ifj\" Got: \"%s\" .\n", id);
        }
        GT
        if(currentToken.type == tokentype_id){ 
            char *builtinName = currentToken.value;
            *betterID = builtinName;
            *symtableNode = checkBuiltinId(builtinName); // if there is no builtin with id, it exits with error
            *builtinCall = true;
            GT
        }else{ERROR(ERR_SYNTAX, "Expected: builtin id.\n");}
    }
    // RULE 32 <builtin> -> ε
    else if(currentToken.type == tokentype_lbracket){
        *symtableNode = findSymNode(funSymtable->rootPtr, id);
        *builtinCall = false;
        *betterID = id;
    }else{ERROR(ERR_SYNTAX, "Expected: \"(\" or \".\".\n");}

    return true;
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
 * @see    findSymNode
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
 * @brief      Validates that all entries that were defined are also used in scope and checks if all called
 *             functions were defined.
 * 
 *             Function uses resursive preorder traversal to go through the BST.
 *             When an entry of a *variable* or *function* which "used" flag is false is found, function
 *             triggers error.
 *             When an entry of a *variable* that is not constant which "changed" flag is false is found,
 *             function triggers error.
 * 
 *             When an entry of a *function* with "defined" flag is false, function triggers error.
 * 
 * @see        symData
 * 
 * @param root Root of a (sub)tree to traverse through.
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
            ERROR(ERR_SEM_UNUSED, "Modifiable variable \"%s\" has no chance of changing after initialization within block.\n", root->key);
        }
        allUsed(root->l);
        allUsed(root->r);
    }

}


/**
 * @brief     Function checks whether variable was defined in scope or before current scope.
 * 
 * @param ID  Name of the function or variable.
 * 
 * @return    True if was defined, false if not.
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
            if(expected[i] == stringOru8 && 
            (givenDataType == string || givenDataType == u8)){
                return true;
            }
            *badIndex = i;
            return false;
        }
    }
    return true;
}


/**
 * @brief          Checks that an array of expressions have the correct nullability as
 *                 they are expected to have.
 * 
 * @param expected An array of expected nullability from definiton of a function.
 * @param given    An array of pointers to astNode that represent the expression.
 * @param paramNum Number of parameters.
 * @param badIndex Pointer to a int flag signalising position of first incorrect parameter type.
 *  
 * @note           Used in funcCalls to validate input parameters given to the function.
 * 
 * @return         False if there is a mismatch, true if all valid.
 */
bool checkParameterNullability(bool *expected, astNode **given, int paramNum, int *badIndex){
    
    bool givenNullable;

    for(int i = 0; i < paramNum; i++){
        givenNullable = given[i]->nodeRep.exprNode.isNullable;
        if(!expected[i] && givenNullable){ // expected is not nullable, but given expression is nullable, which is an error
            *badIndex = i;
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates that given builtin function id is valid.
 * 
 *        Function searches builtinSymtable for an entry with key given
 *        by id. If it is not found, semantic error is triggered.
 *        
 * @see findSymNode()
 * 
 * @param id Id of the builtin function to find.
 * 
 * @return Entry of the builtin function in builtinSymtable if valid.
 */
symNode *checkBuiltinId(char *id){
    symNode *symtableNode = findSymNode(builtinSymtable->rootPtr, id);
    if(symtableNode == NULL){
        ERROR(ERR_SEM_UNDEF, "Builtin function with id \"%s\" does not exist.\n", id);
    }
    else{
        return symtableNode;
    }

}

/**
 * @brief Checks whether given expression is of logic type.
 * 
 * @param expr Expression node to check.
 * 
 * @return True if expression is logic, false otherwise.
 */
bool checkIfExprLogic(astNode *expr){
    if(expr->type == AST_NODE_EXPR){ // is it even expression?
        astExpr repr = expr->nodeRep.exprNode;

        //EQUAL,                  // 4
        //NOT_EQUAL,              // 5
        //LOWER,                  // 6
        //GREATER,                // 7
        //LOWER_OR_EQUAL,         // 8
        //GREATER_OR_EQUAL,       // 9

        if(repr.exprTree->type == AST_NODE_BINOP){ // is it even binary operator on top?
            if(repr.exprTree->nodeRep.binOpNode.op >= EQUAL && 
               repr.exprTree->nodeRep.binOpNode.op <= GREATER_OR_EQUAL){ // is the binary operator logical operator?
                    return true;
               }
        }

    }

    return false;
}

/**
 * @brief Check whether expression is nullable.
 * 
 *        Function is used in checking whether expression/id in while and ifElse
 *        statements can be null when needed.
 * 
 * @param expr Expression node to check nullability from.
 * 
 * @return True if expression is nullable, false otherwise.
 */
bool checkIfNullable(astNode *expr){
    if(expr->type == AST_NODE_EXPR){ // is even expression
        if(expr->nodeRep.exprNode.isNullable){ 
            return true;
        }
    }
    return false;
}

/**
 * @brief          Extracts value from expression and assigns it to value of variable in it's symNode.
 * 
 * @param exprType Type of expression we are extracting the value from.
 * @param exprTree Root of the tree of the expression (should only be one astNode).
 * @param variData Pointer to variable data to put into symtable.
 */
void extractValueToConst(dataType exprType, astNode *exprTree, varData *variData){

    if(exprTree->type == AST_NODE_LITERAL){
        astLiteral literal = exprTree->nodeRep.literalNode;
        switch(exprType){
            case f64:
                variData->value.floatData = literal.value.floatData;
                variData->nullValue = false;
                break;

            case i32:
                variData->value.intData = literal.value.intData;
                variData->nullValue = false;
                break;
            
            case null_:
                variData->nullValue = true;
                break;
            default:
                break;
        }

    }
    else if(exprTree->type == AST_NODE_VAR){
        varData vData = exprTree->nodeRep.varNode.symtableEntry->data.data.vData;
        switch(exprType){
            case f64:
                variData->value.floatData = vData.value.floatData;
                variData->nullValue = false;
                break;

            case i32:
                variData->value.intData = vData.value.intData;
                variData->nullValue = false;
                break;
            
            case null_:
                variData->nullValue = true;
                break;
            default:
                    break;
        }
    }

}


/**
 * @brief  Validating that body has always-reachable return statement.
 *         
 *         Using depth first traversal through AST 
 *         recursively checks whether every possible execution
 *         path in given body contains return statement.
 * 
 * @param statement Statement to traverse through.
 * 
 * @return True if in all (sub)bodies return statement appears.
 */
bool allReturns(astNode *statement){

    if(statement == NULL){ // end of body
        return 0; // no return statement found in (sub)body
    }

    switch(statement->type){

        case AST_NODE_RETURN:  // return statement found, propagating the information upstream
            return true;
        
        case AST_NODE_IFELSE: // two possible execution paths, both need to contain return statement
            bool ifB = allReturns(statement->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.body); // traversing if body
            bool elseB = allReturns(statement->nodeRep.ifElseNode.elsePart->nodeRep.elseNode.body); // traversing else body
            if(ifB && elseB){ // both contain return, (sub)body contains return
                return true;
            }
            else{
                return allReturns(statement->next); // at least one does not contain return, we have to traverse next statements
                                                    // after the ifElse statement
            }

        case AST_NODE_WHILE: // one possible execution path
            bool whileB = allReturns(statement->nodeRep.whileNode.body); // traverse while body
            if(whileB){
                return true;
            }
            else{
                return allReturns(statement->next);
            }
        
        default: // any other statement cannot have return, moving to the next statement
            return allReturns(statement->next);
    }
}

/**
 * @brief  Encapsulates the logic for recursive parser.
 * 
 *         symtableStack is initialised, builtin symtable is prepared,
 *         symtable for functions is created, first traversal is performed,
 *         scanner reset and second traversal is performed.
 * 
 * @return Root of the AST.
 */
astNode *parser(){

    initStack(&symtableStack);
    prepareBuiltinSymtable();
    funSymtable = createSymtable();

    prog(true); // first pass, just collect information about defined functions
    reset_scanner();
    prog(false); // second pass, do everything else

    return ASTree.root;
}


/* EOF parser.c */