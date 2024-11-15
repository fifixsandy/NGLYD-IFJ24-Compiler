/**
 * @file   scanner.h
 * 
 * @brief  Header file for scanner for IFJ24
 * 
 *         This is a header file which includes declarations
 *         of functions used in scanner.c to tokenize
 *         input.
 *         
 * @author xmalegt00
 * @date   15.11.2024
*/

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "error.h"

#define NUM_OF_KEYWORDS 13 //Defining the number of keywords.

extern int Line_Number;    //Global variable for tracking the line number.
extern int Column_Number;

/*
An enum holding each of the types of token possible.
*/
typedef enum {
    tokentype_EOF,

    tokentype_vbar,
    tokentype_semicolon,
    tokentype_notequal,

    tokentype_lbracket,
    tokentype_rbracket,

    tokentype_lcbracket,
    tokentype_rcbracket,

    tokentype_plus,
    tokentype_minus,
    tokentype_divide,
    tokentype_comment,
    tokentype_multiply,
    tokentype_assign,
    tokentype_equal,
    tokentype_lower,
    tokentype_lowerequal,
    tokentype_greater,
    tokentype_greaterequal,
    tokentype_import,
    tokentype_dot,
    tokentype_colon,
    tokentype_string,
    tokentype_id,
    tokentype_keyword,
    tokentype_pseudovar,
    tokentype_zeroint,
    tokentype_int,
    tokentype_float,
    tokentype_exponentialnum,
    tokentype_nullid,
    tokentype_comma,
    tokentype_lsbracket,
    tokentype_rsbracket,

    //KEYWORD TYPES
    tokentype_kw_const,
    tokentype_kw_else,
    tokentype_kw_fn,
    tokentype_kw_if,
    tokentype_kw_i32,
    tokentype_kw_f64,
    tokentype_kw_null,
    tokentype_kw_pub,
    tokentype_kw_return,
    tokentype_kw_u8,
    tokentype_kw_var,
    tokentype_kw_void,
    tokentype_kw_while
    
}token_types;

#define FIRST_KEYWORD tokentype_kw_const //Define the first keyword token type for use in functions.

/* 
Structure of token, containing its type, line, column and value
*/
typedef struct {
    token_types type;
    int line;
    int column;
    char* value;
} Token;

/*
Linked list for storing values of each token,
containing the value of token and a pointer to the value
*/
typedef struct TokenValues {
    char *value;
    struct TokenValues *next;
} TokenValues;

//FUNCTION DECLARATIONS
extern const char *keywords[NUM_OF_KEYWORDS];

extern FILE *input_file;

Token getToken();

Token process_Number_Token(char firstchar, FILE *input_file);

Token process_String_Token(FILE *input_file);

Token process_ID_Token(char firstchar, FILE *input_file);

Token process_Char_Arr(FILE *input_file);

Token process_Import(FILE *input_file);

Token process_Multiline_String_Token(FILE *input_file);

token_types is_next_token(FILE *file, Token *token, char expected_char, token_types type1, token_types type2);

int is_keyword(const char *str, Token *token);

int init_value(char **buffer, int initial_size);

int realloc_value(char **buffer, int *buffer_size);

void add_value_pointer(char *value);

void free_all_values();

void reset_scanner();

#endif

/* END OF FILE scanner.h */