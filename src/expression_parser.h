/**
 *         Implementace překladače imperativního jazyka IFJ24.
 * 
 * @file   expression_parser.h
 * 
 * @brief  Function declarations, structs and extern declaration for
 *         parsing expressions using precedence parsing techniques
 *         defined in expression_parser.c.
 * 
 * @author xfignam00 Matúš Fignár
 * @date   21.11.2024
*/


#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "ast.h"
#include "error.h"

//  enum for precedence table
typedef enum{
    LS,     // less                                         0
    EQ,     // equal                                        1
    GR,     // greater than                                 2
    ND,     // not definied or specific behavior or end     3
    AC,     // accept                                       4 -???
} precedence ;



extern precedence precedence_table[14][14];


// Stores metadata about an expression's state for semantic checks.
typedef struct control_items{
    bool known_during_compile;
    bool is_nullable;
    bool is_convertable;
    dataType type;
} control_items;


// Represents a single item in the expression stack.
typedef struct stack_item{
    astNode *node;
    symbol_number expr;                 //Represents the type or role of the stack item 
    struct stack_item *next;
    struct control_items *control;      //Pointer to the control_items struct containing semantic metadata
} stack_item;



// Represents a stack used for expression parsing and evaluation.
typedef struct exp_stack{
    stack_item *top;
    int count;
} exp_stack;



exp_stack *exp_stack_create();
void exp_stack_push(exp_stack *estack, astNode *node, symbol_number op, control_items *control);
astNode *exp_stack_pop(exp_stack *estack, bool control_needed);
symbol_number exp_stack_top_term_symb(exp_stack *estack); 
void exp_stack_free_stack(exp_stack *estack);
bool exp_stack_find_lbr(exp_stack *estack);


bool expression(astNode *expr_node);
bool process_expr(exp_stack *estack);
symbol_number evaluate_given_token(exp_stack *estack, Token token, astNode *node, control_items *control); 
int shift(exp_stack *estack, astNode *curr_node, control_items *control, symbol_number curr_symb);
void reduce(exp_stack *estack);
void semantic_check(stack_item *left_operand, stack_item *operator, stack_item *right_operand, control_items *control);
void retype(astNode *operand);


#endif