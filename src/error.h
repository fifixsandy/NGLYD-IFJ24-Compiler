#ifndef ERROR_H
#define ERROR_H

#include "symtable.h"
#include "scanner.h"

#define ERR_LEX           1
#define ERR_SYNTAX        2
#define ERR_SEM_UNDEF     3
#define ERR_SEM_FUN       4
#define ERR_SEM_REDEF     5
#define ERR_SEM_RETURN    6
#define ERR_SEM_TYPE      7
#define ERR_SEM_INHERIT   8
#define ERR_SEM_UNUSED    9
#define ERR_SEM_ELSE      10

#define ERR_INTERNAL      99

/** this is and example error message you put into ERROR after errNum
 * "L: %d C: %d \nWrong ID in prologue section.\nExpected: \"ifj\"\nGot: %s\n", 
 *  currentToken.line, currentToken.column, currentToken.value
 */

// ADD CLEANUP
#define ERROR(errNum, ...) do { \
    fprintf(stderr, "ERROR NUMBER %d: ", errNum); \
    fprintf(stderr, __VA_ARGS__); \
    exit(errNum); \
} while (0)


/*
controls all allocations in program and deletes everything
*/
void delete_all_allocated();

#endif //ERROR_H