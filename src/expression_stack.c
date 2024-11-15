#include <stdio.h>
#include "expression_stack.h"

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

/*
* Vytvor a inicializuj stack, proste allokuj a daj top na NULL
*/
exp_stack *exp_stack_create(){
    exp_stack *Stack = (exp_stack *)malloc(sizeof(exp_stack));
    if(Stack == NULL){
        return NULL; //internal error
    }
    Stack->top = NULL;
    Stack->count = 0;
    return Stack;
}


/** 
* @brief push on stack new item 
* 
* @param estack pointer to stack
* @param node 
* @param op operator needed for precedence parsing
* @param type what type is new item
* @param isnullable true if parameter can be NULL false if not
* @param convert true if we can convert parameter to different type
*/
void exp_stack_push(exp_stack *estack, astNode *node, symbol_number op){
    stack_item *new_item = (stack_item *)malloc(sizeof(stack_item));
    if(new_item == NULL){
        return; //internal error
    }
    new_item->node = node;
    new_item->expr = op;
    new_item->next = estack->top;
    estack->top = new_item;
    estack->count++;  //v prípade problemu obrať sa na tuto inštrukciu
}





astNode *exp_stack_pop_node(exp_stack *estack){
    if (estack->top == NULL){
        return NULL;
    }
    astNode *top_node = estack->top->node;
    stack_item *top_to_delete = estack->top;
    estack->top = estack->top->next;
    //free(top_to_delete->control);
    free(top_to_delete);
    estack->count--;
    return top_node;
}


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



bool exp_stack_is_empty(exp_stack *estack){
    return estack->top == NULL;
}



void exp_stack_free_stack(exp_stack *estack){
    stack_item *curr = estack->top;
    while(curr){
        stack_item *next_item = curr->next;
        free(curr);
        curr = next_item;
    }
    free(estack);
}

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

stack_item *exp_stack_pop(exp_stack *estack){
    stack_item *popped_item = estack->top;
    estack->top = estack->top->next;
    estack->count--;
    return popped_item;
}

/****************************************************************************************** */
/*                              veci stade napchaj potom inde                               */

// ako opravil som niektoré veci pri základom parsovaní ale pokazil som vyhodnotenie error funkcie

