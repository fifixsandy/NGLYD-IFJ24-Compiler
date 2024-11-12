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

/*
* Pushni node na vrchol zásobníka, vytvor element stacku a hoď ho na vrchol ez ne?
* veľmi podstatný je symbol_number a tu ide vec lebo podľa normálneho algoritmu sa musí dať symbol indetifikátora na vrchol zásobníka
* a až potom sa vyhodnocuje jeho prevod na neterminálny symbol IBAŽE asi vo väčšine prípadoch sa ten symbol automaticky prevedie
* neprevedie sa iba v prípade, pokiaľ je id na vrchole zásobníka a na input príde (, no my vtedy vyhodnocujeme že sa jedná o funkciu
* čiže tá sa premení na neterminál a po neterminále tiež nemôže byť zátvorka (pokiaľ v IFJ24 neexistujú implicitné *) ale vyhodnotenie
* niečoho takého by no bolo zaujímavé asi najlepšie to bude hodiť na exp_stack a pri ďalšom symbole vyhodnotiť a previesť na neterminál
* potom znovu sa znovu pokusiť to hodiť na stack a vyhodnotiť podľa pravidiel, lebo keď pri prvom hodení víde ND tak je problém niečo
* čo nemôže byť a BUUUUUUM error, keď si toto dočítal sorry xDD ako sa máš mi napíš na DS
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


/*
* No tu je kus problém lebo funkcia bude vraciať element toho zásobníka keď sa to popne a ten obsahuje viac vecí než len ast node
* tu to bude treba domyslieť asi by som mohol zrobiť že by sa vytiahol iba node stack_element by sa uvoľnil celý element lebo aj tak
* jeho atributy sa budú meniť hlavne keď budem robiť reduce no a tu ide otázka
* nedochádza tu ku memorry leakom????
*/

//treba zavolať funkcia ktorá vyymaže stack_item a nechá iba astNode proste len priradim do premennej vytvorim novu do tej priradim ast a staru free
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

/*
* nič extra len kvôli porovnaniu treba poznať najvyšší terminálny symbol aby sa podľa precedenčnej tabuľky vedelo vkládať do stacku
* prípadne vytvárať podstromy s nižšou prioritou
*/
symbol_number exp_stack_top_term_symb(exp_stack *estack){
    if(estack->top->expr == NO_TERMINAL){
        if(estack->top->next->expr == NO_TERMINAL){
            return ERROR; // toto nemože nastať keby hej error, chyba parsera alebo výrazu,
        }
        return estack->top->next->expr;
    }
    else{
        return estack->top->expr;
    }
}


/*
* tato funkcia sa sama vysvetľuje takže skip
*/
bool exp_stack_is_empty(exp_stack *estack){
    return estack->top == NULL;
}


/*
* asi by sa patrilo mať niečo takéto aj keď neviem aby sme mohli vyprázdniť stack musíme hej musíme 
*/
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
        case tokentype_zeroint :
            createLiteralNode(node, i32, token.value, NULL);
            return ID;
            
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
                return ID;
            }
            else{
                return ERROR; //TODO ERRROR
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
        // no a tu sme pri probléééééme ktorý vyžaduje vyriešnie, počuješ budúci matúš, okay poriešil si to asi, si boss, diki, ja viem

    }
}


// tu by trebalo funkciu čo by vyhodnotila že id ako klasické id a id ako funkciu ale keď sa nemýlim filip mi hoovril že ast vie to rozpoznať
//volanie funkcie bude jeden node??

//vytvoernie vzoru pre pravidlo reduce


//zisti typ tej kokotiny čo si spracoval ked je to číslo len na základe tokenu 
//ked je to id hladaj ked nie je inicalizované id chyba


bool expression(astNode *expr_node){
    exp_stack *estack = exp_stack_create();
        if(estack == NULL){
            return NULL; //TODO internal error
        }
    exp_stack_push(estack, NULL, STOP);

    if(shift(estack)){
        astNode *final_exp = exp_stack_pop(estack);
        
        createExpressionNode(expr_node, what_type(final_exp), final_exp); 
        exp_stack_free_stack(estack);
        return true;
    }
    // tu budeš volať process_expr
    exp_stack_free_stack(estack);
    return false;
}

bool shift(exp_stack *estack){
    GT
    astNode *curr_node = createAstNode();
    if(curr_node == NULL){
        return false; //TODO internal Error
    }
    HERE:
    symbol_number top_term = exp_stack_top_term_symb(estack);
    if(top_term == ERROR){
        return false; // TODO WRONG INPUT error
    }
    
    DEBPRINT("currtokentype %d\n", currentToken.type);
    symbol_number curr_symb = evaluate_given_token(estack, currentToken, curr_node);
    DEBPRINT("SYMBOL NUM %d,,,,%d\n", curr_symb, top_term);
    precedence compare = precedence_table[top_term][curr_symb];
    DEBPRINT("precedenc %d\n", compare);
    if(compare == LS){
        exp_stack_push(estack, curr_node, curr_symb);
    }
    else if(compare == GR || compare == EQ){
        reduce(estack);
        goto HERE;
        exp_stack_push(estack, curr_node, curr_symb);
    }
    else{
        if(top_term == STOP && curr_symb == STOP){
            return true; // GG vyhrali sme
        }
        
    }
    return shift(estack);
}


astNode *process_expr(exp_stack *exp_stack){
    GT
    DEBPRINT("curr token type %d \n", currentToken.type);
    astNode *curr_node = createAstNode();
    symbol_number curr_symb = evaluate_given_token(exp_stack, currentToken, curr_node);
    if(curr_symb == STOP){
        return NULL; //TODO doriešiť ako pracovať s týmto a vyhodnotiť kedy je to error
    }
    return curr_node;
}



void reduce(exp_stack *stack){
    symbol_number top_term = exp_stack_top_term_symb(stack);
    switch(top_term){
        case ID :
            stack->top->expr = NO_TERMINAL;
            break;
 
        case RBR :
            if(stack->count < 4){
                return; // TODO error
            }
            astNode *rbr = exp_stack_pop(stack);
            free(rbr);
            astNode *expr = exp_stack_pop(stack);
            if(stack->top->expr != LBR){
                return; // TODO error
            }
            astNode *lbr = exp_stack_pop(stack);
            free(lbr);
            exp_stack_push(stack, expr, NO_TERMINAL);
            return;

        case STOP :
            //napojenie na expr, node, môj výsledný strom
            break;

        default :
            if(stack->count < 4 || top_term == ERROR){
                return; // TODO error
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
    DEBPRINT("ELEMENT IS NULL %d at %d\n", elemnt_node == NULL, currentToken.type);
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


/*
find in stack

*/


/*
RULES : { 
1. E->i
2. E->E*E
3. E->E/E
4. E->E+E
5. E->E-E
6. E->E==E
7. E->E!=E
8. E->E<E
9. E->E>E
10. E->E<=E
11. E->E>=E
12. E->(E)
*/

/*
while(curr_stack_item->expr != STOP){
        count++;
        curr_stack_item->next;
        if(curr_stack_item == ID){
            break;
        }
        else if(curr_stack_item != ID || curr_stack_item != NO_TERMINAL || curr_stack_item)
    }
*/


//pokiaľ by to bolo možné táto funkcia sa dá uplne spraviť inak enum kktiny
// tu by trebalo zrobiť asi aj semanticke porovnanie a porovnanie či popy boli uspšené


/*
*@brief funcion to apply reduction rule of terminal in expression stack based on precedencial rules
*
*/


/*
    
*/



        

    
// create astNOde