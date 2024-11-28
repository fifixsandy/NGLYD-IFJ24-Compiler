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

#define BUFFER buf          // Global buffer for code generation
#define RETVAL "GF@retval"  // Global variable name for storing the return value of functions in generated code
#define NIL "nil@nil"       // Represents a nil value in the generated code
#define TMP1 "_tmp_1"       // Temporary variable 1 used for generating bin operator ">=" and "<="
#define TMP2 "_tmp_2"       // Temporary variable 1 used for generating bin operator ">=" and "<="
#define COMPILER true       // Flag to indicate compiler-defined variables
#define USER false          // Flag to indicate user-defined variables

Buffer_ll *BUFFER;  // Pointer to the buffer structure for code generation.

/**
 * @brief Adds a string to the accumulator string.
 * 
 * @param val String to be added to the accumulator string.
 * @return `false` if the operation fails; otherwise, execution continues.
 */
#define add_code(val) \
    do { \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)



/**
 * @brief Adds a string to accumlator string in buffer and insert space charatceter before that string.
 *
 * @param val String to be added
 * @return `false` if the operation fails; otherwise, execution continues.
 *
 */
#define add_param(val) \
    do { \
        if (!buf_add(BUFFER, " ")) { \
            return false; \
        } \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)


/**
 * @brief Adds a integer to accumlator string in buffer and insert '%' before that integer.
 *
 * @param int_val The interger to be added
 * @return `false` if the operation fails; otherwise, execution continues.
 *
 */
#define PARAM(int_val) \
    do { \
        if (!buf_add(BUFFER, "%")) { \
            return false; \
        } \
        if (!buf_add_int(BUFFER, int_val)) { \
            return false; \
        } \
    } while (0)


/**
 * @brief Adds a string to accumlator string in buffer and insert 'TF@' before that string.
 *
 * @param val String to be added
 * @return `false` if the operation fails; otherwise, execution continues.
 *
 */
#define TF_ARGS(val) \
    do { \
        if (!buf_add(BUFFER, "TF@")) { \
            return false; \
        } \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)


/**
 * @brief Adds a string to accumlator string in buffer and insert 'TF@_' before that string.
 *
 * @param val String to be added
 * @return `false` if the operation fails; otherwise, execution continues.
 *
 */
#define TF(val) \
    do { \
        if (!buf_add(BUFFER, "TF@_")) { \
            return false; \
        } \
        if (!buf_add(BUFFER, val)) { \
            return false; \
        } \
    } while (0)


/**
 * @brief Adds a newline to accumaltor and pushes accumulator stirng it into the linked list.
 *
 * @return `false` if the operation fails; otherwise, execution continues.
 *
 */
#define endl() \
    do { \
        if (!buf_add_push(BUFFER, "\n")) { \
            return false; \
        } \
    } while (0)


/**
 * @brief Adds a space charatceter to accumulator string.
 *
 * @return `false` if the operation fails; otherwise, execution continues.
 *
 */
#define space() \
    do { \
        if (!buf_add(BUFFER, " ")) { \
            return false; \
        } \
    } while (0)


/**
 * @brief Initializes a Defined_vars structure.
 *
 * @param vars Pointer to a Defined_vars structure to be initialized.
 */
void inint_def_vars(Defined_vars *vars){
    vars->names = NULL;
    vars->num_of_vars = 0;
}


/**
 * @brief Adds a new variable name to the Defined_vars structure.
 *
 *
 * @param vars Pointer to the Defined_vars structure.
 * @param name Pointer to the string representing the variable name.
 * @return true if the variable was successfully added, false on failure.
 */
bool add_to_def_vars(Defined_vars *vars, char *name){
    char **tmp = realloc(vars->names, sizeof(char*)*(vars->num_of_vars+1));
    if(tmp == NULL) return false;

    vars->names = tmp;

    vars->names[vars->num_of_vars] = malloc(strlen(name) + 1);
    if (vars->names[vars->num_of_vars] == NULL) {
        return false; 
    }

    strcpy(vars->names[vars->num_of_vars], name);
    vars->num_of_vars++;

    return true;
}


