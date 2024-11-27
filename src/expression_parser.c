/**
 *         Implementace překladače imperativního jazyka IFJ24.
 * 
 * @file   expression_parser.c
 * 
 * @brief  Implementation of a recursive parser for the IFJ24 project. 
 *         This module is responsible for parsing expressions using precedence parsing techniques.
 *         It manages an expression stack and generates an Abstract Syntax Tree (AST).
 *         The module provides functions for stack manipulation, syntax validation, and semantic
 *         checks to ensure proper type handling and compliance with the IFJ24 (based on ZIG) language rules.
 * 
 *         Functionalities:
 *                      - Defines a precedence table for expression parsing.
 *                      - Implements stack-based operations such as push, pop, and top-terminal retrieval.
 *                      - Handles the evaluation of tokens and reduction rules for expression tree construction.
 *                      - Includes semantic checks for type compatibility and retyping as needed.
 *                      - Manages memory efficiently by freeing stack resources and AST nodes.
 * 
 *         During evaluation, if a variable identifier or function identifier is encountered, control is handed to the parser
 *         to verify whether the identifier is defined.
 *         In case of errors, the `ERROR(...)` macro (defined in error.h) is called, and the program terminates with the appropriate error code.
 * 
 * @see    parser.h, ast.h, error.h
 * 
 * @author xfignam00 Matúš Fignár
 * @date   21.11.2024
 */

#include <stdio.h>
#include "expression_parser.h"


/**************************************************************************************************************************************************/
/*                                                        Precedence table                                                                        */


precedence precedence_table[14][14] = {
//    *   /   +   -   ==  !=  <   >   <=  >=  (   )   i   $
    { GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, LS, GR, LS, GR},  // *    
    { GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, LS, GR, LS, GR},  // /    
    { LS, LS, GR, GR, GR, GR, GR, GR, GR, GR, LS, GR, LS, GR},  // +    
    { LS, LS, GR, GR, GR, GR, GR, GR, GR, GR, LS, GR, LS, GR},  // -    
    { LS, LS, LS, LS, ND, ND, ND, ND, ND, ND, LS, GR, LS, GR},  // ==   
    { LS, LS, LS, LS, ND, ND, ND, ND, ND, ND, LS, GR, LS, GR},  // !=   
    { LS, LS, LS, LS, ND, ND, ND, ND, ND, ND, LS, GR, LS, GR},  // <    
    { LS, LS, LS, LS, ND, ND, ND, ND, ND, ND, LS, GR, LS, GR},  // >    
    { LS, LS, LS, LS, ND, ND, ND, ND, ND, ND, LS, GR, LS, GR},  // <=   
    { LS, LS, LS, LS, ND, ND, ND, ND, ND, ND, LS, GR, LS, GR},  // >=   
    { LS, LS, LS, LS, LS, LS, LS, LS, LS, LS, LS, EQ, LS, ND},  // (    
    { GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, ND, GR, ND, GR},  // )    
    { GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, ND, GR, ND, GR},  // i    
    { LS, LS, LS, LS, LS, LS, LS, LS, LS, LS, LS, ND, LS, ND},  // $    
};

/*
                    |       |
 -> token/input     |       V 
                    |     stack
*/


/**************************************************************************************************************************************************/
/*                                                        Expresion stack functions                                                               */




/**
* @brief Creates and initializes stack for expressions
*
* @return
*/
exp_stack *exp_stack_create(){
    exp_stack *Stack = (exp_stack *)malloc(sizeof(exp_stack));
    if(Stack == NULL){
        ERROR(ERR_INTERNAL, "Failed to allocate memory for stack.");
    }
    Stack->top = NULL;
    Stack->count = 0;
    return Stack;
}


/** 
* @brief "Pushes a new item onto the expression stack
* 
* @param estack pointer to stack
* @param node pointer to node that is added on stack
* @param op opearnd or type of expression character
*/
void exp_stack_push(exp_stack *estack, astNode *node, symbol_number op, control_items *control){
    stack_item *new_item = (stack_item *)malloc(sizeof(stack_item));
    if(new_item == NULL){
        ERROR(ERR_INTERNAL, "Failed to allocated memory for stack element"); 
    }
    new_item->control = control;
    new_item->node = node;
    new_item->expr = op;
    new_item->next = estack->top;
    estack->top = new_item;
    estack->count++;  
}




