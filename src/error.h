/**
 *         Implementace překladače imperativního jazyka IFJ24.
 * 
 * @file   error.h
 * 
 * @brief  File includes definitions of error codes and macros to trigger errors,
 *         while deallocating allocated memory.
 * 
 * @author xnovakf00 Filip Novák
 * @date   21.11.2024
*/


#ifndef ERROR_H
#define ERROR_H

#include "symtable.h"
#include "scanner.h"

#define ERR_LEX           1  // Error in the program during lexical analysis (invalid structure of the current lexeme)
#define ERR_SYNTAX        2  // Error in the program during syntax analysis (incorrect program syntax, missing header, etc.)
#define ERR_SEM_UNDEF     3  // Semantic error in the program – undefined function or variable
#define ERR_SEM_FUN       4  // Semantic error in the program – incorrect number/type of parameters in a function call; invalid or unauthorized discarding of a function's return value
#define ERR_SEM_REDEF     5  // Semantic error in the program – redefinition of a variable or function; assignment to an immutable variable
#define ERR_SEM_RETURN    6  // Semantic error in the program – missing/extraneous expression in a return statement
#define ERR_SEM_TYPE      7  // Semantic error in type compatibility in arithmetic, string, and relational expressions; incompatible expression type (e.g., during assignment)
#define ERR_SEM_INHERIT   8  // Semantic error in type inference – variable type is not specified and cannot be inferred from the used expression
#define ERR_SEM_UNUSED    9  // Semantic error of unused variable within its scope of validity; mutable variable without a possibility of change after its initialization
#define ERR_SEM_ELSE      10 // Other semantic errors

#define ERR_INTERNAL      99 // Internal error of compiler

/** this is and example error message you put into ERROR after errNum
 * "Wrong ID in prologue section.\nExpected: \"ifj\"\nGot: %s\n", currentToken.value
 */

#define ERROR(errNum, ...) do { \
    fprintf(stderr, "\nERROR NUMBER %d at line %d column %d: ", errNum, currentToken.line, currentToken.column); \
    fprintf(stderr, __VA_ARGS__); \
    delete_all_allocated(); \
    exit(errNum); \
} while (0)

#define ERRORLEX(errNum, ...) do{\
    fprintf(stderr, "\nERROR NUMBER %d: ", errNum); \
    fprintf(stderr, __VA_ARGS__); \
    free_all_values(); \
    exit(errNum); \
} while (0)

void delete_all_allocated();

#endif //ERROR_H

/* EOF error.h */