/**
 * @brief Adds a new variable name to the Defined_vars structure.
 *
 *
 * @param vars Pointer to the Defined_vars structure.
 * @param name Pointer to the string representing the variable name.
 * @return true if the variable was successfully added, false on failure.
 */
bool is_in_def_vars(Defined_vars *vars, char *name){
    for(int i = 0; i<vars->num_of_vars; i++){
        if(strcmp(vars->names[i], name) == 0) return true;
    }
    return false;
}


/**
 * @brief Deletes all variable names and frees memory in the Defined_vars structure.
 *
 * @param vars Pointer to the Defined_vars structure to be deleted.
 */
void delete_def_vars(Defined_vars *vars){
    for(int i = 0; i<vars->num_of_vars; i++){
        free(vars->names[i]);
    }
    free(vars->names);
    vars->names = NULL;
    vars->num_of_vars = 0;
}

/**
 * @brief Add formatted int value to the code buffer.
 * @param val The int value to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool add_int(int val){
    add_code("int@");
    if(!buf_add_int(buf, val)) return false;
    return true;
}

/**
 * @brief Add formatted float value to the code buffer.
 * @param val The float value to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool add_float(double val){
    add_code("float@");
    if(!buf_add_float(buf, val)) return false;
    return true;
}

/**
 * @brief Add formatted string value to the code buffer.
 * @param str The string value to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool add_string(char *str){
    add_code("string@");
    if(!buf_add_string(buf, str)) return false;
    return true;
}

/**
 * @brief Helper function to add a null value in a specific format(nil@nil)
 * @return true Always returns true.
 */
bool add_null(){
    add_code("nil@nil");
    return true;
}

/**
 * @brief Add READ instruction to the code buffer.
 * @param var The variable which input will be assigned to.
 * @param type Type of variable being read (INT, FLOAT, STRING).
 * @return true if the operation was successful, false otherwise.
 */
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


/**
 * @brief Add WRITE instruction to the code buffer.
 * @param term The term (int, float, string, or variable) to be printed.
 * @return true if the operation was successful, false otherwise.
 */
bool add_write(char *term){
    add_code("WRITE");
    add_param(term);
    endl();
    return true;
}


/**
 * @brief Add INT2FLOAT instruction to the code buffer.
 * @param var The variable where the conversion result will be assigned.
 * @param symb The symbol (string or variable) from which the conversion is performed.
 * @return true if the operation was successful, false otherwise.
 */
bool add_i2f(char *var, char *symb){
    add_code("INT2FLOAT");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}


/**
 * @brief Add FLOAT2INT instruction to the code buffer.
 * @param var The variable where the conversion result will be assigned.
 * @param symb The symbol (string or variable) from which the conversion is performed.
 * @return true if the operation was successful, false otherwise.
 */
bool add_f2i(char *var, char *symb){
    add_code("FLOAT2INT");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}


/**
 * @brief Add STRLEN instruction to the code buffer.
 * @param var The variable where the result will be assigned.
 * @param symb The symbol (string or variable) whose length will be calculated.
 * @return true if the operation was successful, false otherwise.
 */
bool add_str_len(char *var, char *symb){
    add_code("STRLEN");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}


/**
 * @brief Add CONCAT instruction to the code buffer.
 * @param var The variable where the concatenation result will be assigned.
 * @param symb1 The first symbol to concatenate.
 * @param symb2 The second symbol to concatenate.
 * @return true if the operation was successful, false otherwise.
 */
bool add_str_concat(char *var, char *symb1, char *symb2){
    add_code("CONCAT");
    add_param(var);
    add_param(symb1);
    add_param(symb2);
    endl();
    return true;
}


