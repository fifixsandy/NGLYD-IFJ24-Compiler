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
    return Stack;
}


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





astNode *exp_stack_pop(exp_stack *estack){
    if (estack->top == NULL){
        return NULL;
    }
    astNode *top_node = estack->top->node;
    stack_item *top_to_delete = estack->top;
    estack->top = estack->top->next;
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


/****************************************************************************************** */
/*                              veci stade napchaj potom inde                               */



symbol_number evaluate_given_token(exp_stack *estack, Token token, astNode *node){
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
            if(wasDefined(token.value, &symnode)){  //check že či je to NULL
                createVarNode(node, token.value, symnode->data.data.vData.type, symnode, NULL);
                return ID;
            }
            else{
                GT
                if(currentToken.value == tokentype_lbracket){
                    ERROR(ERR_SYNTAX, ("Nedeklarovana funkcia TODO-----------------------\n")); //nedeklarovana funkcia nepozname return typ na presné určenie čo to môže byť diera v strome
                }
                else{
                    ERROR(ERR_SYNTAX, ("Undeclared variable\n")); //TODO ERRROR
                }
            }

        case tokentype_int :
            createLiteralNode(node, i32, token.value, NULL);
            return ID;

        case tokentype_float :
            createLiteralNode(node, f64, token.value, NULL);
            return ID;

        default:
            //free(node);
            return STOP;
        

    }
}


// ako opravil som niektoré veci pri základom parsovaní ale pokazil som vyhodnotenie error funkcie

/*
* @brief A funcion that calls helper funcions to build an expression tree
*
* @return True if build was successful; False if build wasn't successful
*/
bool expression(astNode *expr_node){
    //fprintf(stderr, "\nDOSTAL SOM SA DO EXPRESSION ČASTI!!\n");
    exp_stack *estack = exp_stack_create();
        if(estack == NULL){
            return false; //TODO internal error
        }
    exp_stack_push(estack, NULL, STOP);

    if(process_expr(estack)){
        astNode *final_exp = exp_stack_pop(estack);
        
        createExpressionNode(expr_node, what_type(final_exp), final_exp); 
        exp_stack_free_stack(estack);
        
        return true;
    }
    
    fprintf(stderr, "HODNOTA SUČASNÉHO TOKENU: %d", currentToken.type);
    exp_stack_free_stack(estack);
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
int shift(exp_stack *estack, astNode *curr_node){
    
    symbol_number top_term = exp_stack_top_term_symb(estack);
    
    symbol_number curr_symb = evaluate_given_token(estack, currentToken, curr_node);
    if(curr_symb == ERROR){
        
        return 2; // TODO error
    }
    precedence compare = precedence_table[top_term][curr_symb];
    if(compare == LS || compare == EQ){
        exp_stack_push(estack, curr_node, curr_symb);
        return 0;
    }
    else if(compare == GR){
        fprintf(stderr, "súčasný token je %d a najvyšší znak je %d\n", currentToken.type, estack->top->expr);
        reduce(estack);
        return shift(estack, curr_node);
    }
    else{
        if(top_term == ID && curr_symb == LBR){
        }

        else if(top_term == STOP && curr_symb == STOP){
            return 1; // GG vyhrali sme
        }
        else{
            fprintf(stderr, "vyhodnocovacie pravidlo nd\n");
            ERROR(ERR_SYNTAX, ("Invalid expression, expected: \";\"\n")); // ERORR chybný expression
        }
    }
}


bool process_expr(exp_stack *estack){
    GT
    astNode *curr_node = createAstNode();
    if(curr_node == NULL){
        return false; //TODO internal Error
    }
    int evaluate = shift(estack, curr_node);
    fprintf(stderr, "prijatý token %d a hodnota shiftu je %d\n", currentToken.type, evaluate);
    if(evaluate == 0){
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



void reduce(exp_stack *stack){
    symbol_number top_term = exp_stack_top_term_symb(stack);
    switch(top_term){
        case ID :
            stack->top->expr = NO_TERMINAL;
            break;
 
        case RBR :
            astNode *rbr = exp_stack_pop(stack);
            free(rbr);
            astNode *expr = exp_stack_pop(stack);
            if(stack->top->expr != LBR){
                ERROR(ERR_SYNTAX, "Invalid Token");
            }
            astNode *lbr = exp_stack_pop(stack);
            free(lbr);
            exp_stack_push(stack, expr, NO_TERMINAL);
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
            //ked jedna je nulovacia error

        default :
            if(stack->count < 4 ){
                ERROR(ERR_SYNTAX, "Invalid token\n");
            }
            
            dataType data_type_buffer;  // semanticke pravidlo pretypovania alebo erroru, treba urobiť porovnanie oboch výrazo a prípadne pritypovanie
            astNode *right_elem = exp_stack_pop(stack);
            astNode *operator = exp_stack_pop(stack);
            astNode *left_elem = exp_stack_pop(stack);

            dataType dtype_of_r_e = what_type(right_elem);
            dataType dtype_of_l_e = what_type(left_elem);

            if(dtype_of_r_e == dtype_of_l_e){
                data_type_buffer = dtype_of_r_e;
            }
            //pretypovanie

            right_elem->parent = operator;
            left_elem->parent = operator;
            createBinOpNode(operator, top_term, left_elem, right_elem, data_type_buffer, NULL);
            exp_stack_push(stack, operator, NO_TERMINAL);
            return;
    }
      
}




dataType what_type(astNode *elemnt_node){
    switch(elemnt_node->type){
        case AST_NODE_VAR :
            return elemnt_node->nodeRep.varNode.dataT;

        case AST_NODE_LITERAL :
            return elemnt_node->nodeRep.literalNode.dataT;

        case AST_NODE_BINOP :
            return elemnt_node->nodeRep.binOpNode.dataT;

        default:
            return void_;

    }
}

bool retype_literals(astNode *left_element, astNode *right_element){
    astNodeType l_el = left_element->type;
    astNodeType r_el = right_element->type;

    if(l_el == AST_NODE_LITERAL || left_element->nodeRep.defVarNode.symtableEntry->data.data.vData.isConst == true){
        // toto som ešte nedomyslel tu treba vytvoriť proste podmienku že no pretypovať f64 na int alebo sa to bude riadiť podla assign?? že ked assign je float tak sa pretypuje na float alebo až ten koniec sa pretypuje???

    }
    else if(r_el == AST_NODE_LITERAL){
        
    }
}