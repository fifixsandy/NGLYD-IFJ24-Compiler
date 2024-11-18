#include <stdio.h>
#include "expression_stack.h"

#define EPSILON 1e-6    // 0.000001

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


/**
* @brief Creates and intializes stack for expressions
*/
exp_stack *exp_stack_create(){
    exp_stack *Stack = (exp_stack *)malloc(sizeof(exp_stack));
    if(Stack == NULL){
        ERROR(ERR_INTERNAL, "Failed to allocated memory for stack.");
    }
    Stack->top = NULL;
    Stack->count = 0;
    return Stack;
}


/** 
* @brief Pushes new item on expression stack 
* 
* @param estack pointer to stack
* @param node 
* @param op opearnd or type of expression operator 
*/
void exp_stack_push(exp_stack *estack, astNode *node, symbol_number op, control_items *control){
    stack_item *new_item = (stack_item *)malloc(sizeof(stack_item));
    if(new_item == NULL){
        ERROR(ERR_INTERNAL, "Failed to allocated memory for stack element"); //internal error
    }
    new_item->control = control;
    new_item->node = node;
    new_item->expr = op;
    new_item->next = estack->top;
    estack->top = new_item;
    estack->count++;  //v prípade problemu obrať sa na tuto inštrukciu
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
 */
symbol_number exp_stack_top_term_symb(exp_stack *estack){
    if(estack->top->expr == NO_TERMINAL){
        if(estack->top->next->expr == NO_TERMINAL){
            ERROR(ERR_SYNTAX, ("Expected operation between operands")); // toto nemože nastať keby hej error, chyba parsera alebo výrazu,
        }
        return estack->top->next->expr;
    }
    else{
        return estack->top->expr;
    }
}


/**
 * @brief destroy everything allocated in stack
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
 * @brief 
 * 
 * @return true if in the stack is left bracket; false if left bracket isn't in the stack
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



/****************************************************************************************************************************** */
/*                              veci stade napchaj potom inde                                                                   */



/**
* @brief A funcion that calls helper funcions to build an expression tree
*
* @return True if build was successful; False if build wasn't successful
*/
bool expression(astNode *expr_node){
    fprintf(stderr, "\n");
    DEBPRINT("entered expression func\n");
    bool *known_upon_compilation;
    //fprintf(stderr, "\nDOSTAL SOM SA DO EXPRESSION ČASTI!!\n");
    exp_stack *estack = exp_stack_create();

    exp_stack_push(estack, NULL, STOP, NULL);    //push stop symbol on top of the stack
    DEBPRINT("pushed stop sign to stack\n");

    if(process_expr(estack)){
        
        control_items *expr_items = estack->top->control;
       
        astNode *final_exp = exp_stack_pop(estack, true);
        
        //DEBPRINT("IS EXPRESSION KNOWN DURING COMPILATION: %d, data typ je %d\n", expr_items->known_during_compile, expr_items->type);
        createExpressionNode(expr_node, expr_items->type, final_exp, expr_items->is_nullable, expr_items->known_during_compile);  // TODO MATUS TOTO FALSE TAM ASI NEMA BYT UPRAV PLS DIK TO JA PRIDAL NECH NA MNA NEKRICI VSCODE
        
        free(expr_items);
        exp_stack_free_stack(estack);
        
        return true;
    }
    
    //fprintf(stderr, "HODNOTA SUČASNÉHO TOKENU: %d", currentToken.type);
    exp_stack_free_stack(estack);
    return false;
}

bool process_expr(exp_stack *estack){
    DEBPRINT("entered process_expr func with token %d (token_types)\n", currentToken.type);
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
    DEBPRINT("evaluated current token as %d (symbol_number)\n", curr_symb);       
    int evaluate = shift(estack, curr_node, control, curr_symb);
    //fprintf(stderr, "prijatý token %d a hodnota shiftu je %d\n", currentToken.type, evaluate);
    if(evaluate == 0){
        
        GT
        
        return process_expr(estack);
    }
    else if(evaluate == 1){
        if(estack->top->expr != NO_TERMINAL){
            ERROR(ERR_SYNTAX, "Empty expression\n");
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
* @return True if the expression has been fully processed or encountered an error; false if the expression has not been fully processed.
*/
int shift(exp_stack *estack, astNode *curr_node, control_items *control, symbol_number curr_symb){
    //DEBPRINT("som vo funkcii shift a na vrchole stacku sa nachádza %d", estack->top->expr);
    if(curr_symb == RBR && exp_stack_find_lbr(estack) == false){
        curr_symb = STOP;
        free(curr_node);
        free(control);
    }

    symbol_number top_term = exp_stack_top_term_symb(estack);
    DEBPRINT("in fucnion shit found that top term symbol on stack is %d (symbol_number)\n", top_term);
    precedence compare = precedence_table[top_term][curr_symb];
    DEBPRINT("Porovnaním najvyššieho terminalu na stacku a mojho znaku som priradil precednciu %d\n", compare);
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
            return 1; // GG vyhrali sme
        }
        else{
            //fprintf(stderr, "vyhodnocovacie pravidlo nd\n");
            ERROR(ERR_SYNTAX, ("Invalid expression, expected: \";\", sprava prišla z funkcie shift\n")); // ERORR chybný expression
        }
    }
}




void reduce(exp_stack *stack){
    symbol_number top_term = exp_stack_top_term_symb(stack);
    switch(top_term){
        case ID :
            stack->top->expr = NO_TERMINAL;
            break;
 
        case RBR :
            astNode *rbr = exp_stack_pop(stack, false);    // delete right bracket item from stack
            freeASTNode(rbr); 
            control_items *operand_items = stack->top->control;
            astNode *expr = exp_stack_pop(stack, true);
            if(stack->top->expr != LBR){
                ERROR(ERR_SYNTAX, "Unexpected token in stack");
            }
            astNode *lbr = exp_stack_pop(stack, false);
            freeASTNode(lbr);
            exp_stack_push(stack, expr, NO_TERMINAL, operand_items);
            return;

        case STOP :
            return;


        default :
            if(stack->count < 4 ){
                ERROR(ERR_SYNTAX, "Invalid token in expression\n");
            }
            
            control_items *operation_item = (control_items *)malloc(sizeof(struct control_items));
            
            semantic_check_retype(stack->top->next->next, stack->top->next, stack->top, operation_item);
            
            astNode *right_elem = exp_stack_pop(stack, false);
            astNode *operator = exp_stack_pop(stack, false);
            astNode *left_elem = exp_stack_pop(stack, false);

            createBinOpNode(operator, top_term, left_elem, right_elem, operation_item->type, NULL);
            exp_stack_push(stack, operator, NO_TERMINAL, operation_item);
            return;
    }
      
}


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
                            float fdata = symnode->data.data.vData.value.floatData;
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
                            control->is_convertable = true; //every i32 can be converted to f64
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
                    if(node->nodeRep.funcCallNode.retType == void_){
                        ERROR(ERR_SYNTAX, "Void funcion cannot be used in expression.\n");
                    }

                    control->known_during_compile = false;
                    control->is_nullable = node->nodeRep.funcCallNode.nullableRetType;
                    control->type = node->nodeRep.funcCallNode.retType;
                    control->is_convertable = false;
                    return ID;
                }
                else{
                    ERROR(ERR_SEM_UNDEF, ("Variable or function undefined.\n")); //TODO ERRROR
                }
            }

        case tokentype_int :
            int ivalue = atoi(token.value);
            createLiteralNode(node, i32, &ivalue,  NULL);
            control->is_nullable = false;
            control->type = i32;
           
            control->is_convertable = true;
            control->known_during_compile = true;
            return ID;

        case tokentype_float :
            float fvalue = atof(token.value);
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

        case tokentype_kw_null :
            createLiteralNode(node, null_, NULL, NULL );
            control->known_during_compile = true;
            control->is_nullable = true;
            control->type = null_;
            
            control->is_convertable = false;
            return ID;

        case tokentype_zeroint :
            int zero_int = 0;
            createLiteralNode(node, i32, &zero_int, NULL);
            control->known_during_compile = true;
            control->is_nullable = false;
            control->type = i32;
            
            control->is_convertable = true;
            return ID;

        case tokentype_string:
            createLiteralNode(node, string, token.value, NULL); //TODO strings
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


