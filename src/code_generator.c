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