/**
 * @brief Add INT2CHAR instruction to the code buffer.
 * @param var The variable where the character result will be assigned.
 * @param symb The symbol (integer or variable) to be converted to a character.
 * @return true if the operation was successful, false otherwise.
 */
bool add_chr(char *var, char *symb){
    add_code("INT2CHAR");
    add_param(var);
    add_param(symb);
    endl();
    return true;
}


/**
 * @brief Generate built-in functions in the code buffer.
 * @return true if the operation was successful, false otherwise.
 */
bool generate_build_in_functions(){
    add_code(SUBSTRING);
    endl();
    add_code(STRCMP);
    endl();
    add_code(ORD);
    return true;
}

/**
 * @brief Generate the header for the code.
 * @return true if the operation was successful, false otherwise.
 */
bool generate_header(){
    add_code(HEADER); endl();
    if(!generate_build_in_functions()) return false;
    endl();
    return true;
}

/**
 * @brief Generate the footer for the code.
 * @return true if the operation was successful, false otherwise.
 */
bool generate_footer(){
    add_code("LABEL $$end\n"); endl();
    return true;
}


/**
 * @brief Checks if a variable is defined and defines it if not already defined.
 *
 * @param TF_vars Pointer to the Defined_vars structure containing the list of defined variables.
 * @param var_tmp The name of the variable to be checked or defined.
 * @param defined_by_who A boolean flag indicating who is defining the variable (e.g., compiler).
 * 
 * @return true if the operation was successful, false otherwise.
 */
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


/**
 * @brief Generates a label based on the specified type and number.
 *
 * @param label A pointer to a string buffer where the generated label will be stored.
 * @param type The type of the label to be generated.
 * @param number A unique number that will be appended to the label to differentiate it.
 *
 */
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