/**
 * @brief Pop top AST node from expression stack 
 * 
 * @param estack Pointer to stack  
 */
astNode *exp_stack_pop(exp_stack *estack, bool control_needed){
    if (estack->top == NULL){
        return NULL;
    }
    astNode *top_node = estack->top->node;
    stack_item *top_to_delete = estack->top;
    estack->top = estack->top->next;
    if(!control_needed){
        free(top_to_delete->control);
    }
    free(top_to_delete);
    estack->count--;
    return top_node;
}

/**
 * @brief funcion to find terminal that is on the top of stack
 * 
 * @return Symbol number of the terminal closest to the top of the stack.
 */
symbol_number exp_stack_top_term_symb(exp_stack *estack){
    if(estack->top->expr == NO_TERMINAL){
        if(estack->top->next->expr == NO_TERMINAL){
            ERROR(ERR_SYNTAX, ("Expected operation between operands")); 
        }
        return estack->top->next->expr;
    }
    else{
        return estack->top->expr;
    }
}


/**
 * @brief Frees all memory allocated for the stack and its contents
 */
void exp_stack_free_stack(exp_stack *estack){
    stack_item *curr = estack->top;
    while(curr){
        stack_item *next_item = curr->next;
        freeASTNode(curr->node);
        free(curr->control);
        free(curr);
        curr = next_item;
    }
    free(estack);
}


/**
 * @brief Search for left bracket in stack.
 * 
 * @return True if in the stack is left bracket; False if left bracket isn't in the stack
 */
bool exp_stack_find_lbr(exp_stack *estack){
    stack_item *estack_item = estack->top;
    while(estack_item){
        
        if(estack_item->expr == LBR){
            return true;
        }
        estack_item = estack_item->next;
    }
    return false;
}



/**************************************************************************************************************************************************/
/*                                                        Expresion parser functions                                                              */



/**
* @brief A funcion that calls helper funcions to build an expression tree
*
* @return True if build was successful; False if build wasn't successful
*/
bool expression(astNode *expr_node){
    DEBPRINT("entered expression func\n");
    exp_stack *estack = exp_stack_create();

    exp_stack_push(estack, NULL, STOP, NULL);    //push stop symbol on top of the stack
    DEBPRINT("pushed stop sign to stack\n");

    if(process_expr(estack)){
        
        control_items *expr_items = estack->top->control;
        astNode *final_exp = exp_stack_pop(estack, true);
        
        createExpressionNode(expr_node, expr_items->type, final_exp, expr_items->is_nullable, expr_items->known_during_compile); 
        
        free(expr_items);
        exp_stack_free_stack(estack);
        
        return true;
    }
    
    exp_stack_free_stack(estack);
    return false;
}

bool process_expr(exp_stack *estack){
    astNode *curr_node = createAstNode();
    

    control_items *control =(control_items *)malloc(sizeof(struct control_items));
    if(control == NULL){
        ERROR(ERR_INTERNAL, "Failed to allocate memory for control struct"); //TODO internal Error
    }

    // initialization of empty controls
    control->is_convertable = false;
    control->is_nullable = false;
    control->type = void_;
    control->known_during_compile = false;

    symbol_number curr_symb = evaluate_given_token(estack, currentToken, curr_node, control);
        
    int evaluate = shift(estack, curr_node, control, curr_symb);
    
    if(evaluate == 0){
        
        GT
        
        return process_expr(estack);
    }

    else if(evaluate == 1){
        if(estack->top->expr != NO_TERMINAL){
            ERROR(ERR_SYNTAX, "Invalid expression\n");
        }
        
        return true;
    }

    else{
        return false;
    }
    return false;
    
}

/**
* @brief A function that evaluates the precedence of a token with the precedence of the highest terminal symbol and applies a rule according to precedence parsing.
*
* @param estack A pointer to an expression stack
* @param pointer to a node of an expression tree
* 
* @return true if the expression has been fully processed; false if the expression has not been fully processed.
*/
int shift(exp_stack *estack, astNode *curr_node, control_items *control, symbol_number curr_symb){
    
    if(curr_symb == RBR && exp_stack_find_lbr(estack) == false){
        curr_symb = STOP;
        free(curr_node);
        free(control);
    }

    symbol_number top_term = exp_stack_top_term_symb(estack);
    precedence compare = precedence_table[top_term][curr_symb];

    if(compare == LS || compare == EQ){
        
        exp_stack_push(estack, curr_node, curr_symb, control);
        return 0;
    }
    else if(compare == GR){
        reduce(estack);
        return shift(estack, curr_node, control, curr_symb);
    }
    else{
        if(top_term == STOP && curr_symb == STOP){
            return 1; 
        }
        else{
            
            ERROR(ERR_SYNTAX, ("Invalid expression\n")); 
        }
    }
}



