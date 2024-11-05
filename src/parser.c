#include "parser.h"

#define GT currentToken = getToken(); // encapsulating the assignment

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
                            }
                        }
                    }
                }
            }
        }
    }

    return correct;
}

bool code(){
    bool correct = false;

    // RULE 3 <code> -> <def_func>
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        correct = def_func();
    }

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

    return correct;
}

bool def_func(){
    bool correct = false;
    // RULE 6 <def_func> -> pub fn id ( <params> ) <type_func_ret> { <body> }
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        GT
        if(currentToken.type == tokentype_keyword){ // TODO check if fn
            GT
            if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if redefiniton
                GT
                if(currentToken.type == tokentype_lbracket){
                    GT
                    if(params()){
                        GT
                        if(type_func_ret()){
                            GT
                            if(currentToken.type == tokentype_lcbracket){
                                GT
                                if(body()){
                                    GT
                                    correct = (currentToken.type == tokentype_rcbracket);
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    return correct;
}

bool params(){
    bool correct = false;
    // RULE 7 <params> -> id : <type> <params_n>
    if(currentToken.type == tokentype_id){
        GT
        if(currentToken.type == tokentype_colon){
            GT
            if(type()){
                GT
                correct = params_n();
            }
        }
    }
    // RULE 8 <params> -> ε
    else{
        correct = (currentToken.type == tokentype_rbracket);
    }
    
    return correct;
}

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
    
    return correct;
}

bool def_variable(){
    bool correct = false;
    // RULE 11 <def_variable> -> <varorconst> id <type_var_def> = expression ; 
    if(currentToken.type == tokentype_keyword){ // TODO check if const or var
        GT
        if(varorconst()){
            GT
            if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if redefining or sth
                GT
                if(type_var_def()){
                    GT
                    if(currentToken.type == tokentype_assign){
                        if(expression()){ // TODO EXPRESSION
                            GT
                            correct = (currentToken.type == tokentype_semicolon);
                        }
                    }
                }
            }
        }
    }
    
    return correct;
}

bool varorconst(){
    bool correct = false;
    if(currentToken.type == tokentype_keyword){
        // RULE 12 <varorconst> -> const
        if(true){ // TODO check if keyword == const
            correct == true;
        }
        // RULE 13 <varorconst> -> var
        else if(true){ // TODO check if keyword == var
            correct == true;
        }
    }
    
    return correct;
}

bool unused_decl(){
    bool correct = false;
    // RULE 14 <unused_decl> -> _ = expression ;
    if(currentToken.type == tokentype_underscore){
        GT
        if(currentToken.type == tokentype_assign){
            GT
            if(expression()){ // TODO EXPRESSION
                GT
                correct = (currentToken.type == tokentype_semicolon);
            }
        }
    }
    
    return correct;
}

bool type_normal(){
    bool correct = false;
    // RULE 15 <type_normal> -> i32
    // RULE 16 <type_normal> -> f64
    if(currentToken.type == tokentype_keyword){ // TODO check if keyword == i32 or f64
        correct = true;
    }
    // RULE 17 <type_normal> -> [ ] u8
    else if(currentToken.type == tokentype_lsbracket){
        GT
        if(currentToken.type == tokentype_rsbracket)
        {
            GT
            if(currentToken.type == tokentype_keyword){ // TODO check if keyword == u8
                correct = true;
            }
        }
    }

    return correct;
}

bool type_null(){
    bool correct = false;
    // RULE 18 <type_null> -> ? <type_normal>
    if(currentToken.type == tokentype_nullid){
        GT
        correct = type_normal();
    }
    
    return correct;
}

bool type(){
    bool correct = false;
    // RULE 19 <type> -> <type_normal>
    if(currentToken.type == tokentype_keyword || 
       currentToken.type == tokentype_lsbracket){ // TODO check if keyword == i32 or f64
            GT
            correct = type_normal();
    }
    // RULE 20 <type> -> <type_null>
    else if(currentToken.type == tokentype_nullid){
        GT
        correct = type_null();
    }
    
    return correct;
}

bool type_func_ret(){
    bool correct = false;


    
    return correct;
}

bool type_var_def(){
    bool correct = false;


    
    return correct;
}

bool st(){
    bool correct = false;


    
    return correct;
}

bool body(){
    bool correct = false;


    
    return correct;
}

bool return_(){
    bool correct = false;
    // RULE 42 <return> -> return <exp_func_ret> ;
    if(currentToken.type == tokentype_keyword){ // TODO check if keyword == return
        GT
        if(exp_func_ret()){
            GT
            correct = (currentToken.type == tokentype_semicolon);
        }
    } 
    
    return correct;
}

bool exp_func_ret(){
    bool correct = false;


    
    return correct;
}

bool id_without_null(){
    bool correct = false;


    
    return correct;
}

bool while_statement(){
    bool correct = false;


    
    return correct;
}

bool if_statement(){
    bool correct = false;


    
    return correct;
}

bool expr_params(){
    bool correct = false;


    
    return correct;
}

bool expr_params_n(){
    bool correct = false;


    
    return correct;
}

bool after_id(){
    bool correct = false;


    
    return correct;
}

bool assign_or_f_call(){
    bool correct = false;
    // RULE 31 <assign_or_f_call> -> id <after_id>
    if(currentToken.type == tokentype_id){
        GT
        correct = after_id();
    }
    
    return correct;
}

bool builtin(){
    bool correct = false;
    // RULE 32 <builtin> -> . id
    if(currentToken.type == tokentype_dot){
        GT
        if(currentToken.type == tokentype_id){ // TODO SEMANTIC check if correct builtin name
            correct = true;
        }
    }
    // RULE 33 <builtin> -> ε
    else{
        correct = (currentToken.type == tokentype_lbracket);
    }
    
    return correct;
}