void semantic_check_retype(stack_item *left_operand, stack_item *operator, stack_item *right_operand, control_items *control){
    if(left_operand->expr != NO_TERMINAL || left_operand->expr != NO_TERMINAL){
        ERROR(ERR_SYNTAX, "ERROR, ktorý bude došetriť vzinok na základe toho že funkcia nedostala niečo čo by vyhodnotila ako operandy\n"); //dorieš výpis
    }

    if(left_operand->control->type == u8 || right_operand->control->type == u8 || left_operand->control->type == string || right_operand->control->type == string ){
        ERROR(ERR_SEM_TYPE, ("Cannont use u8 type or string in arithmetical or logical operations\n"));
    }
    if((left_operand->control->is_nullable == true || right_operand->control->is_nullable == true) && operator->expr != NOT_EQUAL && operator->expr != EQUAL){
        ERROR(ERR_SEM_TYPE, "Operand with null cannot be used in expression\n");
    }

    if(right_operand->control->known_during_compile == true && operator->expr == DIVISION){
        dataType typeZ = right_operand->node->nodeRep.literalNode.dataT;
        if( typeZ == f64){
            float fvalue = right_operand->node->nodeRep.literalNode.value.floatData;
            if( fabs(fvalue) < EPSILON){        // if expresion is devided by zero or very small number
                ERROR(ERR_SEM_ELSE, "Expression can't be devided by zero\n");
            }
        }
        if(typeZ == i32){
            int ivalue = right_operand->node->nodeRep.literalNode.value.intData;
            if(ivalue == 0){        // if expresion is devided by zero
                ERROR(ERR_SEM_ELSE, "Expression can't be devided by zero\n");
            }
        }
    }

    control->known_during_compile = false;
    control->is_nullable = false;


    if((operator->expr == NOT_EQUAL || operator->expr == EQUAL)){
        if(left_operand->control->type == null_ && right_operand->control->is_nullable == false){
            ERROR(ERR_SEM_TYPE, "Cannot compere null value with non null operand\n");
        }
        else if(right_operand->control->type == null_ && left_operand->control->is_nullable == false){
            ERROR(ERR_SEM_TYPE, "Cannot compere null value with non null operand\n");
        }
        
    }

    

    if(left_operand->control->type == right_operand->control->type){
        control->type = left_operand->control->type;
        if(left_operand->control->is_convertable == true && right_operand->control->is_convertable == true){
            control->is_convertable = true;
        }
        else{
            control->is_convertable = false;
        }
        return; //všetko vpohode nerieš

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


void retype(astNode *operand){
    
    if(operand->type == AST_NODE_BINOP){
        //DEBPRINT("Môj typ je %d - nachadzam sa v node bin operacie\n", operand->nodeRep.binOpNode.dataT);
        retype(operand->nodeRep.binOpNode.left);
        retype(operand->nodeRep.binOpNode.right);
        return;
    }
    else if(operand->type = AST_NODE_LITERAL){
        
        dataType type = operand->nodeRep.literalNode.dataT;
        //DEBPRINT("Môj typ je %d - nachadzam sa v node literal\n", type);
        if(type == i32){
            int value = operand->nodeRep.literalNode.value.intData;
            operand->nodeRep.literalNode.dataT = f64;
            operand->nodeRep.literalNode.value.floatData = (float) value; 
            return;
        }
        else if(type == f64){
            float value = operand->nodeRep.literalNode.value.floatData;
            operand->nodeRep.literalNode.dataT = i32;
            operand->nodeRep.literalNode.value.intData = (int) value;
            return; 
        }
        else{
            ERROR(ERR_INTERNAL, "TODO in fucion retype, recieved wrong type.\n");
        }
    }
}