/**
 * @brief Performs reduction rules for expressions.
 *
 *      Reduction rules include:
 *          E -> id
 *          E -> (E)
 *          E -> E * E
 *          E -> E / E
 *          E -> E + E
 *          E -> E - E
 *          E -> E > E
 *          E -> E < E
 *          E -> E <= E
 *          E -> E >= E
 *          E -> E == E
 *          E -> E != E
 * 
 * @param stack Pointer to the stack containing elements of the expression.
 */
void reduce(exp_stack *stack){
    symbol_number top_term = exp_stack_top_term_symb(stack);
    switch(top_term){
        case ID :
            stack->top->expr = NO_TERMINAL;
            return;
 
        case RBR :{
            astNode *rbr = exp_stack_pop(stack, false);    // delete right bracket item from stack
            freeASTNode(rbr); 
            control_items *operand_items = stack->top->control;
            astNode *expr = exp_stack_pop(stack, true);
            if(stack->top->expr != LBR){
                ERROR(ERR_SYNTAX, "Unexpected \")\" in expression ");
            }
            astNode *lbr = exp_stack_pop(stack, false); // delete left bracket item from stack
            freeASTNode(lbr);
            exp_stack_push(stack, expr, NO_TERMINAL, operand_items);
            return;
        }

        case STOP :
            return;

        case LOWER:
        case LOWER_OR_EQUAL:
        case GREATER:
        case GREATER_OR_EQUAL:
        case EQUAL:
        case NOT_EQUAL:
            if(stack->count < 4 ){
                    ERROR(ERR_SYNTAX, "Invalid character in expression\n");
                }
            if(stack->top->next->next->node->type == AST_NODE_BINOP){   // for cases when logical operator was reduced to nonterminal due to E -> (E) rule
                symbol_number operator = stack->top->next->next->node->nodeRep.binOpNode.op;
                if(operator == LOWER || operator == LOWER_OR_EQUAL || operator == GREATER || operator == GREATER_OR_EQUAL || operator == EQUAL || operator == NOT_EQUAL){
                    ERROR(ERR_SYNTAX, "Invalid expression.\n");
                }
            }
            break;

        default :
            break;
    }

    if(stack->count < 4 ){
        ERROR(ERR_SYNTAX, "Invalid character in expression\n");
    }
    
    control_items *operation_item = (control_items *)malloc(sizeof(struct control_items));
    
    semantic_check(stack->top->next->next, stack->top->next, stack->top, operation_item);
    
    astNode *right_elem = exp_stack_pop(stack, false);
    astNode *operator = exp_stack_pop(stack, false);
    astNode *left_elem = exp_stack_pop(stack, false);

    createBinOpNode(operator, top_term, left_elem, right_elem, operation_item->type, NULL); 
    exp_stack_push(stack, operator, NO_TERMINAL, operation_item);
    return;

      
}

/**
 * @brief           Evalutates given token assigns proper symbol number for expression precedence
 * 
 * @param stack     pointer to stack where are all elements of expression stored
 * @param token     current token that is processed
 * @param node      pointer to node that will create part of expression in case of id or funcion call
 * @param control   pointer to control struct that stores informations about node needed for future handling
 * 
 * @return          Symbol number of expression character
 */
