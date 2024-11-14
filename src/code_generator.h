/**
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

typedef struct{
    char ** names;
    int num_of_vars;
} Defined_vars;

/**
 * @brief The header section of the code generation
 */
#define HEADER                                          \
    ".IFJcode24\n"                                      \
    "DEFVAR GF@retval"

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
    "STRLEN TF@len_str LF@%%0\n"                        \
                                                        \
    "GT TF@end_cond LF@%%1 TF@len_str\n"                \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "EQ TF@end_cond LF@%%1 TF@len_str\n"                \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "GT TF@end_cond LF@%%2 TF@len_str\n"                \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
                                                        \
    "LT TF@end_cond LF@%%1 int@0\n"                     \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "LT TF@end_cond LF@%%2 int@0\n"                     \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
    "GT TF@end_cond LF@%%1 LF@%%2\n"                    \
    "JUMPIFEQ $$substring-end TF@end_cond bool@true\n"  \
                                                        \
    "MOVE GF@retval string@\n"                          \
    "DEFVAR TF@i\n"                                     \
    "MOVE TF@i LF@%%1\n"                                \
    "LABEL $$substring-while-start-1\n"                 \
    "EQ TF@end_cond TF@i LF@%%2\n"                      \
    "JUMPIFEQ $$substring-while-end-1 TF@i LF@%%2\n"    \
    "GETCHAR TF@char LF@%%0 TF@i\n"                     \
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
    "STRLEN TF@len_str1 LF@%%0\n"                        \
    "STRLEN TF@len_str2 LF@%%1\n"                        \
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
    "STRI2INT TF@ascii1 LF@%%0 TF@i\n"                   \
    "STRI2INT TF@ascii2 LF@%%1 TF@i\n"                   \
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
    "LT TF@bool_tmp LF@%%1 int@0\n"                      \
    "JUMPIFEQ $$ord-end TF@bool_tmp bool@true\n"         \
    "JUMPIFEQ $$ord-end LF@%%0 string@\n"                \
                                                         \
    "STRLEN TF@len_str LF@%%0\n"                         \
    "SUB TF@len_str TF@len_str int@1\n"                  \
    "GT TF@bool_tmp LF@%%1 TF@len_str\n"                 \
    "JUMPIFEQ $$ord-end TF@bool_tmp bool@true\n"         \
                                                         \
    "STRI2INT GF@retval LF@%%0 LF@%%1\n"                 \
                                                         \
    "LABEL $$ord-end\n"                                  \
    "POPFRAME\n"                                         \
    "RETURN\n"



/**
 * @brief Add a parameter to the code buffer.
 * @param param The parameter to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool add_param(char *param);

/**
 * @brief Add formatted int value to the code buffer.
 * @param val The int value to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool add_int(int val);

bool add_null();

/**
 * @brief Add formatted float value to the code buffer.
 * @param val The float value to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool add_float(int val);

/**
 * @brief Add formatted string value to the code buffer.
 * @param str The string value to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool add_string(char *str);

/**
 * @brief Add READ instruction to the code buffer.
 * @param var The variable which input will be assigned to.
 * @param type Type of variable being read (INT, FLOAT, STRING).
 * @return true if the operation was successful, false otherwise.
 */
bool add_read(char *var, Types type);

/**
 * @brief Add WRITE instruction to the code buffer.
 * @param term The term (int, float, string, or variable) to be printed.
 * @return true if the operation was successful, false otherwise.
 */
bool add_write(char *term);

/**
 * @brief Add INT2FLOAT instruction to the code buffer.
 * @param var The variable where the conversion result will be assigned.
 * @param symb The symbol (string or variable) from which the conversion is performed.
 * @return true if the operation was successful, false otherwise.
 */
bool add_i2f(char *var, char *symb);

/**
 * @brief Add FLOAT2INT instruction to the code buffer.
 * @param var The variable where the conversion result will be assigned.
 * @param symb The symbol (string or variable) from which the conversion is performed.
 * @return true if the operation was successful, false otherwise.
 */
bool add_f2i(char *var, char *symb);

/**
 * @brief Add STRLEN instruction to the code buffer.
 * @param var The variable where the result will be assigned.
 * @param symb The symbol (string or variable) whose length will be calculated.
 * @return true if the operation was successful, false otherwise.
 */
bool add_str_len(char *var, char *symb);

/**
 * @brief Add CONCAT instruction to the code buffer.
 * @param var The variable where the concatenation result will be assigned.
 * @param symb1 The first symbol to concatenate.
 * @param symb2 The second symbol to concatenate.
 * @return true if the operation was successful, false otherwise.
 */
bool add_str_concat(char *var, char *symb1, char *symb2);

/**
 * @brief Add INT2CHAR instruction to the code buffer.
 * @param var The variable where the character result will be assigned.
 * @param symb The symbol (integer or variable) to be converted to a character.
 * @return true if the operation was successful, false otherwise.
 */
bool add_chr(char *var, char *symb);

/**
 * @brief Generate built-in functions in the code buffer.
 * @return true if the operation was successful, false otherwise.
 */
bool generate_build_in_functions();

/**
 * @brief Generate the header for the code.
 * @return true if the operation was successful, false otherwise.
 */
bool generate_header();


void inint_def_vars(Defined_vars *vars);
bool add_to_def_vars(Defined_vars *vars, char *name);
bool is_in_def_vars(Defined_vars *vars, char *name);
void delete_def_vars(Defined_vars *vars);
void generate_label(char *label, LABEL_TYPES type, int number);
bool code_generator(astNode *ast,  Defined_vars *TF_vars);
bool generate_code(astNode *ast);
#endif
