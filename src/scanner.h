#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "error.h"

#define NUM_OF_KEYWORDS 13

extern int Line_Number;

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

#define FIRST_KEYWORD tokentype_kw_const

typedef struct {
    token_types type;
    int line;
    int column;
    char* value;
} Token;

typedef struct TokenValues {
    char *value;
    struct TokenValues *next;
} TokenValues;


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

#endif