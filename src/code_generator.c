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
#define RETVAL "GF@retval"
#define NIL "nil@nil"
#define TMP1 "tmp_1"
#define TMP2 "tmp_2"
#define COMPILER true
#define USER false

Buffer_ll *BUFFER;


void inint_def_vars(Defined_vars *vars){
    vars->names = NULL;
    vars->num_of_vars = 0;
}

bool add_to_def_vars(Defined_vars *vars, char *name){
    char **tmp = realloc(vars->names, sizeof(char*)*(vars->num_of_vars+1));
    if(tmp == NULL) return false;

    vars->names = tmp;
    //vars->names[vars->num_of_vars] = name;

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
    vars->names = NULL;
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
        if (!buf_add(BUFFER, "%")) { \
            return false; \
        } \
        if (!buf_add_int(BUFFER, int_val)) { \
            return false; \
        } \
    } while (0)

#define TF_ARGS(val) \
    do { \
        if (!buf_add(BUFFER, "TF@")) { \
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
        if (!buf_add(BUFFER, RETVAL)) { \
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

// Help function for adding const int value
bool add_int(int val){
    add_code("int@");
    if(!buf_add_int(buf, val)) return false;
    return true;
}

// Help function for adding const float value
bool add_float(int val){
    add_code("float@");
    if(!buf_add_float(buf, val)) return false;
    return true;
}

// Help function for adding const string value
bool add_string(char *str){
    add_code("string@");
    if(!buf_add_string(buf, str)) return false;
    return true;
}

// Help function for adding null
bool add_null(){
    add_code("nil@nil");
    return true;
}

// Genarate built in funciton ifj.read
bool add_read(char *var, Types type){
    add_code("READ");
    add_param(var);  
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
    endl();
    return true;
}

// Genarate built in funciton ifj.write
bool add_write(char *term){
    add_code("WRITE");
    add_param(term);
    endl();
    return true;
}

// Genarate built in funciton ifj.i2f
bool add_i2f(char *var, char *symb){
    add_code("INT2FLOAT");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}

// Genarate built in funciton ifj.f2i
bool add_f2i(char *var, char *symb){
    add_code("FLOAT2INT");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}

// Genarate built in funciton ifj.length
bool add_str_len(char *var, char *symb){
    add_code("STRLEN");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}

// Genarate built in funciton ifj.concat
bool add_str_concat(char *var, char *symb1, char *symb2){
    add_code("CONCAT");
    add_param(var);
    add_param(symb1);
    add_param(symb2);
    endl();
    return true;
}

// Genarate built in funciton ifj.chr
bool add_chr(char *var, char *symb){
    add_code("INT2CHAR");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}

// Genarate bulit in functions, from macros
bool generate_build_in_functions(){
    add_code(SUBSTRING);
    endl();
    add_code(STRCMP);
    endl();
    add_code(ORD);
    return true;
}

bool generate_header(){
    add_code(HEADER); endl();
    if(!generate_build_in_functions()) return false;
    endl();
    return true;
}

bool generate_footer(){
    add_code("LABEL $$end\n"); endl();
    return true;
}

// Function for checking if var vas defined if no, will be defined
bool def_var(Defined_vars *TF_vars, char *var_tmp, bool defined_by_who){
    if(!is_in_def_vars(TF_vars, var_tmp)){
        add_code("DEFVAR "); 
        if(defined_by_who == COMPILER){
            TF_ARGS(var_tmp);
        }
        else{
            TF(var_tmp);
        }
        add_code("\n");
        if(!buf_push_after_flag(BUFFER)) return false;
        if(!add_to_def_vars(TF_vars, var_tmp)) return false;
    }   
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
    static int count = 0;   // count for function genarate_label 
    if(ast == NULL) return true;

    // Vars for storing generated lable
    char cond_label[52];
    char else_label[52];
    char end_label[52];
    DEBPRINT("------%d\n", ast->type);
    switch (ast->type){
        case AST_NODE_WHILE:
            count++;
            // TODO id_with_null
            // save genrated label names
            generate_label(cond_label, WHILE_COND, count);
            generate_label(end_label, WHILE_END, count);

            add_code("LABEL "); add_code(cond_label); endl();

            if(!code_generator(ast->nodeRep.whileNode.condition, TF_vars)) return false;

            if(ast->nodeRep.whileNode.withNull){
                def_var(TF_vars, ast->nodeRep.whileNode.id_without_null, USER);

                add_code("POPS "); TF(ast->nodeRep.whileNode.id_without_null); endl();

                //add_code("EQ "); TF_ARGS("tmp_bool"); space(); add_null(); space(); TF(ast->nodeRep.whileNode.id_without_null); endl();

                add_code("JUMPIFEQ "); add_code(end_label); space(); add_null(); space(); TF(ast->nodeRep.whileNode.id_without_null); endl();
            }
            else{
                add_code("POPS TF@tmp_bool"); endl();

                add_code("JUMPIFNEQ "); add_code(end_label); add_code(" TF@tmp_bool bool@true"); endl();
            }

            

            if(!code_generator(ast->nodeRep.whileNode.body, TF_vars)) return false;
            
            add_code("JUMP "); add_code(cond_label); endl();
            add_code("LABEL "); add_code(end_label); endl();

            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        case AST_NODE_IFELSE:
            count++;
            // save genrated label names
            generate_label(else_label, IF_ELSE, count);
            generate_label(end_label, IF_END, count);

            //if(ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null)
            if(!code_generator(ast->nodeRep.ifElseNode.condition, TF_vars)) return false;

            if(ast->nodeRep.ifElseNode.withNull){ 
                def_var(TF_vars, ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null, USER);
                add_code("POPS "); TF(ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null); endl();

                //add_code("EQ "); TF_ARGS("tmp_bool"); space(); add_null(); space(); TF(ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null); endl();

                add_code("JUMPIFEQ "); add_code(else_label); space(); add_null(); space(); TF(ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null); endl();
            }
            else{
                add_code("POPS TF@tmp_bool"); endl();
                add_code("JUMPIFNEQ "); add_code(else_label); add_code(" TF@tmp_bool bool@true"); endl();
            }


            if(!code_generator(ast->nodeRep.ifElseNode.ifPart, TF_vars)) return false;
            add_code("JUMP "); add_code(end_label); endl();

            add_code("LABEL "); add_code(else_label); endl();
            if(!code_generator(ast->nodeRep.ifElseNode.elsePart, TF_vars)) return false;
            add_code("LABEL "); add_code(end_label); endl();
            
            if(!code_generator(ast->next, TF_vars)) return false;

            break;
        case AST_NODE_IF:
            // TODO id_with_null
            if(!code_generator(ast->nodeRep.ifNode.body, TF_vars)) return false;
            break;
        
        case AST_NODE_ELSE:
            // TODO id_with_null
            if(!code_generator(ast->nodeRep.elseNode.body, TF_vars)) return false;
            break;
        
        case AST_NODE_ASSIGN:
            // Expresion evaluation
            if(!code_generator(ast->nodeRep.assignNode.expression, TF_vars)) return false;

            // Assign result to var
            add_code("POPS "); TF(ast->nodeRep.assignNode.id); endl();

            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_NODE_EXPR:
            // Expresion evaluation
            if(!code_generator(ast->nodeRep.exprNode.exprTree, TF_vars)) return false;
            if(ast->nodeRep.exprNode.exprTree != NULL){
                if(ast->nodeRep.exprNode.exprTree->type == AST_NODE_FUNC_CALL){
                    add_code("PUSHS "); GF(); endl();
                }
            }
            break;
        
        case AST_NODE_BINOP:
            // Check if left operand is function call, if so push the return value to data stack
            if(!code_generator(ast->nodeRep.binOpNode.left, TF_vars)) return false;
            if(ast->nodeRep.binOpNode.left != NULL && ast->nodeRep.binOpNode.left->type == AST_NODE_FUNC_CALL){
                add_code("PUSHS "); GF(); endl();
            }

            // Check if right operand is function call, if so push the return value to data stack
            if(!code_generator(ast->nodeRep.binOpNode.right, TF_vars)) return false;
            if(ast->nodeRep.binOpNode.right != NULL && ast->nodeRep.binOpNode.right->type == AST_NODE_FUNC_CALL){
                add_code("PUSHS "); GF(); endl();
            }

            // Handle binary operations based on the operator type in the AST node
            // Each case corresponds to a different binary operation
            switch (ast->nodeRep.binOpNode.op){
            case MULTIPLICATION:
                add_code("MULS"); endl();
                break;
            case DIVISION:
                // Check data type for integer or float division
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
                add_code("SUBS"); endl();
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
            case LOWER_OR_EQUAL:
                // Handle <= comparison by combining LTS and EQS
                if(!def_var(TF_vars, TMP1, COMPILER)) return false;
                if(!def_var(TF_vars, TMP2, COMPILER)) return false;
                add_code("POPS TF@tmp_2"); endl();
                add_code("POPS TF@tmp_1"); endl();

                add_code("PUSHS TF@tmp_1"); endl();
                add_code("PUSHS TF@tmp_2"); endl();
                add_code("LTS");endl();     // Check if tmp_2 < tmp_1

                add_code("PUSHS TF@tmp_1"); endl();
                add_code("PUSHS TF@tmp_2"); endl();
                add_code("EQS");endl();  // Check if tmp_2 == tmp_1
                
                add_code("ORS");endl(); // Combine LTS and EQS results                            
                break;
            case GREATER_OR_EQUAL:
                // Handle >= comparison by combining GTS and EQS
                if(!def_var(TF_vars, TMP1, COMPILER)) return false;
                if(!def_var(TF_vars, TMP2, COMPILER)) return false;
                add_code("POPS TF@tmp_2"); endl();
                add_code("POPS TF@tmp_1"); endl();

                add_code("PUSHS TF@tmp_1"); endl();
                add_code("PUSHS TF@tmp_2"); endl();
                add_code("GTS");endl();     // Check if tmp_2 > tmp_1

                add_code("PUSHS TF@tmp_1"); endl();
                add_code("PUSHS TF@tmp_2"); endl();
                add_code("EQS");endl();     // Check if tmp_2 == tmp_1

                add_code("ORS");endl();      // Combine GTS and EQS results
                break;
            default:
                //code
                break;
            }
            break;
        
        // Handle literal nodes in the AST
        // Pushes the literal value onto the stack in correct format based on it's type
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
            case null_:
                if(!add_null()) return false;
                break;
            default:
                break;
            }
            endl();
            break;
        

        // Handle variable nodes in the AST
        // Pushes the variable's value onto the stack based on its var id
        case AST_NODE_VAR:
            add_code("PUSHS ");
            TF(ast->nodeRep.varNode.id); endl();
            break;


        // Handle variable definition nodes in the AST
        // Defines a variable if not already defined and generates code to initialize it
        case AST_NODE_DEFVAR:
            ;
            char *name = ast->nodeRep.defVarNode.id;
            // Define the variable in the temporary frame if not yet defined
            if(!def_var(TF_vars, name, USER)) return false;

            // Evaluate assigning expression
            if(!code_generator(ast->nodeRep.exprNode.exprTree, TF_vars)) return false;

            //Asign the result after evaulation
            add_code("POPS "); TF(name); endl();

            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        

        // Hadnle expression which isn't assign to anything
        // Just evaluate the expresion
        case AST_UNUSED:
            if(!code_generator(ast->nodeRep.unusedNode.expr, TF_vars)) return false;
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        // Handle fucntion definition node in AST
        // Genrate lables, and body of function
        case AST_NODE_DEFFUNC:
            // Generating label, based on name of function
            add_code("LABEL "); 
            add_code("$"); add_code(ast->nodeRep.defFuncNode.id);
            endl();

            // Push old frame, unless it's the 'main' function
            if(strcmp(ast->nodeRep.defFuncNode.id, "main") != 0){
                add_code("PUSHFRAME"); endl();
            }
            add_code("CREATEFRAME"); endl();
            
            // TODO add bin operators, maybe
            add_code("DEFVAR TF@tmp_bool");endl();

            for(int i = 0; i < ast->nodeRep.defFuncNode.paramNum; i++){
                char *name_ = ast->nodeRep.defFuncNode.paramNames[i];
                if(!add_to_def_vars(TF_vars, name_)) return false;
                add_code("DEFVAR "); TF(name_); endl();

                add_code("MOVE "); TF(name_); space(); add_code("LF@"); PARAM(i); endl();
            }
            // ast->nodeRep.defFuncNode.paramNames;
            // create flag for var definition
            buf_add_flag(BUFFER);
            // generate body
            if(!code_generator(ast->nodeRep.defFuncNode.body, TF_vars)) return false;

            if(strcmp(ast->nodeRep.defFuncNode.id, "main") == 0){
                add_code("JUMP $$end"); endl();
            }
            else{
                add_code("POPFRAME"); endl();
                add_code("RETURN"); endl();
            }
            endl();
            delete_def_vars(TF_vars);
            //TODO if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_NODE_RETURN:
            if(!code_generator(ast->nodeRep.returnNode.returnExp, TF_vars)) return false;
            if(ast->nodeRep.returnNode.returnType != void_){
                add_code("POPS "); GF(); endl();
            }
            break;
        
        case AST_NODE_ROOT:
            return code_generator(ast->next, TF_vars);
            break;

        case AST_NODE_FUNC_CALL:
            if(ast->nodeRep.funcCallNode.builtin){
                if(strcmp(ast->nodeRep.funcCallNode.id, "readstr") == 0){
                    if(!add_read(RETVAL, STRING)) return false;
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "readi32") == 0){
                    if(!add_read(RETVAL, INT)) return false;
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "readf64") == 0){
                    if(!add_read(RETVAL, FLOAT)) return false;
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "write") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS "); GF(); endl();
                    if(!add_write(RETVAL)) return false;
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "i2f") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS "); GF(); endl();
                    if(!add_i2f(RETVAL, RETVAL)) return false;
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "f2i") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS "); GF(); endl();
                    if(!add_f2i(RETVAL, RETVAL)) return false;
                    goto code_generator_end;
                }
                 else if(strcmp(ast->nodeRep.funcCallNode.id, "string") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS "); GF(); endl();
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "length") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS "); GF(); endl();
                    if(!add_str_len(RETVAL, RETVAL)) return false;
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "concat") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS "); GF(); endl();
                    
                    char var_tmp[] = "%1";
                    // define var if it is not defined on begining
                    if(!def_var(TF_vars,var_tmp, COMPILER)) return false;

                    add_code("MOVE "); TF_ARGS(var_tmp); space(); GF(); endl();
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[1], TF_vars)) return false;

                    add_code("POPS "); GF(); endl();
                    add_code("CONCAT"); add_param(RETVAL); space(); TF_ARGS(var_tmp); add_param(RETVAL); endl();
                    goto code_generator_end;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "chr") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS "); GF(); endl();
                    if(!add_chr(RETVAL, RETVAL)) return false;
                    goto code_generator_end;
                }
            }
            for(int i = 0; i < ast->nodeRep.funcCallNode.paramNum; i++){
                char var_tmp[30];
                sprintf(var_tmp, "%%%d", i);
                // define var if it is not defined on begining
                if(!def_var(TF_vars, var_tmp, COMPILER)) return false;

                if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[i], TF_vars)) return false;
                add_code("POPS "); TF_ARGS(var_tmp); endl();
                // add_code("MOVE "); TF(var_tmp); space(); GF(); endl();
            }
            add_code("CALL $");
            if(ast->nodeRep.funcCallNode.builtin) add_code("$");        // adding second $, because builin functions have $$ before name
            add_code(ast->nodeRep.funcCallNode.id);endl();
            
        code_generator_end:
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_INVALID:
            return false;   //TODO just to run it till its fixed
            break;
    
    }
    return true;
}

bool generate_code(astNode *ast){
    if(!buf_init(&BUFFER)) return false;
    Defined_vars var_def;
    inint_def_vars(&var_def);
    if(!generate_header()) return false;

    ast = ast->next;
    while(ast != NULL){
        if(!code_generator(ast, &var_def)) return false;
        ast = ast->next;
    }
    if(!generate_footer()) return false;
    fprint_buffer(BUFFER, stdout);
    return true;
}
