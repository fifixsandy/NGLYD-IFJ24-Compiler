#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define NUM_OF_KEYWORDS 13

typedef enum {
    tokentype_EOL,
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
    tokentype_chararr,
    tokentype_comma,
    tokentype_lsbracket,
    tokentype_rsbracket,

    tokentype_invalid,

}token_types;

typedef struct {
    token_types type;
    int line;
    int column;
    char* value;
} Token;



extern const char *keywords[NUM_OF_KEYWORDS];

extern FILE *input_file;

Token getToken();

Token process_Number_Token(char firstchar, FILE *input_file);

Token process_String_Token(char firstchar, FILE *input_file);

Token process_ID_Token(char firstchar, FILE *input_file);

Token process_Char_Arr(FILE *input_file);

Token process_Import(FILE *input_file);

token_types is_next_token(FILE *file, Token *token, char expected_char, token_types type1, token_types type2);

int is_keyword(const char *str, Token *token);

int init_value(char **buffer, int initial_size);

int realloc_value(char **buffer, int *buffer_size);

#endif