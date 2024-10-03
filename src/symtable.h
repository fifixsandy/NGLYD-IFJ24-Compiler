#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


 typedef struct symData{
    // TODO add whether float/int/string...
    bool isConst;
    union{
        int   intData;
        float floatData;
        char *stringData;
    }value;
 }symData;
 
 
 typedef struct symNode{
    int key;
    int height;

    symData data;

    struct symNode *r;
    struct symNode *l;
 }symNode;
 
 
 typedef struct symtable{
    symNode *rootPtr;
    int      nodeCnt;
 }symtable;

symtable*  createSymtable();
void       initSymtable  (symtable *symtable);

symNode*   createSymNode (int key, symData data);
symNode*   insertSymNode (symNode *rootPtr, int key, symData data);
symNode*   deleteSymNode (symNode *rootPtr, int key);
symNode*   findSymNode   (symNode *rootPtr, int key);

symNode*   minSymNode    (symNode *rootPtr);
symNode*   rRotate       (symNode *node);
symNode*   lRotate       (symNode *node);
int        heightVal     (symNode *node);
int        balanceVal    (symNode *node);
int        max           (int a, int b);

 