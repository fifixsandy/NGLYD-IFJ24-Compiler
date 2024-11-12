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


void inint_def_vars(Defined_vars *vars){
    vars->names = NULL;
    vars->num_of_vars = 0;
}

bool add_to_def_vars(Defined_vars *vars, char *name){
    char **tmp = realloc(vars->names, sizeof(char*)*(vars->num_of_vars+1));
    if(tmp == NULL) return false;

    vars->names = tmp;
    vars->names[vars->num_of_vars] = name;

    vars->names[vars->num_of_vars] = malloc(strlen(name) + 1);
    if (vars->names[vars->num_of_vars] == NULL) {
        return false; 
    }

    strcpy(vars->names[vars->num_of_vars], name);
    vars->num_of_vars++;

    return true;
}

bool is_in_def_vars(Defined_vars *vars, char *name){
    for(int i = 0; i<vars->num_of_vars; i++){
        if(strcmp(vars->names[i], name) == 0) return true;
    }
    return false;
}

void delete_def_vars(Defined_vars *vars){
    for(int i = 0; i<vars->num_of_vars; i++){
        free(vars->names[i]);
    }
    free(vars->names);
    vars->num_of_vars = 0;
}

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

#define PARAM(int_val) \
    do { \
        if (!buf_add(BUFFER, "%%")) { \
            return false; \
        } \
        if (!buf_add_int(BUFFER, int_val)) { \
            return false; \
        } \
    } while (0)

#define LF(val) \
    do { \
        if (!buf_add(BUFFER, "LF@_")) { \
            return false; \
        } \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)

#define TF(val) \
    do { \
        if (!buf_add(BUFFER, "TF@_")) { \
            return false; \
        } \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)