symbol_number evaluate_given_token(exp_stack *estack, Token token, astNode *node, control_items *control){
    symNode *symnode;
    switch(token.type){
        case tokentype_multiply :
            
            return MULTIPLICATION;

        case tokentype_divide :
            
            return DIVISION;
            
        case tokentype_plus :
            
            return ADDITION;
            
        case tokentype_minus :
            
            return SUBSTRACTION;
        
        case tokentype_equal :
            
            return EQUAL;
        
        case tokentype_notequal :
            
            return NOT_EQUAL;

        case tokentype_lower :
            
            return LOWER;

        case tokentype_greater :
           
            return GREATER;
        
        case tokentype_lowerequal :
            
            return LOWER_OR_EQUAL;
        
        case tokentype_greaterequal :
            
            return GREATER_OR_EQUAL;

        case tokentype_lbracket :
            
            return LBR;

        case tokentype_rbracket :
            if(exp_stack_find_lbr(estack)){
                
                return RBR;
            }
            freeASTNode(node);
            free(control);
            return STOP;

        case tokentype_id :
            if(wasDefined(token.value, &symnode)){

                if(symnode->data.data.vData.knownDuringCompile == true){    //if variable has value known during compile check if it can be converted to different type and set flag in control struct
                    control->known_during_compile = true;

                    if(symnode->data.data.vData.isNullable == false){   // Nullable variable has to be checked for possible null value by user (if not this check needs to be redone) 
                        dataType type = symnode->data.data.vData.type;

                        if(type == f64){   // when value of node is float (or double float)
                            double fdata = symnode->data.data.vData.value.floatData;
                            if((int) fdata == fdata){ //can be converted into int
                                    createLiteralNode(node, f64, &fdata, NULL);
                                    control->is_convertable = true;
                                    control->is_nullable = false;
                                    control->type = f64;
                                    return ID;
                            }
                        }

                        else if(type == i32){
                            int idata = symnode->data.data.vData.value.intData; //TODO shouldn't this be long int?
                            createLiteralNode(node, i32, &idata, NULL);
                            control->is_convertable = true;                     //every i32 can be converted to f64
                            control->is_nullable = false;
                            control->type = i32;
                            return ID;
                        }
                    }
                }
                
                createVarNode(node, token.value, symnode->data.data.vData.type, symnode, NULL);
                control->known_during_compile = false;
                control->is_convertable = false;
                control->type = symnode->data.data.vData.type;
                control->is_nullable = symnode->data.data.vData.isNullable;
                return ID;
            }
            else{
                char *id = currentToken.value;
                GT
                if(currentToken.type == tokentype_lbracket || currentToken.type == tokentype_dot){
                    
                    funCallHandle(id, node, true);
                    
                    control->known_during_compile = false;
                    control->is_nullable = node->nodeRep.funcCallNode.nullableRetType;
                    control->type = node->nodeRep.funcCallNode.retType;
                    control->is_convertable = false;
                    return ID;
                }
                else{
                    ERROR(ERR_SEM_UNDEF, ("Variable undefined.\n")); 
                }
            }

        case tokentype_int :{
            int ivalue = atoi(token.value);
            createLiteralNode(node, i32, &ivalue,  NULL);
            control->is_nullable = false;
            control->type = i32;
           
            control->is_convertable = true;
            control->known_during_compile = true;
            return ID;
        }
        case tokentype_exponentialnum:
        case tokentype_float :{
            double fvalue = strtod(token.value, NULL);
            createLiteralNode(node, f64, &fvalue, NULL);

            control->is_nullable = false;
            control->type = f64;
            
            control->known_during_compile = true;

            if((int) node->nodeRep.literalNode.value.floatData == node->nodeRep.literalNode.value.floatData ){
                control->is_convertable = true;
            }
            else{
                control->is_convertable = false;
            }
            return ID;
        }

        case tokentype_kw_null :
            createLiteralNode(node, null_, NULL, NULL );
            control->known_during_compile = true;
            control->is_nullable = true;
            control->type = null_;
            
            control->is_convertable = false;
            return ID;

        case tokentype_zeroint :{
            int zero_int = 0;
            createLiteralNode(node, i32, &zero_int, NULL);
            control->known_during_compile = true;
            control->is_nullable = false;
            control->type = i32;
            
            control->is_convertable = true;
            return ID;
        }

        case tokentype_string:
            createLiteralNode(node, string, token.value, NULL); 
            control->known_during_compile = true;
            control->is_convertable = false;
            control->is_nullable = false;
            
            control->type = string;
            return ID;

        default:
            freeASTNode(node);
            free(control);
            return STOP;
        

    }
}

/**
 * @brief                   Function that performs semantic checks for binary operations.  
 * 
 * @param left_operand      Pointer to a stack element representing the left operand.  
 * @param operator          Pointer to a stack element representing the operator.
 * @param right_operand     Pointer to a stack element representing the right operand.
 * @param control           Pointer to a control structure storing information about the new stack element needed for future handling.  
 */
