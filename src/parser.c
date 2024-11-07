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

//#define DEBUG
#ifdef DEBUG
    #define DEBPRINT(...) \
        fprintf(stderr, "D: %d: ",  __LINE__); \
        fprintf(stderr, __VA_ARGS__);
#else
    #define DEBPRINT(...) 
#endif

Token currentToken;


bool prog(){
    bool correct = false;
    // RULE 1 <prog> -> <prolog> <code> <next_code> // EOF MBY
    if(currentToken.type == tokentype_keyword){ // TODO check if const
        correct = prolog();
        correct = correct && code();
        correct = correct && next_code();
        // TODO Check eof idk how rn
    }
    DEBPRINT("prog %d\n", correct);
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
    DEBPRINT("prolog %d\n", correct);
    return correct;
}

bool code(){
    bool correct = false;
    DEBPRINT("current in code %d\n",currentToken.type);
    // RULE 3 <code> -> <def_func>
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        correct = def_func();
    }
    DEBPRINT("code %d\n", correct);
    return correct;
}

bool next_code(){
    bool correct = false;
    // RULE 4 <next_code> -> <code>
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        correct = code();
    }
    // RULE 5 <next_code> -> ε
    else if(currentToken.type == tokentype_EOF){
        correct = true;
    }
    DEBPRINT("next_code %d\n", correct);
    return correct;
}

bool def_func(){
    bool correct = false;
    // RULE 6 <def_func> -> pub fn id ( <params> ) <type_func_ret> { <body> }
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        GT
        if(currentToken.type == tokentype_keyword){ // TODO check if fn
        GT
        if(currentToken.type == tokentype_id){
        
        // check for redefining already defined function
        symNode *functionEntry = findSymNode(funSymtable->rootPtr, currentToken.value);
        if(functionEntry != NULL){
            if(functionEntry->data.data.fData.defined){}
        } // TODO SEMANTIC error 5 redefinition
        
        // create new scope and push it
        symtable *symtableNewF = createSymtable();
        push(&symtableStack, symtableNewF);

        // TODO get info from other parts
        funData entryData;
        symData entrySymData = {.varOrFun = 1, .data.fData = entryData};
        char   *entryFunName = currentToken.value;
        insertSymNode(funSymtable, entryFunName, entrySymData); // PUT SOMEWHERE ELSE

        GT
        if(currentToken.type == tokentype_lbracket){
        GT
        if(params()){
        if(currentToken.type == tokentype_rbracket){
        GT
        if(type_func_ret(&entryData.nullableRType, &entryData.returnType)){
        if(currentToken.type == tokentype_lcbracket){
        GT
        if(body()){
        correct = (currentToken.type == tokentype_rcbracket);
        GT
        }}}}}}}}
    }
    pop(&symtableStack); // TODO pop symtable into function node
    DEBPRINT("deffunc %d\n", correct);
    return correct;
}

bool params(){
    bool correct = false;
    // RULE 7 <params> -> id : <type> <params_n>
    if(currentToken.type == tokentype_id){
        GT
        symNode *entry = findInStack(&symtableStack, currentToken.value);
        if(entry != NULL){} // TODO SEMANTIC ERROR 5
        varData entryVarData = {.inheritedType = 0, .isConst = 1};
        symData entryData = {.data.vData = entryVarData, .varOrFun = 0};
        char *paramName = currentToken.value;

        if(currentToken.type == tokentype_colon){
            GT
            if(type(&entryVarData.isNullable, &entryVarData.type)){
                
                insertSymNode(symtableStack.top->tbPtr, paramName, entryData);

                correct = params_n();
            }
        }
    }
    // RULE 8 <params> -> ε
    else{
        correct = (currentToken.type == tokentype_rbracket);
        
    }
    DEBPRINT("params %d\n", correct);
    return correct;
}

// TODO CHECK THIS WHOLE FUNCTION
bool params_n(){
    bool correct = false;
    // RULE 9 <params_n> -> , <params>
    if(currentToken.type == tokentype_comma){
        GT
        correct = params();
    }
    // RULE 10 <params_n> -> ε
    else{
        correct = (currentToken.type == tokentype_rbracket);
        
    }
    DEBPRINT("params_n %d\n", correct);
    return correct;
}

bool def_variable(){
    bool correct = false;
    // RULE 11 <def_variable> -> <varorconst> id <type_var_def> = expression ;
            
    if(currentToken.type == tokentype_keyword){ // TODO check if const or var
        bool isConst;
        if(varorconst(&isConst)){
            DEBPRINT("dadsanjk curr %d\n", currentToken.type);
            if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if redefining or sth

                symNode *varEntry = findInStack(&symtableStack, currentToken.value);
                if(varEntry != NULL){} // TODO SEMANTIC error 5 redefinition
                // TODO get info from other parts

                varData variData  = {.isConst = isConst};
                symData entryData = {.data.vData = variData, .used = false, .varOrFun = 0};
                char   *entryName = currentToken.value;
                GT

                if(type_var_def(&variData.isNullable, &variData.type, &variData.inheritedType)){
                    DEBPRINT("current in defvar %d \n", currentToken.type); 
                
                if(currentToken.type == tokentype_assign){
                    GT
                    if(expression()){ // TODO EXPRESSION
                        correct = (currentToken.type == tokentype_semicolon);
                        GT
                    }
                }
                insertSymNode(symtableStack.top->tbPtr, entryName, entryData);
                }}}}
    DEBPRINT("def_var %d\n", correct);
    return correct;
}