#define GF() \
    do { \
        if (!buf_add(BUFFER, "GF@retval")) { \
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

#define space() \
    do { \
        if (!buf_add(BUFFER, " ")) { \
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

void generate_label(char *label, LABEL_TYPES type, int number){
    char tmp[15];
    switch(type){
        case WHILE_COND:
            strcpy(tmp, "while-cond");
            break;
        case WHILE_END:
            strcpy(tmp, "while-end");
            break;
        case IF_ELSE:
            strcpy(tmp, "if-else");
            break;
        case IF_END:
            strcpy(tmp, "if-end");
            break;
    }
    sprintf(label, "&%s-%d", tmp, number);
}

bool code_generator(astNode *ast, Defined_vars *TF_vars){
    static int count = 0;
    if(ast == NULL) return true;
    char cond_label[52];
    char else_label[52];
    char end_label[52];
    switch (ast->type){
        case AST_NODE_WHILE:
            count++;
            //TODO id_with_null
            //save genrated label names
            generate_label(cond_label, WHILE_COND, count);
            generate_label(end_label, WHILE_END, count);

            add_code("LABEL "); add_code(cond_label); endl();

            if(!code_generator(ast->nodeRep.ifElseNode.condition, TF_vars)) return false;
            add_code("POPS TF@tmp_bool"); endl();

            add_code("JUMPIFNEQ "); add_code(end_label); add_code(" TF@tmp_bool bool@true"); endl();

            if(!code_generator(ast->nodeRep.whileNode.body, TF_vars)) return false;
            
            add_code("JUMP "); add_code(cond_label); endl();
            add_code("LABEL "); add_code(end_label); endl();

            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        case AST_NODE_IFELSE:
            count++;
            //TODO id_with_null
            //save genrated label names
            generate_label(else_label, IF_ELSE, count);
            generate_label(end_label, IF_END, count);

            if(!code_generator(ast->nodeRep.ifElseNode.condition, TF_vars)) return false;
            add_code("POPS TF@tmp_bool"); endl();

            add_code("JUMPIFNEQ "); add_code(else_label); add_code(" TF@tmp_bool bool@true"); endl();
            if(!code_generator(ast->nodeRep.ifElseNode.ifPart, TF_vars)) return false;
            add_code("JUMP "); add_code(end_label); endl();

            add_code("LABEL "); add_code(else_label); endl();
            if(!code_generator(ast->nodeRep.ifElseNode.elsePart, TF_vars)) return false;
            add_code("LABEL "); add_code(end_label); endl();
            
            if(!code_generator(ast->next, TF_vars)) return false;

            break;
        case AST_NODE_IF:
            //TODO id_with_null
            if(!code_generator(ast->nodeRep.ifNode.body, TF_vars)) return false;
            break;
        
        case AST_NODE_ELSE:
            //TODO id_with_null
            if(!code_generator(ast->nodeRep.elseNode.body, TF_vars)) return false;
            break;
        
        case AST_NODE_ASSIGN:
            if(!code_generator(ast->nodeRep.assignNode.expression, TF_vars)) return false;
            add_code("POPS "); TF(ast->nodeRep.assignNode.id); endl();
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_NODE_EXPR:
            //add_code("CLEARS"); endl();
            if(!code_generator(ast->nodeRep.exprNode.exprTree, TF_vars)) return false;
            break;
        
        case AST_NODE_BINOP:
            //ast->nodeRep.binOpNode.
            if(!code_generator(ast->nodeRep.binOpNode.left, TF_vars)) return false;
            if(ast->nodeRep.binOpNode.left != NULL && ast->nodeRep.binOpNode.left->type == AST_NODE_FUNC_CALL){
                add_code("PUSHS "); GF(); endl();
            }
            if(!code_generator(ast->nodeRep.binOpNode.right, TF_vars)) return false;
            if(ast->nodeRep.binOpNode.right != NULL && ast->nodeRep.binOpNode.right->type == AST_NODE_FUNC_CALL){
                add_code("PUSHS "); GF(); endl();
            }
            symbol_number type = ast->nodeRep.binOpNode.op;
             //TODO LOWER_OR_EQUAL,         // 8
            //GREATER_OR_EQUAL,       // 9
            switch (type){
            case MULTIPLICATION:
                add_code("MULS"); endl();
                break;
            case DIVISION:
                if(ast->nodeRep.binOpNode.dataT == i32){
                    add_code("IDIVS"); endl();
                }
                else{
                    add_code("DIVS"); endl();
                }
                break;

            case ADDITION:
                add_code("ADDS"); endl();
                break;
            case SUBSTRACTION:
                add_code("SUBs"); endl();
                break;
            case EQUAL:
                add_code("EQS");endl();
                break;
            case NOT_EQUAL:
                add_code("EQS");endl();
                add_code("NOTS");endl();
                break;
            case LOWER:
                add_code("LTS");endl();
                break;
            case GREATER:
                add_code("GTS");endl();
                break;
            default:
                //code
                break;
            }
            break;
        
        case AST_NODE_LITERAL:
            add_code("PUSHS ");
            switch(ast->nodeRep.literalNode.dataT){
            case u8:
                if(!add_string(ast->nodeRep.literalNode.value.charData)) return false;
                break;
            case i32:
                if(!add_int(ast->nodeRep.literalNode.value.intData)) return false;
                break;
            case f64:
                if(!add_float(ast->nodeRep.literalNode.value.floatData)) return false;
                break;
            default:
                break;
            }
            endl();
            break;
        
        case AST_NODE_VAR:
            add_code("PUSHS ");
            TF(ast->nodeRep.varNode.id); endl();
            break;

        case AST_NODE_DEFVAR:
            ;
            char *name = ast->nodeRep.defVarNode.id;
            if(!is_in_def_vars(TF_vars, name)){
                TF(name);
                if(!buf_push_after_flag(BUFFER)) return false;
                if(!add_to_def_vars(TF_vars, name)) return false;
            }

            if(!code_generator(ast->nodeRep.exprNode.exprTree, TF_vars)) return false;
            add_code("POPS "); TF(name); endl();
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_UNUSED:
            if(!code_generator(ast->nodeRep.unusedNode.expr, TF_vars)) return false;
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_NODE_DEFFUNC:
            //LABAL $id
            add_code("LABAEL "); 
            add_code("$"); add_code(ast->nodeRep.defFuncNode.id);
            endl();

            add_code("PUSHFRAME"); endl();
            add_code("CREATEFRAME"); endl();
            
            //TODO add bin operators, maybe
            add_code("DEFVAR TF@tmp_bool");endl();

            for(int i = 0; i < ast->nodeRep.defFuncNode.paramNum; i++){
                char *name = ast->nodeRep.defFuncNode.paramNames[i];
                if(!add_to_def_vars(TF_vars, name)) return false;
                add_code("DEFVAR "); TF(name); endl();

                add_code("MOVE "); TF(name); space(); PARAM(i); endl();
            }
            //ast->nodeRep.defFuncNode.paramNames;
            //create flag for var definition
            buf_add_flag(BUFFER);
            //generate body
            if(!code_generator(ast->nodeRep.defFuncNode.body, TF_vars)) return false;
            
            
            
            delete_def_vars(TF_vars);
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_NODE_RETURN:
            if(!code_generator(ast->nodeRep.returnNode.returnExp, TF_vars)) return false;
            add_code("POPS "); GF(); endl();
            add_code("POPFRAME"); endl();
            add_code("RETURN"); endl();
            break;
        
        case AST_NODE_ROOT:
            if(!code_generator(ast->next, TF_vars)) return false;
            break;

        case AST_NODE_FUNC_CALL:
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_INVALID:
            /* code */
            break;
    
    }
    return true;
}

bool generate_code(astNode *ast){
    if(!buf_init(&BUFFER)) return false;
    Defined_vars var_def;
    inint_def_vars(&var_def);
    if(!code_generator(ast, &var_def)) return false;
    fprint_buffer(BUFFER, stdout);
    return true;
}
