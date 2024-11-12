#ifndef ERROR_H
#define ERROR_H

#include "symtable.h"
#include "scanner.h"

#define ERR_LEX           1  // chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
#define ERR_SYNTAX        2  // chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.)
#define ERR_SEM_UNDEF     3  // sémantická chyba v programu – nedefinovaná funkce či proměnná
#define ERR_SEM_FUN       4  // sémantická chyba v programu – špatný počet/typ parametrů u volání funkce; špatný typ či nepovolené zahození návratové hodnoty z funkce
#define ERR_SEM_REDEF     5  // sémantická chyba v programu – redefinice proměnné nebo funkce; přiřazení do nemodifikovatelné proměnné
#define ERR_SEM_RETURN    6  // sémantická chyba v programu – chybějící/přebývající výraz v příkazu návratu z funkce
#define ERR_SEM_TYPE      7  // sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech; nekompatibilní typ výrazu (např. při přiřazení)
#define ERR_SEM_INHERIT   8  // sémantická chyba odvození typu – typ proměnné není uveden a nelze odvodit od použitého výrazu 
#define ERR_SEM_UNUSED    9  // sémantická chyba nevyužité proměnné v jejím rozsahu platnosti; modifikovatelná proměnná bez možnosti změny po její inicializaci
#define ERR_SEM_ELSE      10 // ostatní sémantické chyby

#define ERR_INTERNAL      99 // interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.)

/** this is and example error message you put into ERROR after errNum
 * "Wrong ID in prologue section.\nExpected: \"ifj\"\nGot: %s\n", currentToken.value
 */

// ADD CLEANUP
#define ERROR(errNum, ...) do { \
    fprintf(stderr, "ERROR NUMBER %d at line %d: \n", errNum, currentToken.line); \
    fprintf(stderr, __VA_ARGS__); \
    exit(errNum); \
} while (0)


/*
controls all allocations in program and deletes everything
*/
void delete_all_allocated();

#endif //ERROR_H