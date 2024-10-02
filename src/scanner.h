#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

typedef enum {
    tokentype_EOL,
    tokentype_EOF,

    tokentype_vbar,
    tokentype_semicolon,
    tokentype_notequal,

    tokentype_lbracket,
    tokentype_rbracket,

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
    tokentype_pseudovar,
    tokentype_zeroint,
    tokentype_int,
    tokentype_float,
    tokentype_exponentialnum,

}token_types;

typedef struct {
    token_types type;
    int line;
    int column;
    char* value;
} Token;

#endif