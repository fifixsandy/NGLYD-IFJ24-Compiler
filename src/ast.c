#include "ast.h"



astNode *createAstNode(){
    astNode *new = malloc(sizeof(astNode));
    if(new == NULL){ // TODO ERROR

    }

    new->next   = NULL;
    new->parent = NULL;
    new->type   = AST_INVALID;

    return new;
}

astNode *createWhileNode(bool withNull, char *id_without_null, astNode *cond, astNode *body, symtable *symtableW, astNode *parent){

    astNode *new = createAstNode();

    astWhile newWhile = {
        .body = body,
        .condition = cond,
        .withNull = withNull,
        .id_without_null = id_without_null,
        .symtableWhile = symtableW
    };

    new->type = AST_NODE_WHILE;
    new->nodeRep.whileNode =  newWhile;
    new->parent = parent;
    new->next = NULL;

    return new;
}

astNode *createIfElseNode(astNode *cond, astNode *ifPart, astNode *elsePart, bool withNull, astNode *parent){
    astNode *new = createAstNode();

    astIfElse newIfElse = {
        .condition = cond,
        .ifPart = ifPart,
        .elsePart = elsePart,
        .withNull = withNull
    };

    new->parent = parent;
    new->type = AST_NODE_IFELSE;
    new->next = NULL;
    new->nodeRep.ifElseNode = newIfElse;

    return new;
}

astNode *createIfNode(char *id_without_null, symtable *symtable, astNode *body, astNode *parent){
    astNode *new = createAstNode();

    astIf newIf = {
        .body = body,
        .id_without_null = id_without_null,
        .symtableIf = symtable
        
    };

    new->next = NULL;
    new->type = AST_NODE_IF;
    new->parent = parent;
    new->nodeRep.ifNode = newIf;

    return new;
}

astNode *createElseNode(symtable *symtableIf, astNode *body, astNode *parent) {
    astNode *new = createAstNode();

    astElse newElse = {
        .symtableIf = symtableIf,
        .body = body
    };

    new->parent = parent;
    new->type = AST_NODE_ELSE;
    new->next = NULL;
    new->nodeRep.elseNode = newElse;

    return new;
}

astNode *createAssignNode(char *id, astNode *expression, astNode *parent, dataType dataT) {
    astNode *new = createAstNode();

    astAssign newAssign = {
        .id = id,
        .expression = expression,
        .dataT = dataT
    };

    new->parent = parent;
    new->type = AST_NODE_ASSIGN;
    new->next = NULL;
    new->nodeRep.assignNode = newAssign;

    return new;
}

astNode *createDefVarNode(char *id, astNode *initExpr, symtable *symtableEntry, astNode *parent) {
    astNode *new = createAstNode();

    astDefVar newDefVar = {
        .id = id,
        .initExpr = initExpr,
        .symtableEntry = symtableEntry
    };

    new->next = NULL;
    new->type = AST_NODE_DEFVAR;
    new->parent = parent;
    new->nodeRep.defVarNode = newDefVar;

    return new;
}

astNode *createDefFuncNode(char *id, int paramNum, dataType *paramTypes, char **paramNames, dataType returnType, symtable *symtableFun, astNode *body, astNode *parent) {
    astNode *new = createAstNode();

    astDefFunc newDefFunc = {
        .id = id,
        .paramNum = paramNum,
        .returnType = returnType,
        .symtableFun = symtableFun,
        .body = body
    };

    for(int i = 0; i < paramNum; i++) {
        newDefFunc.paramNames[i] = paramNames[i];
        newDefFunc.paramTypes[i] = paramTypes[i];
    }

    new->next = NULL;
    new->type = AST_NODE_DEFFUNC;
    new->parent = parent;
    new->nodeRep.defFuncNode = newDefFunc;

    return new;
}

astNode *createReturnNode(astNode *returnExp, dataType returnType, astNode *parent) {
    astNode *new = createAstNode();

    astReturn newReturn = {
        .returnExp = returnExp,
        .returnType = returnType
    };

    new->next = NULL;
    new->type = AST_NODE_RETURN;
    new->parent = parent;
    new->nodeRep.returnNode = newReturn;

    return new;
}

astNode *createBinOpNode(binOpType op, astNode *left, astNode *right, dataType dataT, astNode *parent) {
    astNode *new = createAstNode();

    astBinOp newBinOp = {
        .op = op,
        .left = left,
        .right = right,
        .dataT = dataT
    };

    new->next = NULL;
    new->type = AST_NODE_BINOP;
    new->parent = parent;
    new->nodeRep.binOpNode = newBinOp;

    return new;
}

astNode *createLiteralNode(dataType dataT, void *value, astNode *parent) {
    astNode *new = createAstNode();

    astLiteral newLiteral = {
        .dataT = dataT
    };

    if (dataT == i32) {
        newLiteral.value.intData = *(int *)value;
    } else if (dataT == f64) {
        newLiteral.value.floatData = *(float *)value;
    } else if (dataT == u8) {
        newLiteral.value.charData = (char *)value;
    }

    new->next = NULL;
    new->type = AST_NODE_LITERAL;
    new->parent = parent;
    new->nodeRep.literalNode = newLiteral;

    return new;
}


astNode *createVarNode(char *id, dataType dataT, symNode *symtableEntry, astNode *parent) {
    astNode *new = createAstNode();

    astVar newVar = {
        .dataT = dataT,
        .id = id,
        .symtableEntry = symtableEntry
    };

    new->next = NULL;
    new->type = AST_NODE_VAR;
    new->parent = parent;
    new->nodeRep.varNode = newVar;

    return new;
}


void addNext(astNode *prev, astNode *next){
    prev->next = next;
}