/*
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
        if(estack == NULL){
            return false; //TODO internal error
        }
    exp_stack_push(estack, NULL, STOP);    //push stop symbol on top of the stack
    DEBPRINT("pushed stop sign to stack\n");

    if(process_expr(estack)){
        
        control_items *expr_items = estack->top->control;
       
        astNode *final_exp = exp_stack_pop_node(estack);
        

        createExpressionNode(expr_node, expr_items->type, final_exp, expr_items->is_nullable);  // TODO MATUS TOTO FALSE TAM ASI NEMA BYT UPRAV PLS DIK TO JA PRIDAL NECH NA MNA NEKRICI VSCODE
        
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
    if(curr_node == NULL){
        return false; //TODO internal Error
    }

    control_items *control =(control_items *)malloc(sizeof(struct control_items));
    if(control == NULL){
        return false; //TODO internal Error
    }

    // initialization of empty controls
    control->is_convertable = false;
    control->is_nullable = false;
    control->litconst = false;
    control->type = void_;


    symbol_number curr_symb = evaluate_given_token(estack, currentToken, curr_node, control);
    DEBPRINT("evaluated current token as %d (symbol_number)\n", curr_symb);       
    if(curr_symb == ERROR){
        
        ERROR(ERR_SYNTAX, "Wrong Token in expression\n");
    }
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

/*
* @brief A function that evaluates the precedence of a token with the precedence of the highest terminal symbol and applies a rule according to precedence parsing.
*
* @param estack A pointer to an expression stack
* @param pointer to a node of an expression tree
* 
* @return True if the expression has been fully processed or encountered an error; false if the expression has not been fully processed.
*/
int shift(exp_stack *estack, astNode *curr_node, control_items *control, symbol_number curr_symb){
    //DEBPRINT("som vo funkcii shift a na vrchole stacku sa nachádza %d", estack->top->expr);


    symbol_number top_term = exp_stack_top_term_symb(estack);
    DEBPRINT("in fucnion shit found that top term symbol on stack is %d (symbol_number)\n", top_term);
    precedence compare = precedence_table[top_term][curr_symb];
    DEBPRINT("Porovnaním najvyššieho terminalu na stacku a mojho znaku som priradil precednciu %d\n", compare);
    if(compare == LS || compare == EQ){
        
        exp_stack_push(estack, curr_node, curr_symb);
        estack->top->control = control;
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


// NULL rieš ako literál

void reduce(exp_stack *stack){
    symbol_number top_term = exp_stack_top_term_symb(stack);
    switch(top_term){
        case ID :
            stack->top->expr = NO_TERMINAL;
            break;
 
        case RBR :
            astNode *rbr = exp_stack_pop_node(stack);
            free(rbr);
            control_items *term_items = stack->top->control;
            astNode *expr = exp_stack_pop_node(stack);
            if(stack->top->expr != LBR){
                ERROR(ERR_SYNTAX, "Invalid Token");
            }
            astNode *lbr = exp_stack_pop_node(stack);
            free(lbr);
            exp_stack_push(stack, expr, NO_TERMINAL);
            stack->top->control = term_items;
            return;

        case STOP :
            break;


        case MULTIPLICATION:
        case DIVISION:
        case ADDITION:
        case SUBSTRACTION:
        case LOWER:
        case GREATER:
        case LOWER_OR_EQUAL:
        case GREATER_OR_EQUAL:
            if(stack->count < 4 ){  //môže sa toto vykonať validne ked je na stacku menej ako 3 položky, jeden stop operand operator operand
                ERROR(ERR_SYNTAX, "Invalid token\n");
            }
            if(stack->top->control->is_nullable == true){
                ERROR(ERR_SYNTAX, ("Operand with null cannot be used in expression\n"));
            }
            else if(stack->top->next->next->control->is_nullable == true){
                ERROR(ERR_SYNTAX, ("Operand with null cannot be used in expression\n"));
            }
            //ked jedna je nulovacia error

        default :
            if(stack->count < 4 ){
                ERROR(ERR_SYNTAX, "Invalid token in expression\n");
            }
            
            dataType data_type_buffer;  // semanticke pravidlo pretypovania alebo erroru, treba urobiť porovnanie oboch výrazo a prípadne pritypovanie
            
            control_items *operation_item = (control_items *)malloc(sizeof(struct control_items));
            
            semantic_check_retype(stack->top->next->next, stack->top->next, stack->top, operation_item);
            
            astNode *right_elem = exp_stack_pop_node(stack);
            astNode *operator = exp_stack_pop_node(stack);
            astNode *left_elem = exp_stack_pop_node(stack);

            createBinOpNode(operator, top_term, left_elem, right_elem, operation_item->type, NULL);
            exp_stack_push(stack, operator, NO_TERMINAL);
            stack->top->control = operation_item;
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
            return STOP;

        case tokentype_id :
        //doriešiť funkcie
            if(wasDefined(token.value, &symnode)){
                    createVarNode(node, token.value, symnode->data.data.vData.type, symnode, NULL);
                    control->litconst = node->nodeRep.varNode.symtableEntry->data.data.vData.isConst;
                    if(control->litconst == true){
                        if(symnode->data.data.vData.type == f64){
                            control->is_convertable = false;    // storing value issue

                        }else if(symnode->data.data.vData.type == i32){
                                control->is_convertable = true;
                        }else if(symnode->data.data.vData.type == u8){
                            control->is_convertable = false;
                        }else{
                             ERROR(ERR_SYNTAX, (" incompatible type in expression\n")); 
                        }
                    }
                    else{   //vo výraze môže byť premenná len typu f64 alebo i32 ostatné typy (zamerané presnejšie na []u8 je nepodoporvaný)
                        control->is_convertable = false;
                    }
                    control->is_nullable = node->nodeRep.varNode.isNullable;
                    control->type = symnode->data.data.vData.type;
                return ID;
            }
            else{
                char *id = currentToken.value;
                GT
                if(currentToken.type == tokentype_lbracket || currentToken.type == tokentype_dot){
                    
                    funCallHandle(id, node, true);
                    if(node->nodeRep.funcCallNode.retType == void_){
                        ERROR(ERR_SYNTAX, "Void funcion cannot be used in expression\n");
                    }
                    control->is_nullable = false; //alebo može byť null??
                    control->type = node->nodeRep.funcCallNode.retType;
                    control->is_convertable = false;
                    control->litconst = false;
                    
                    return ID;
                }
                else{
                    ERROR(ERR_SYNTAX, ("Unexpected Token\n")); //TODO ERRROR
                }
            }

        case tokentype_int :
            createLiteralNode(node, i32, token.value, NULL);
            control->is_nullable = false;
            control->type = i32;
            control->litconst = true;
            control->is_convertable = true;
            return ID;

        case tokentype_float :
            createLiteralNode(node, f64, token.value, NULL);
            control->is_nullable = false;
            control->type = f64;
            control->litconst = true;
            if((int) node->nodeRep.literalNode.value.floatData == node->nodeRep.literalNode.value.floatData ){
                control->is_convertable = true;
            }
            else{
                control->is_convertable = false;
            }
            return ID;

        case tokentype_kw_null :
            createLiteralNode(node, null_, NULL, NULL );
            control->is_nullable = true;
            control->type = null_;
            control->litconst = true;
            control->is_convertable = false;
            return ID;

        case tokentype_zeroint :
            createLiteralNode(node, i32, token.value, NULL);
            control->is_nullable = false;
            control->type = i32;
            control->litconst = true;
            control->is_convertable = true;
            return ID;

        case tokentype_string:
            createLiteralNode(node, u8, token.value, NULL);
            control->is_convertable = false;
            control->is_nullable = false;
            control->litconst = true;
            control->type = u8;
            return ID;

        default:
            //free(node);
            return STOP;
        

    }
}





void semantic_check_retype(stack_item *left_operand, stack_item *operator, stack_item *right_operand, control_items *control){
    if(left_operand->expr != NO_TERMINAL || left_operand->expr != NO_TERMINAL){
        ERROR(ERR_SYNTAX, "ERROR, ktorý bude došetriť vzinok na základe toho že funkcia nedostala niečo čo by vyhodnotila ako operandy\n"); //dorieš výpis
    }

    if(left_operand->control->type == u8 || left_operand->control->type == u8){
        ERROR(ERR_SEM_TYPE, ("Cannont use u8 type in arithmetical or logical operations\n"));
    }
    control->is_nullable = false;
    control->litconst = false;

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
    else if(left_operand->control->is_convertable == true){
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
        control->type = left_operand->control->type;
        if(left_operand->control->is_convertable == true){
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