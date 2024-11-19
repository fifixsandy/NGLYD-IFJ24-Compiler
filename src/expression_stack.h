
#ifndef EXP_STACK_H
#define EXP_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "ast.h"
#include "error.h"

//precedence table
typedef enum{
    LS,     // less                                         0
    EQ,     // equal                                        1
    GR,     // greater than                                 2
    ND,     // not definied or specific behavior or end     3
    AC,     // accept                                       4 -???
} precedence ;





extern precedence precedence_table[14][14];

typedef struct control_items{
    bool known_during_compile;
    bool is_nullable;
    bool is_convertable;
    dataType type;
} control_items;

typedef struct stack_item{
    astNode *node;
    symbol_number expr;
    struct stack_item *next;
    struct control_items *control;
} stack_item;


typedef struct exp_stack{
    stack_item *top;
    int count;
} exp_stack;

exp_stack *exp_stack_create();
void exp_stack_push(exp_stack *estack, astNode *node, symbol_number op, control_items *control);
astNode *exp_stack_pop(exp_stack *estack, bool control_needed);
symbol_number exp_stack_top_term_symb(exp_stack *estack); //toto musí niečo vraciať s čím sa bude dať porovnavať, UPDATE: poriešil som to v hlave
void exp_stack_free_stack(exp_stack *estack);
bool exp_stack_find_lbr(exp_stack *estack);


/****************************************************************************************** */
/*                              veci stade napchaj potom inde                               */
symbol_number evaluate_given_token(exp_stack *estack, Token token, astNode *node, control_items *control);    //tu možeš poriešiť asi aj funkciu -> id
//funkcia na poskladanie stromu zo stacku ale to potom
//void reduce_bin_op(exp_stack *estack, control_items *control);
void reduce(exp_stack *estack);
int shift(exp_stack *estack, astNode *curr_node, control_items *control, symbol_number curr_symb);

bool expression(astNode *expr_node);
bool process_expr(exp_stack *estack);
void retype(astNode *operand);


#endif