/**
 * @brief Generates output code from an AST.
 *
 * This function recursively traverses the provided AST and generates corresponding code.
 *  
 * @param ast Pointer to the root of the AST to be processed.
 * @param TF_vars Pointer to the structure that holds the defined variables.
 *
 * @return true if the code generation was successful, false if an error occurred.
 */
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
            count++;    // Increment count for generating unique labels for the loop

            // Save genrated label names
            generate_label(cond_label, WHILE_COND, count);
            generate_label(end_label, WHILE_END, count);
        
            add_code("LABEL "); add_code(cond_label); endl(); 

            // Recursively generate code for the while loop condition
            if(!code_generator(ast->nodeRep.whileNode.condition, TF_vars)) return false;

             // Check if the loop uses a variable with 'null' handling
            if(ast->nodeRep.whileNode.withNull){
                // Define the variable in the temporary frame
                def_var(TF_vars, ast->nodeRep.whileNode.id_without_null, USER);

                add_code("POPS "); TF(ast->nodeRep.whileNode.id_without_null); endl();
                add_code("JUMPIFEQ "); add_code(end_label); space(); add_null(); space(); TF(ast->nodeRep.whileNode.id_without_null); endl();
            }
            else{
                add_code("POPS TF@tmp_bool"); endl();
                add_code("JUMPIFNEQ "); add_code(end_label); add_code(" TF@tmp_bool bool@true"); endl();
            }

            
            // Recursively generate code for the body of the while loop
            if(!code_generator(ast->nodeRep.whileNode.body, TF_vars)) return false;
            
            
            add_code("JUMP "); add_code(cond_label); endl();
            add_code("LABEL "); add_code(end_label); endl();

            // Continue with the next part of the program
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        case AST_NODE_IFELSE:
            count++;
            // Save genrated label names
            generate_label(else_label, IF_ELSE, count);
            generate_label(end_label, IF_END, count);

            // Generate code for the condition expression
            if(!code_generator(ast->nodeRep.ifElseNode.condition, TF_vars)) return false;

            // Check if the if-else condition involves 'null' handling
            if(ast->nodeRep.ifElseNode.withNull){ 
                // Define the variable in the temporary frame
                def_var(TF_vars, ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null, USER);

                add_code("POPS "); TF(ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null); endl();
                add_code("JUMPIFEQ "); add_code(else_label); space(); add_null(); space(); TF(ast->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null); endl();
            }
            else{
                add_code("POPS TF@tmp_bool"); endl();
                add_code("JUMPIFNEQ "); add_code(else_label); add_code(" TF@tmp_bool bool@true"); endl();
            }

            // Generate code for the 'if' part of the statement
            if(!code_generator(ast->nodeRep.ifElseNode.ifPart, TF_vars)) return false;
            add_code("JUMP "); add_code(end_label); endl();
            add_code("LABEL "); add_code(else_label); endl();

             // Generate code for the 'else' part of the statement
            if(!code_generator(ast->nodeRep.ifElseNode.elsePart, TF_vars)) return false;
            add_code("LABEL "); add_code(end_label); endl();
            
            // Continue with the next part of the program
            if(!code_generator(ast->next, TF_vars)) return false;

            break;
        case AST_NODE_IF:
            // Procecess the if body
            if(!code_generator(ast->nodeRep.ifNode.body, TF_vars)) return false;
            break;
        
        case AST_NODE_ELSE:
            // Procecess the else body
            if(!code_generator(ast->nodeRep.elseNode.body, TF_vars)) return false;
            break;
        
        case AST_NODE_ASSIGN:
            // Expresion evaluation
            if(!code_generator(ast->nodeRep.assignNode.expression, TF_vars)) return false;

            // Assign result to var
            add_code("POPS "); TF(ast->nodeRep.assignNode.id); endl();

            // Continue with the next part of the program
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_NODE_EXPR:
            // Expresion evaluation
            if(!code_generator(ast->nodeRep.exprNode.exprTree, TF_vars)) return false;

            if(ast->nodeRep.exprNode.exprTree != NULL){
                // Check if the expression is a function call
                if(ast->nodeRep.exprNode.exprTree->type == AST_NODE_FUNC_CALL){
                    add_code("PUSHS"); add_param(RETVAL); endl();
                }
            }
            break;
        
        case AST_NODE_BINOP:
            // Check if left operand is function call, if so push the return value to data stack
            if(!code_generator(ast->nodeRep.binOpNode.left, TF_vars)) return false;
            if(ast->nodeRep.binOpNode.left != NULL && ast->nodeRep.binOpNode.left->type == AST_NODE_FUNC_CALL){
                add_code("PUSHS"); add_param(RETVAL); endl();
            }

            // Check if right operand is function call, if so push the return value to data stack
            if(!code_generator(ast->nodeRep.binOpNode.right, TF_vars)) return false;
            if(ast->nodeRep.binOpNode.right != NULL && ast->nodeRep.binOpNode.right->type == AST_NODE_FUNC_CALL){
                add_code("PUSHS"); add_param(RETVAL); endl();
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
                add_code("POPS "); TF_ARGS(TMP2); endl();
                add_code("POPS "); TF_ARGS(TMP1); endl();

                add_code("PUSHS "); TF_ARGS(TMP1); endl();
                add_code("PUSHS "); TF_ARGS(TMP2); endl();
                add_code("LTS");endl();     // Check if tmp_2 < tmp_1

                add_code("PUSHS "); TF_ARGS(TMP1); endl();
                add_code("PUSHS "); TF_ARGS(TMP2); endl();
                add_code("EQS");endl();  // Check if tmp_2 == tmp_1
                
                add_code("ORS");endl(); // Combine LTS and EQS results                            
                break;
            case GREATER_OR_EQUAL:
                // Handle >= comparison by combining GTS and EQS
                if(!def_var(TF_vars, TMP1, COMPILER)) return false;
                if(!def_var(TF_vars, TMP2, COMPILER)) return false;
                add_code("POPS "); TF_ARGS(TMP2); endl();
                add_code("POPS "); TF_ARGS(TMP1); endl();

                add_code("PUSHS "); TF_ARGS(TMP1); endl();
                add_code("PUSHS "); TF_ARGS(TMP2); endl();
                add_code("GTS");endl();     // Check if tmp_2 > tmp_1

                add_code("PUSHS "); TF_ARGS(TMP1); endl();
                add_code("PUSHS "); TF_ARGS(TMP2); endl();
                add_code("EQS");endl();     // Check if tmp_2 == tmp_1

                add_code("ORS");endl();      // Combine GTS and EQS results
                break;
            default:
                //code
                break;
            }
            break;
        
        case AST_NODE_LITERAL:
            // Handle literal nodes in the AST
            // Pushes the literal value onto the stack in correct format based on it's type
            add_code("PUSHS ");
            switch(ast->nodeRep.literalNode.dataT){
            case u8:
            case string:
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
        

        case AST_NODE_VAR:
            // Handle variable nodes in the AST
            // Pushes the variable's value onto the stack based on its var id
            add_code("PUSHS ");
            TF(ast->nodeRep.varNode.id); endl();
            break;


        case AST_NODE_DEFVAR:
            // Handle variable definition nodes in the AST
            // Defines a variable if not already defined and generates code to initialize it
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
        

        case AST_UNUSED:
            // Hadnle expression which isn't assign to anything
            // Just evaluate the expresion
            if(!code_generator(ast->nodeRep.unusedNode.expr, TF_vars)) return false;
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_NODE_DEFFUNC:
            // Handle fucntion definition node in AST

            // Generating label, based on name of function
            add_code("LABEL "); add_code("$"); add_code(ast->nodeRep.defFuncNode.id); endl();

            // Push old frame, unless it's the 'main' function
            if(strcmp(ast->nodeRep.defFuncNode.id, "main") != 0){
                add_code("PUSHFRAME"); endl();
            }

            add_code("CREATEFRAME"); endl();
            add_code("DEFVAR TF@tmp_bool");endl();

            // Loop over the parameters of the function and define them in the temporary frame (TF).
            for(int i = 0; i < ast->nodeRep.defFuncNode.paramNum; i++){
                char *name_ = ast->nodeRep.defFuncNode.paramNames[i];
                if(!add_to_def_vars(TF_vars, name_)) return false;

                add_code("DEFVAR "); TF(name_); endl();
                add_code("MOVE "); TF(name_); space(); add_code("LF@"); PARAM(i); endl();
            }

            // Create flag for var definition
            buf_add_flag(BUFFER);

            // Generate body
            if(!code_generator(ast->nodeRep.defFuncNode.body, TF_vars)) return false;

            // Special case for the 'main' function: it jumps to an 'end' label
            if(strcmp(ast->nodeRep.defFuncNode.id, "main") == 0){
                add_code("JUMP $$end"); endl();
            }
            // For other functions pop the frame and return from the function
            else if(ast->nodeRep.defFuncNode.returnType == void_){
                add_code("POPFRAME"); endl();
                add_code("RETURN"); endl();
            }

            endl();

            // Clean up the defined variables
            delete_def_vars(TF_vars);
            break;
        
        case AST_NODE_RETURN:
            // Handle return node in AST

            // Generate code for the return expression, if present
            if(!code_generator(ast->nodeRep.returnNode.returnExp, TF_vars)) return false;

            // If the return type is not 'void', pop the return value into RETVAL
            if(ast->nodeRep.returnNode.returnType != void_){
                add_code("POPS"); add_param(RETVAL); endl();
            }

            // If this return is in the 'main' function, jump to end of the program
            if(ast->nodeRep.returnNode.inMain){
                add_code("JUMP $$end"); endl();
            }
             // For other functions, pop the frame and return
            else{
                add_code("POPFRAME"); endl();
                add_code("RETURN"); endl();
            }
            break;
        
        case AST_NODE_ROOT:
            // Continue generating code for the next node in the AST.
            return code_generator(ast->next, TF_vars);
            break;

        case AST_NODE_FUNC_CALL:
            // Handle function call node in AST


            if(    ast->nodeRep.funcCallNode.builtin 
                && (strcmp(ast->nodeRep.funcCallNode.id, "substring") != 0)
                && (strcmp(ast->nodeRep.funcCallNode.id, "strcmp") != 0)
                && (strcmp(ast->nodeRep.funcCallNode.id, "ord") != 0)
            ){
                // Handle built-in functions
                // Each built-in function is processed differently depending on its type
                if(strcmp(ast->nodeRep.funcCallNode.id, "readstr") == 0){
                    if(!add_read(RETVAL, STRING)) return false;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "readi32") == 0){
                    if(!add_read(RETVAL, INT)) return false;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "readf64") == 0){
                    if(!add_read(RETVAL, FLOAT)) return false;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "write") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS"); add_param(RETVAL); endl();
                    if(!add_write(RETVAL)) return false;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "i2f") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS"); add_param(RETVAL); endl();
                    if(!add_i2f(RETVAL, RETVAL)) return false;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "f2i") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS"); add_param(RETVAL); endl();
                    if(!add_f2i(RETVAL, RETVAL)) return false;
                }
                 else if(strcmp(ast->nodeRep.funcCallNode.id, "string") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS"); add_param(RETVAL); endl();
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "length") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS"); add_param(RETVAL); endl();
                    if(!add_str_len(RETVAL, RETVAL)) return false;
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "concat") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[1], TF_vars)) return false;
                    
                    char var_tmp[] = "_concat_tmp";
                    // define var if it is not defined on begining
                    if(!def_var(TF_vars,var_tmp, COMPILER)) return false;

                    add_code("POPS"); add_param(RETVAL); endl();
                    add_code("POPS "); TF_ARGS(var_tmp); endl();

                    add_code("CONCAT"); add_param(RETVAL); space(); TF_ARGS(var_tmp); add_param(RETVAL); endl();
                }
                else if(strcmp(ast->nodeRep.funcCallNode.id, "chr") == 0){
                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[0], TF_vars)) return false;
                    add_code("POPS"); add_param(RETVAL); endl();
                    if(!add_chr(RETVAL, RETVAL)) return false;
                }
            }   
            else{
                // For other functions generate code for each parameter
                for(int i = 0; i < ast->nodeRep.funcCallNode.paramNum; i++){
                    char var_tmp[30];
                    sprintf(var_tmp, "%%%d", i);
                    // define var if it is not defined on begining
                    if(!def_var(TF_vars, var_tmp, COMPILER)) return false;

                    if(!code_generator(ast->nodeRep.funcCallNode.paramExpr[i], TF_vars)) return false;
                    add_code("POPS "); TF_ARGS(var_tmp); endl();
                }

                // Call the function with the specified parameters
                add_code("CALL $");
                if(ast->nodeRep.funcCallNode.builtin) add_code("$");        // adding second $, because builin functions have $$ before name
                add_code(ast->nodeRep.funcCallNode.id);endl();
            }
            // Continue processing the next node in the AST
            if(!code_generator(ast->next, TF_vars)) return false;
            break;
        
        case AST_INVALID:
            // Handle invalid AST node
            return false;   
            break;
    
    }
    return true;
}

bool generate_code(astNode *ast){
    // Initialize the buffer where generated code will be stored
    if(!buf_init(&BUFFER)) return false;

    // Initialize the structure to hold the defined variables
    Defined_vars var_def;
    inint_def_vars(&var_def);


    if(!generate_header()) return false;
    
    // Move to the first actual node in the AST, noot the root
    ast = ast->next;

    // Iterate through each AST function nodes
    while(ast != NULL){
        if(!code_generator(ast, &var_def)) return false;
        ast = ast->next;
    }

    if(!generate_footer()) return false;
    fprint_buffer(BUFFER, stdout);  // Output the generated code from the buffer
    return true;
}

/* EOF code_generator.c */