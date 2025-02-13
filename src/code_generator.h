/**
 *         Implementation of IFJ24 imperative language compiler.
 * 
 * @file   code_generator.h
 * @brief  Header file for managing the code generation functions for buffer manipulation and operations.
 * 
 * 
 * @author xskovaj00
 * @date   7.11.2024
 */

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "code_buffer.h"
#include "ast.h"


typedef enum {INT, FLOAT, STRING} Types;

typedef enum {WHILE_COND, WHILE_END, IF_ELSE, IF_END} LABEL_TYPES;


/**
 * @struct Defined_vars
 * @brief Structure to manage a list of defined variable names.
 *
 * This structure contains a dynamic array of strings (names) and a count of
 * the number of variables in the array.
 */
typedef struct{
    char ** names;
    int num_of_vars;
} Defined_vars;



//#define DEBUG
#ifdef DEBUG
    #ifndef DEBPRINT
    #define DEBPRINT(...) \
        fprintf(stderr, "D: %s, %d: ", __func__ , __LINE__); \
        fprintf(stderr, __VA_ARGS__);
    #endif
#else
    #ifndef DEBPRINT 
    #define DEBPRINT(...) 
    #endif
#endif

/**
 * @brief The header section of the code generation
 */
#define HEADER                                          \
    ".IFJcode24\n"                                      \
    "DEFVAR GF@retval\n"                                \
    "JUMP $main\n\n\n"

/**
 * @brief Built-in function for substring generation
 */
#define SUBSTRING                                       \
    "#Build-in fucntion ifj.substring\n"                \
    "LABEL $$substring\n"                               \
    "PUSHFRAME\n"                                       \
    "CREATEFRAME\n"                                     \
    "MOVE GF@retval nil@nil\n"                          \
    "DEFVAR TF@char\n"                                  \
    "DEFVAR TF@len_str\n"                               \
    "DEFVAR TF@end_cond\n"                              \
    "STRLEN TF@len_str LF@%0\n"                        \
                                                        \
    "GT TF@end_cond LF@%1 TF@len_str\n"                \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "EQ TF@end_cond LF@%1 TF@len_str\n"                \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "GT TF@end_cond LF@%2 TF@len_str\n"                \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
                                                        \
    "LT TF@end_cond LF@%1 int@0\n"                     \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "LT TF@end_cond LF@%2 int@0\n"                     \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "GT TF@end_cond LF@%1 LF@%2\n"                    \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
                                                        \
    "MOVE GF@retval string@\n"                          \
    "DEFVAR TF@i\n"                                     \
    "MOVE TF@i LF@%1\n"                                \
    "LABEL $$substring-while-start-1\n"                 \
    "EQ TF@end_cond TF@i LF@%2\n"                      \
    "JUMPIFEQ $$substring-while-end-1 TF@i LF@%2\n"    \
    "GETCHAR TF@char LF@%0 TF@i\n"                     \
    "CONCAT GF@retval GF@retval TF@char\n"              \
    "ADD TF@i TF@i int@1\n"                             \
    "JUMP $$substring-while-start-1\n"                  \
    "LABEL $$substring-while-end-1\n"                   \
    "LABEL $$substring-end\n"                           \
    "POPFRAME\n"                                        \
    "RETURN\n"

/**
 * @brief Built-in function for string comparison (strcmp)
 */
