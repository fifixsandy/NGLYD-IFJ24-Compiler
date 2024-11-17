#include "ast.h"
#include "error.h"
#include "parser.h"


astNode *createAstNode(){
    astNode *new = malloc(sizeof(astNode));
    if(new == NULL){ 
        ERROR(ERR_INTERNAL,"Error occured while allocating memory.");
    }

    new->next   = NULL;
    new->parent = NULL;
    new->type   = AST_INVALID;

    return new;
}

void createWhileNode(astNode *dest, bool withNull, char *id_without_null, astNode *cond, astNode *body, symtable *symtableW, astNode *parent){

    astWhile newWhile = {
        .body = body,
        .condition = cond,
        .withNull = withNull,
        .id_without_null = id_without_null,
        .symtableWhile = symtableW
    };

    dest->type = AST_NODE_WHILE;
    dest->nodeRep.whileNode =  newWhile;
    dest->parent = parent;
    dest->next = NULL;

    return;
}

void createIfElseNode(astNode *dest, astNode *cond, astNode *ifPart, astNode *elsePart, bool withNull, astNode *parent){
    astIfElse newIfElse = {
        .condition = cond,
        .ifPart = ifPart,
        .elsePart = elsePart,
        .withNull = withNull
    };

    dest->type = AST_NODE_IFELSE;
    dest->nodeRep.ifElseNode = newIfElse;
    dest->parent = parent;
    dest->next = NULL;
}

void createIfNode(astNode *dest, char *id_without_null, symtable *symtable, astNode *body, astNode *parent){
    astIf newIf = {
        .body = body,
        .id_without_null = id_without_null,
        .symtableIf = symtable
    };

    dest->next = NULL;
    dest->type = AST_NODE_IF;
    dest->parent = parent;
    dest->nodeRep.ifNode = newIf;
}

void createElseNode(astNode *dest, symtable *symtableElse, astNode *body, astNode *parent) {
    astElse newElse = {
        .symtableElse = symtableElse,
        .body = body
    };

    dest->parent = parent;
    dest->type = AST_NODE_ELSE;
    dest->next = NULL;
    dest->nodeRep.elseNode = newElse;
}

void createAssignNode(astNode *dest, char *id, astNode *expression, astNode *parent, dataType dataT) {
    astAssign newAssign = {
        .id = id,
        .expression = expression,
        .dataT = dataT
    };

    dest->parent = parent;
    dest->type = AST_NODE_ASSIGN;
    dest->next = NULL;
    dest->nodeRep.assignNode = newAssign;
}

void createDefVarNode(astNode *dest, char *id, astNode *initExpr, symNode *symtableEntry, astNode *parent) {
    astDefVar newDefVar = {
        .id = id,
        .initExpr = initExpr,
        .symtableEntry = symtableEntry
    };

    dest->next = NULL;
    dest->type = AST_NODE_DEFVAR;
    dest->parent = parent;
    dest->nodeRep.defVarNode = newDefVar;
}

void createDefFuncNode(astNode *dest, char *id, symtable *symtableFun, astNode *body, astNode *parent, char **paramNames, int paramNum, dataType returnType) {
    astDefFunc newDefFunc = {
        .id          = id,
        .symtableFun = symtableFun,
        .body        = body,
        .paramNames  = paramNames,
        .paramNum    = paramNum,
        .returnType  = returnType
    };

    dest->next = NULL;
    dest->type = AST_NODE_DEFFUNC;
    dest->parent = parent;
    dest->nodeRep.defFuncNode = newDefFunc;
}

void createReturnNode(astNode *dest, astNode *returnExp, dataType returnType, astNode *parent, bool inMain) {
    astReturn newReturn = {
        .returnExp = returnExp,
        .returnType = returnType,
        .inMain    = inMain
    };

    dest->next = NULL;
    dest->type = AST_NODE_RETURN;
    dest->parent = parent;
    dest->nodeRep.returnNode = newReturn;
}

void createBinOpNode(astNode *dest, symbol_number op, astNode *left, astNode *right, dataType dataT, astNode *parent) {
    astBinOp newBinOp = {
        .op = op,
        .left = left,
        .right = right,
        .dataT = dataT
    };

    left->parent = dest;
    right->parent = dest;    
    dest->next = NULL;
    dest->type = AST_NODE_BINOP;
    dest->parent = parent;
    dest->nodeRep.binOpNode = newBinOp;
}

void createLiteralNode(astNode *dest, dataType dataT, void *value, astNode *parent) {
    astLiteral newLiteral = {
        .dataT = dataT
    };



    if (dataT == i32) {
        newLiteral.value.intData = *(int *)(value); // TODO change data type
    } 
    else if (dataT == f64) {
        newLiteral.value.floatData = *(float *)(value); // MIKULAŠ NAHE FOTKY TERAZ !!!!!
    } 
    else if (dataT == u8) {
        newLiteral.value.charData = value;
    }

    dest->next = NULL;
    dest->type = AST_NODE_LITERAL;
    dest->parent = parent;
    dest->nodeRep.literalNode = newLiteral;
}