bool varorconst(bool *isConst){
    bool correct = false;
    DEBPRINT("current in varorconst %d\n", currentToken.type);
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
    DEBPRINT("varorconst %d %s\n", correct, currentToken.value);
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
    DEBPRINT("unuseddecl %d\n", correct);
    return correct;
}

bool type_normal(dataType *datatype){
    bool correct = false;
    // RULE 15 <type_normal> -> i32
    // RULE 16 <type_normal> -> f64
    DEBPRINT("type_normal current %d\n", currentToken.type);
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
DEBPRINT("typenormal %d\n", correct);
    return correct;
}

bool type_null(dataType *datatype){
    bool correct = false;
    // RULE 18 <type_null> -> ? <type_normal>
    if(currentToken.type == tokentype_nullid){
        GT
        correct = type_normal(datatype);
    }
    DEBPRINT("typenull %d\n", correct);
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
    DEBPRINT("type %d\n", correct);
    return correct;
}

bool type_func_ret(bool *nullable, dataType *datatype){
    bool correct = false;
    // RULE 21 <type_func_ret> -> <type>
    DEBPRINT("current in type func rec %d\n", currentToken.type);
    if(currentToken.type == tokentype_keyword){ // TODO add check i32 f64 [ ?
        correct = type(nullable , datatype);
    }
    // RULE 22 <type_func_ret> -> void
    else if(currentToken.type == tokentype_keyword){ // TODO add check void
        correct = true;
        GT
    }
DEBPRINT("type func ret %d\n", correct);
    return correct;
}

bool type_var_def(bool *nullable, dataType *datatype, bool *inheritedDType){
    bool correct = false;
    // RULE 23 <type_var_def> -> : <type>
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
DEBPRINT("type var def %d\n", correct);
    return correct;
}

bool st(){
    bool correct = false;
    // RULE 34 <st> -> <def_variable>
    if(currentToken.type == tokentype_keyword){ // TODO check if const/var
        correct = def_variable();
    }
    // RULE 35 <st> -> <assign_or_f_call>
    else if(currentToken.type == tokentype_id){
        correct = assign_or_f_call();
    }
    // RULE 36 <st> -> <unused_decl>
    else if(currentToken.type == tokentype_pseudovar){
        correct = unused_decl();
    }
    // <st> RULE 37 -> <while_statement>
    else if(currentToken.type == tokentype_keyword){ // TODO check if while
        correct = while_statement();
    }
    // <st> RULE 38 -> <if_statement>
    else if(currentToken.type == tokentype_keyword){ // TODO check if if
        correct = if_statement();
    }
    // <st> RULE 39 -> <return>
    else if(currentToken.type == tokentype_keyword){ // TODO check if return
        correct = return_();
    }


    return correct;
}

bool body(){
    bool correct = false;
    // RULE 40 <body> -> ε
    DEBPRINT("current in body %d \n", currentToken.type);
    if(currentToken.type == tokentype_rcbracket){
        correct = true;
    }
    // RULE 41 <body> -> <st> <body>
    else if(currentToken.type == tokentype_keyword || // TODO check keywords
            currentToken.type == tokentype_id ||
            currentToken.type == tokentype_pseudovar){
        
        if(st()){
            correct = body();
        }
    }

    DEBPRINT("body %d\n", correct);
    return correct;
}

bool return_(){
    bool correct = false;
    // RULE 42 <return> -> return <exp_func_ret> ;
    if(currentToken.type == tokentype_keyword){ // TODO check if keyword == return
        GT
        if(exp_func_ret()){
            correct = (currentToken.type == tokentype_semicolon);
            GT
        }
    } 
    DEBPRINT("ret %d\n", correct);
    return correct;
}

bool exp_func_ret(){
    bool correct = false;
    // RULE 43 <exp_func_ret> -> ε
    if(currentToken.type == tokentype_semicolon){
        correct = true;
    }
    // RULE 44 <exp_func_ret> -> expression
    else{
        
        correct = expression(); // TODO EXPRESSION
    }
    DEBPRINT("exp func ret %d\n", correct);
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
DEBPRINT("id without null %d\n", correct);
    return correct;
}

bool while_statement(){
    bool correct = false;
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
                            if(body()){
                                correct = (currentToken.type == tokentype_rcbracket);
                                GT
                            }
                        }
                    }
                }
            }
        }
    }
DEBPRINT("while %d\n", correct);
    return correct;
}

bool if_statement(){
    bool correct = false;

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
        if(body()){
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
        if(body()){
        correct = (currentToken.type == tokentype_rcbracket);
            GT
        }}}}}}}}}}
        pop(&symtableStack); // TODO pop the symtable into else node 
    }
    DEBPRINT("if %d\n", correct);
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
DEBPRINT("expr params %d\n", correct);
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
    DEBPRINT("expr params n %d\n", correct);
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
DEBPRINT("afterid %d\n", correct);
    return correct;
}

bool assign_or_f_call(){
    bool correct = false;
    // RULE 31 <assign_or_f_call> -> id <after_id>
    if(currentToken.type == tokentype_id){
        GT
        correct = after_id();
    }
    DEBPRINT("ass or f call %d\n", correct);
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
    DEBPRINT("builtin %d\n", correct);
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
}

/* EOF parser.c */