#define STRCMP                                           \
    "LABEL $$strcmp\n"                                   \
    "PUSHFRAME\n"                                        \
    "CREATEFRAME\n"                                      \
    "DEFVAR TF@len_str1\n"                               \
    "DEFVAR TF@len_str2\n"                               \
    "DEFVAR TF@min_len\n"                                \
    "DEFVAR TF@bool_tmp\n"                               \
                                                         \
    "MOVE GF@retval int@0\n"                             \
                                                         \
    "STRLEN TF@len_str1 LF@%0\n"                        \
    "STRLEN TF@len_str2 LF@%1\n"                        \
    "GT TF@bool_tmp TF@len_str1 TF@len_str2\n"           \
    "JUMPIFEQ $$strcmp-if-else-1 TF@bool_tmp bool@false\n"\
    "MOVE TF@min_len TF@len_str2\n"                      \
    "JUMP $$strcmp-if-end-1\n"                           \
                                                         \
    "LABEL $$strcmp-if-else-1\n"                         \
                                                         \
    "MOVE TF@min_len TF@len_str1\n"                      \
    "JUMPIFEQ $$strcmp-end TF@len_str2 int@0\n"          \
    "LABEL $$strcmp-if-end-1\n"                          \
                                                         \
    "DEFVAR TF@ascii1\n"                                 \
    "DEFVAR TF@ascii2\n"                                 \
    "DEFVAR TF@i\n"                                      \
    "MOVE TF@i int@0\n"                                  \
                                                         \
    "LABEL $$strcmp-while-start-1\n"                     \
    "JUMPIFEQ $$strcmp-while-end-1 TF@min_len TF@i\n"    \
                                                         \
    "STRI2INT TF@ascii1 LF@%0 TF@i\n"                   \
    "STRI2INT TF@ascii2 LF@%1 TF@i\n"                   \
                                                         \
    "JUMPIFEQ $$strcmp-if-else-2 TF@ascii1 TF@ascii2\n"  \
                                                         \
    "GT TF@bool_tmp TF@ascii1 TF@ascii2\n"               \
    "JUMPIFEQ $$strcmp-if-else-3 TF@bool_tmp bool@true\n"\
    "MOVE GF@retval int@-1\n"                            \
    "JUMP $$strcmp-if-end-3\n"                           \
                                                         \
    "LABEL $$strcmp-if-else-3\n"                         \
    "MOVE GF@retval int@1\n"                             \
    "LABEL $$strcmp-if-end-3\n"                          \
                                                         \
    "JUMP $$strcmp-end\n"                                \
    "LABEL $$strcmp-if-else-2\n"                         \
                                                         \
    "ADD TF@i TF@i int@1\n"                              \
    "JUMP $$strcmp-while-start-1\n"                      \
    "LABEL $$strcmp-while-end-1\n"                       \
                                                         \
    "JUMPIFEQ $$strcmp-end TF@len_str1 TF@len_str2\n"    \
    "GT TF@bool_tmp TF@len_str1 TF@len_str2\n"           \
    "JUMPIFEQ $$strcmp-if-else-4 TF@bool_tmp bool@false\n"\
    "MOVE GF@retval int@1\n"                             \
    "JUMP $$strcmp-if-end-4\n"                           \
                                                         \
    "LABEL $$strcmp-if-else-4\n"                         \
    "MOVE GF@retval int@-1\n"                            \
    "LABEL $$strcmp-if-end-4\n"                          \
                                                         \
    "LABEL $$strcmp-end\n"                               \
    "POPFRAME\n"                                         \
    "RETURN\n"

/**
 * @brief Built-in function for string ordinal value
 */
#define ORD                                              \
    "LABEL $$ord\n"                                      \
    "PUSHFRAME\n"                                        \
    "CREATEFRAME\n"                                      \
    "DEFVAR TF@len_str\n"                                \
    "DEFVAR TF@bool_tmp\n"                               \
                                                         \
    "MOVE GF@retval int@0\n"                             \
                                                         \
    "LT TF@bool_tmp LF@%1 int@0\n"                      \
    "JUMPIFEQ $$ord-end TF@bool_tmp bool@true\n"         \
    "JUMPIFEQ $$ord-end LF@%0 string@\n"                \
                                                         \
    "STRLEN TF@len_str LF@%0\n"                         \
    "SUB TF@len_str TF@len_str int@1\n"                  \
    "GT TF@bool_tmp LF@%1 TF@len_str\n"                 \
    "JUMPIFEQ $$ord-end TF@bool_tmp bool@true\n"         \
                                                         \
    "STRI2INT GF@retval LF@%0 LF@%1\n"                 \
                                                         \
    "LABEL $$ord-end\n"                                  \
    "POPFRAME\n"                                         \
    "RETURN\n"


bool add_int(int val);
bool add_null();
bool add_float(double val);
bool add_string(char *str);
bool add_read(char *var, Types type);
bool add_write(char *term);
bool add_i2f(char *var, char *symb);
bool add_f2i(char *var, char *symb);
bool add_str_len(char *var, char *symb);
bool add_str_concat(char *var, char *symb1, char *symb2);
bool add_chr(char *var, char *symb);
bool generate_build_in_functions();
bool generate_header();
void inint_def_vars(Defined_vars *vars);
bool def_var(Defined_vars *TF_vars, char *var_tmp, bool arg);
bool is_in_def_vars(Defined_vars *vars, char *name);
void delete_def_vars(Defined_vars *vars);
void generate_label(char *label, LABEL_TYPES type, int number);
bool code_generator(astNode *ast,  Defined_vars *TF_vars);
bool generate_code(astNode *ast);


#endif //CODE_GENERATOR_H

/* EOF code_generator.h */