void createVarNode(astNode *dest, char *id, dataType dataT, symNode *symtableEntry, astNode *parent) {
    astVar newVar = {
        .dataT = dataT,
        .id = id,
        .symtableEntry = symtableEntry,
        .isNullable = symtableEntry->data.data.vData.isNullable
    };

    dest->next = NULL;
    dest->type = AST_NODE_VAR;
    dest->parent = parent;
    dest->nodeRep.varNode = newVar;
    symtableEntry->data.used = true; // set for semantic check
}

void createFuncCallNode(astNode *dest, char *id, dataType retType, bool builtin, symNode *symtableEntry, astNode *parent, astNode **paramExpr, int paramNum, bool isNullable) {
    astFuncCall newFuncCall = {
        .retType         = retType,
        .nullableRetType = isNullable,
        .id              = id,
        .symtableEntry   = symtableEntry,
        .builtin         = builtin,
        .paramExpr       = paramExpr,
        .paramNum        = paramNum
    };
    dest->next = NULL;
    dest->type = AST_NODE_FUNC_CALL;
    dest->parent = parent;
    dest->nodeRep.funcCallNode = newFuncCall;
    symtableEntry->data.used = true; // set for semantic check
}

void createUnusedNode(astNode *dest, astNode *expr, astNode *parent){

    astUnused newUnused = {
        .expr = expr
    };

    dest->next               = NULL;
    dest->type               = AST_UNUSED;
    dest->parent             = parent;
    dest->nodeRep.unusedNode = newUnused;

}

void createExpressionNode(astNode *dest, dataType type, astNode *exprRoot, bool isNullable, bool DuringCompile){
    astExpr newExpr = {
        .dataT = type,
        .exprTree = exprRoot,
        .isNullable = isNullable,
        .knownDuringCompile = DuringCompile
    };

    dest->next               = NULL;
    dest->type               = AST_NODE_EXPR;
    dest->parent             = NULL;
    dest->nodeRep.exprNode   = newExpr;
}

astNode *createRootNode(){
    astNode *new = createAstNode();
    new->type = AST_NODE_ROOT;
    new->next = NULL;
    return new;
}

void addNext(astNode *prev, astNode *next){
    prev->next = next;
}

/**
 * @brief           Connects node to the end of (sub)block.
 * 
 * @param toAdd     Pointer to the node to connect.
 * @param blockRoot Root of the block to be connected to.
 * 
 */
void connectToBlock(astNode *toAdd, astNode *blockRoot){
    
    astNode *currBottom = blockRoot;

    while(currBottom->next != NULL){ // find where to connect
        currBottom = currBottom->next;
    }

    currBottom->next = toAdd;

}

void freeASTNode(astNode *node){
    if(node == NULL){return;}
    switch(node->type){
        case AST_NODE_WHILE:

            freeASTNode(node->nodeRep.whileNode.condition);
            freeASTNode(node->nodeRep.whileNode.body);
            deleteSymtable(node->nodeRep.whileNode.symtableWhile);
            
            break;
        case AST_NODE_IFELSE:
            freeASTNode(node->nodeRep.ifElseNode.condition);
            freeASTNode(node->nodeRep.ifElseNode.ifPart);
            freeASTNode(node->nodeRep.ifElseNode.elsePart);
            break;
        case AST_NODE_IF:
            deleteSymtable(node->nodeRep.ifNode.symtableIf);
            freeASTNode(node->nodeRep.ifNode.body);
            break;
        case AST_NODE_ELSE:
            deleteSymtable(node->nodeRep.elseNode.symtableElse);
            freeASTNode(node->nodeRep.elseNode.body);
            break;
        case AST_NODE_ASSIGN:
            freeASTNode(node->nodeRep.assignNode.expression);
            break;
        case AST_NODE_DEFFUNC:
            free(node->nodeRep.defFuncNode.paramNames);
            freeASTNode(node->nodeRep.defFuncNode.body);
            deleteSymtable(node->nodeRep.defFuncNode.symtableFun);
            break;
        case AST_NODE_RETURN:
            freeASTNode(node->nodeRep.returnNode.returnExp);
            break;
        case AST_NODE_EXPR:
            freeASTNode(node->nodeRep.exprNode.exprTree);
            break;
        case AST_NODE_BINOP:
            freeASTNode(node->nodeRep.binOpNode.left);
            freeASTNode(node->nodeRep.binOpNode.right);
            break;
        case AST_NODE_DEFVAR:
            freeASTNode(node->nodeRep.defVarNode.initExpr);
            break;
        case AST_NODE_FUNC_CALL:
            for(int i = 0; i < node->nodeRep.funcCallNode.paramNum; i++){
                freeASTNode(node->nodeRep.funcCallNode.paramExpr[i]);
            }
            free(node->nodeRep.funcCallNode.paramExpr);
            break;
        case AST_NODE_ROOT:
            freeASTNode(node->next);
            node->next = NULL;
            break;

        default:
            break;
    }

    freeASTNode(node->next);
    node->next = NULL;
    free(node);
}