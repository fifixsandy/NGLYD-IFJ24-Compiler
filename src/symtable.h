 
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
    symNode *root;
    int nodeCnt;
 }symtable;
 