void semantic_check(stack_item *left_operand, stack_item *operator, stack_item *right_operand, control_items *control){
    if(left_operand->expr != NO_TERMINAL || left_operand->expr != NO_TERMINAL){
        ERROR(ERR_SYNTAX, "Unexpected character in expression\n"); 
    }

    if((operator->expr == NOT_EQUAL || operator->expr == EQUAL)){
        if(left_operand->control->type == null_ && right_operand->control->is_nullable == false){
            ERROR(ERR_SEM_TYPE, "Cannot compere null value with non null operand\n");
        }
        else if(right_operand->control->type == null_ && left_operand->control->is_nullable == false){
            ERROR(ERR_SEM_TYPE, "Cannot compere null value with non null operand\n");
        }
        else{
            if(left_operand->control->type == null_ || right_operand->control->type == null_){
            control->known_during_compile = false;
            control->is_convertable = false;
            control->is_nullable = true;
            control->type = null_;
            return;
            }
        }
    }


    if(left_operand->control->type == u8 || right_operand->control->type == u8 || left_operand->control->type == string || right_operand->control->type == string ){
        ERROR(ERR_SEM_TYPE, ("Cannont use u8 type or string in arithmetical or logical operations\n"));
    }
    if((left_operand->control->is_nullable == true || right_operand->control->is_nullable == true) && operator->expr != NOT_EQUAL && operator->expr != EQUAL){
        ERROR(ERR_SEM_TYPE, "Operand with null cannot be used in expression other than == and != \n");
    }

    control->known_during_compile = false;
    control->is_nullable = false;

    if(left_operand->control->type == right_operand->control->type){
        control->type = left_operand->control->type;
        if(left_operand->control->is_convertable == true && right_operand->control->is_convertable == true){
            control->is_convertable = true;
        }
        else{
            control->is_convertable = false;
        }
        return; 

    }

    else if(right_operand->control->is_convertable == true && right_operand->control->type == i32){
        retype(right_operand->node);
        control->type = left_operand->control->type;
        if(left_operand->control->is_convertable == true){
            control->is_convertable = true;
        }
        else{
            control->is_convertable = false;
        }
        return;
    }

    else if(left_operand->control->is_convertable == true && left_operand->control->type == i32){
        retype(left_operand->node);
        control->type = right_operand->control->type;
        if(right_operand->control->is_convertable == true){
            control->is_convertable = true;
        }
        else{
            control->is_convertable = false;
        }
        return;
    }


    else if(right_operand->control->is_convertable == true){
        retype(right_operand->node);
        control->type = left_operand->control->type;
        if(left_operand->control->is_convertable == true){
            control->is_convertable = true;
        }
        else{
            control->is_convertable = false;
        }
        return;

    }

    else if(left_operand->control->is_convertable == true){
        retype(left_operand->node);

        control->type = right_operand->control->type;

        if(right_operand->control->is_convertable == true){
            control->is_convertable = true;
        }
        else{
            control->is_convertable = false;
        }
        return;

    }
    else{
        ERROR(ERR_SEM_TYPE, "types of operands in expresions don't match\n");
    }

}

/**
 * @brief           Function that retypes nodes in an expression tree.  
 * 
 * @param operand   Pointer to a node in the tree that needs to be retyped.  
 */
void retype(astNode *operand){
    
    if(operand->type == AST_NODE_BINOP){
        
        retype(operand->nodeRep.binOpNode.left);
        retype(operand->nodeRep.binOpNode.right);
        return;
    }
    else if(operand->type == AST_NODE_LITERAL){
        
        dataType type = operand->nodeRep.literalNode.dataT;
        
        if(type == i32){
            int value = operand->nodeRep.literalNode.value.intData;
            operand->nodeRep.literalNode.dataT = f64;
            operand->nodeRep.literalNode.value.floatData = (double) value; 
            return;
        }
        else if(type == f64){
            double value = operand->nodeRep.literalNode.value.floatData;
            operand->nodeRep.literalNode.dataT = i32;
            operand->nodeRep.literalNode.value.intData = (int) value;
            return; 
        }
        else{
            ERROR(ERR_INTERNAL, "Fault in compile. I wasn't counting on this D: .\n");
        }
    }
}

/* EOF expression_parser.c */