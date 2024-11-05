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
            if(currentToken.type == tokentype_equal){
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

    GT
    return correct;
}

bool code(){
    bool correct = false;

    // RULE 3 <code> -> <def_func>
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        correct = def_func();
    }

    GT
    return correct;
}

bool next_code(){
    bool correct = false;
    // RULE 4 <next_code> -> <code>
    if(currentToken.type == tokentype_keyword){ // TODO check if pub
        correct = code();
    }
    // RULE 5 <next_code> -> ε
    else if(tokentype_EOF){
        correct = true;
    }
    GT
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
    GT
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
    GT
    return correct;
}

bool params_n(){
    bool correct = false;


    GT
    return correct;
}

bool def_variable(){
    bool correct = false;


    GT
    return correct;
}

bool varorconst(){
    bool correct = false;


    GT
    return correct;
}

bool unused_decl(){
    bool correct = false;


    GT
    return correct;
}

bool type_normal(){
    bool correct = false;


    GT
    return correct;
}

bool type_null(){
    bool correct = false;


    GT
    return correct;
}

bool type(){
    bool correct = false;


    GT
    return correct;
}

bool type_func_ret(){
    bool correct = false;


    GT
    return correct;
}

bool type_var_def(){
    bool correct = false;


    GT
    return correct;
}

bool st(){
    bool correct = false;


    GT
    return correct;
}

bool body(){
    bool correct = false;


    GT
    return correct;
}

bool return_(){
    bool correct = false;


    GT
    return correct;
}

bool exp_func_ret(){
    bool correct = false;


    GT
    return correct;
}

bool id_without_null(){
    bool correct = false;


    GT
    return correct;
}

bool while_statement(){
    bool correct = false;


    GT
    return correct;
}

bool if_statement(){
    bool correct = false;


    GT
    return correct;
}

bool expr_params(){
    bool correct = false;


    GT
    return correct;
}

bool expr_params_n(){
    bool correct = false;


    GT
    return correct;
}

bool after_id(){
    bool correct = false;


    GT
    return correct;
}

bool assign_or_f_call(){
    bool correct = false;


    GT
    return correct;
}

bool builtin(){
    bool correct = false;


    GT
    return correct;
}


