/**
 * @file   code_generator.c
 * @brief  Implementation file for managing code generation functions and buffer operations.
 * 
 * 
 * @author xskovaj00
 * @date   7.11.2024
 */
#include "code_generator.h"
#include <stdio.h>
#include <stdbool.h>
#include "code_buffer.h"
#include "ast.h"

#define BUFFER buf

Buffer_ll *BUFFER;

// Add code to buffer with error handling
#define add_push_code(val) \
    do { \
        if (!buf_add_push(BUFFER, val)) { \
            return false; \
        } \
    } while (0)

#define push_code(val) \
    do { \
        if (!buf_push(BUFFER, val)) { \
            return false; \
        } \
    } while (0)

#define add_code(val) \
    do { \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)

#define add_param(val) \
    do { \
        if (!buf_add(BUFFER, " ")) { \
            return false; \
        } \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)

// Add newline to buffer with error handling
#define endl() \
    do { \
        if (!buf_add_push(BUFFER, "\n")) { \
            return false; \
        } \
    } while (0)


bool add_int(int val){
    add_code("int@");
    if(!buf_add_int(buf, val)) return false;
    return true;
}

bool add_float(int val){
    add_code("float@");
    if(!buf_add_float(buf, val)) return false;
    return true;
}

bool add_string(char *str){
    add_code("string@");
    if(!buf_add_string(buf, str)) return false;
    return true;
}

bool add_read(char *var, Types type){
    add_code("READ");
    switch (type){
    case INT:
        add_param("int");
        break;
    case FLOAT:
        add_param("float");
        break;
    case STRING:
        add_param("string");
    }
    add_param(var);  
    endl();
    return true;
}


bool add_write(char *term){
    add_code("WRITE");
    add_param(term);
    endl();
    return true;
}

bool add_i2f(char *var, char *symb){
    add_code("INT2FLOAT");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}

bool add_f2i(char *var, char *symb){
    add_code("FLOAT2INT");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}


bool add_str_len(char *var, char *symb){
    add_code("STRLEN");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}

bool add_str_concat(char *var, char *symb1, char *symb2){
    add_code("CONCAT");
    add_param(var);
    add_param(symb1);
    add_param(symb2);
    endl();
    return true;
}

bool add_chr(char *var, char *symb){
    add_code("INT2CHAR");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}


bool generate_build_in_functions(){
    add_code(SUBSTRING);
    endl();
    add_code(STRCMP);
    endl();
    add_code(ORD);
    return true;
}

bool generate_header(){
    add_code("#.IFJcode24\n\n");
    return true;
}

bool code_generator(astNode *ast){
    if(ast == NULL) return true;
    switch (ast->type){
    case AST_NODE_WHILE:
        break;
    case AST_NODE_IFELSE:
        /* code */
        break;
    case AST_NODE_IF:
        /* code */
        break;
    
    case AST_NODE_ELSE:
        /* code */
        break;
    
    case AST_NODE_ASSIGN:
        /* code */
        break;
    
    case AST_NODE_EXPR:
        /* code */
        break;
    
    case AST_NODE_BINOP:
        /* code */
        break;
    
    case AST_NODE_LITERAL:
        /* code */
        break;
    
    case AST_NODE_VAR:
        /* code */
        break;

    case AST_NODE_DEFVAR:
        /* code */
        break;
    
    case AST_UNUSED:
        /* code */
        break;
    
    case AST_NODE_DEFFUNC:
        //LABAL $id
        add_code("LABAEL "); 
        add_code("$"); add_code(ast->nodeRep.defFuncNode.id);
        endl();

        add_code("PUSHFRAME"); endl();
        add_code("CREATEFRAME"); endl();
        
        //TODO create loc variabiles from %1, %2, ....
        //create flag for var definition
        buf_add_flag(BUFFER);
        //generate body
        code_generator(ast->nodeRep.defFuncNode.body);
        
        add_code("POPFRAME"); endl();
        add_code("RETURN"); endl();
        
        code_generator(ast->next);
        break;
    
    case AST_NODE_RETURN:
        /* code */
        break;
    
    case AST_NODE_ROOT:
        /* code */
        break;
    
    case AST_INVALID:
        /* code */
        break;
    
    }
}



int main(){
    buf_init(&BUFFER);
      
    add_float(3.14);
    add_string("Hello, World!");

    add_read("myVar", INT);
    add_write("myVar");

    add_i2f("floatVar", "intVar");
    add_f2i("intVar", "floatVar");

    add_str_len("length", "myString");
    add_str_concat("concatVar", "str1", "str2");
    add_chr("charVar", "intVar");
    fprint_buffer(BUFFER, stdout);
}