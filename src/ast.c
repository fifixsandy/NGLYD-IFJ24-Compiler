/**
 *         Implementace překladače imperativního jazyka IFJ24.
 * 
 * @file   ast.c
 * 
 * @brief  This file includes function definitions for creating various asbstract syntactic tree
 *         (AST) nodes defined in ast.h as well as some functions for their management.
 * 
 *         File also includes functions for printing .dot visualisation of the AST in section
 *         Debug print.
 *         
 * @author xnovakf00 Filip Novák
 *         xfignam00 Matúš Fignár     
 * 
 * @todo delete parents
 * 
 * @date   24.11.2024
*/


#include "ast.h"
#include "error.h"
#include "parser.h"

/**
 * @brief   Allocates space for new astNode with invalid information.
 *        
 *          These incomplete nodes are used for example in building AST
 *          for expressions. Before expression parser is called, this node
 *          is allocated so expression parser can connect built nodes to it.
 * 
 * @warning Always call specific create*Node() on returned invalid node.
 * 
 * @return  Created astNode.
 */
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


/**
 * @brief Creates a new AST node of type WHILE.
 * 
 *        This function initializes a new "while" node with the specified condition, body,
 *        and additional info.
 * 
 * @param dest      Pointer to the astNode to initialize as a WHILE node.
 * @param withNull  Flag indicating if the condition is withNull.
 * @param id_without_null ID of the constant without null.
 * @param cond      astNode of condition expression (usually astExpr).
 * @param body      astNode of body of while (root)
 * @param symtableW Pointer to the symtable of while.
 */
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

/**
 * @brief Creates a new AST node of type IFELSE.
 * 
 *        This function initializes a new "if-else" node with the condition,
 *        ifPart (body of the "if" block), elsePart (body of the "else" block), and additional info.
 * 
 * @param dest     Pointer to the astNode to initialize as an IFELSE node.
 * @param cond     astNode representing the condition expression (usually astExpr).
 * @param ifPart   astNode representing the body of the "if" block.
 * @param elsePart astNode representing the body of the "else" block.
 * @param withNull Flag indicating if the condition is withNull.
 */
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

/**
 * @brief Creates a new AST node for an IF statement.
 * 
 *        This function initializes a new "if" node with the specified body and other information.
 * 
 * @param dest            Pointer to the astNode to initialize as an IF node.
 * @param id_without_null ID of the constant without null.
 * @param symtable        Pointer to the symtable of the if statement.
 * @param body            astNode of the body of the if statement (root).
 */

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


/**
 * @brief Creates a new AST node for an ELSE statement.
 * 
 *        This function initializes a new "else" node with the specified body and other details.
 * 
 * @param dest          Pointer to the astNode to initialize as an ELSE node.
 * @param symtableElse  Pointer to the symtable for the else block.
 * @param body          astNode of the body of the else statement (root).
 */

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

/**
 * @brief Creates a new AST node for an assignment.
 * 
 *        This function initializes a new "assignment" node with the given expression and identifier.
 * 
 * @param dest        Pointer to the astNode to initialize as an ASSIGN node.
 * @param id          Identifier of the variable being assigned.
 * @param expression  astNode representing the expression to be assigned.
 * @param dataT       The data type of the assignment.
 */
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


/**
 * @brief Creates a new AST node for variable definition.
 * 
 *        This function initializes a new "define variable" node with the initialization expression.
 * 
 * @param dest          Pointer to the astNode to initialize as a DEFVAR node.
 * @param id            Identifier of the variable being defined.
 * @param initExpr      Expression used to initialize the variable.
 * @param symtableEntry Symbol table entry for the variable.
 */
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


/**
 * @brief Creates a new AST node for function definition.
 * 
 *        This function initializes a new "function definition" node with the function's details.
 * 
 * @param dest          Pointer to the astNode to initialize as a DEFFUNC node.
 * @param id            Identifier of the function.
 * @param symtableFun   Pointer to the symtable for the function.
 * @param body          astNode of the body of the function (root).
 * @param paramNames    Array of parameter names for the function.
 * @param paramNum      Number of parameters for the function.
 * @param returnType    The return type of the function.
 * @param nullable      Whether the function return type can be nullable.
 */
void createDefFuncNode(astNode *dest, char *id, symtable *symtableFun, astNode *body, astNode *parent, char **paramNames, int paramNum, dataType returnType, bool nullable) {
    astDefFunc newDefFunc = {
        .id          = id,
        .symtableFun = symtableFun,
        .body        = body,
        .paramNames  = paramNames,
        .paramNum    = paramNum,
        .returnType  = returnType,
        .nullable    = nullable
    };

    dest->next = NULL;
    dest->type = AST_NODE_DEFFUNC;
    dest->parent = parent;
    dest->nodeRep.defFuncNode = newDefFunc;
}


/**
 * @brief Creates a new AST node for a return statement.
 * 
 *        This function initializes a new "return" node with the return expression and return type.
 * 
 * @param dest          Pointer to the astNode to initialize as a RETURN node.
 * @param returnExp     Expression to be returned.
 * @param returnType    The return type of the expression.
 * @param inMain        Flag if the return is inside the main function.
 */
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


/**
 * @brief Creates a new AST node for a binary operation.
 * 
 *        This function initializes a new "binary operation" 
 *        node with the left and right operands and the operation type.
 * 
 * @param dest   Pointer to the astNode to initialize as a BINOP node.
 * @param op     Symbol for the binary operator.
 * @param left   Left operand of the operation.
 * @param right  Right operand of the operation.
 * @param dataT  The data type of the result of the operation.
 */
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


/**
 * @brief Creates a new AST node for a literal value.
 * 
 *        This function initializes a new "literal" node with the specified value and data type.
 * 
 * @param dest  Pointer to the astNode to initialize as a LITERAL node.
 * @param dataT The data type of the literal value.
 * @param value The value of the literal.
 */
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
    else if(dataT == string){
        newLiteral.value.charData = value;
    }

    dest->next = NULL;
    dest->type = AST_NODE_LITERAL;
    dest->parent = parent;
    dest->nodeRep.literalNode = newLiteral;
}

/**
 * @brief Creates a new AST node for a variable.
 * 
 *        This function initializes a new "variable" node with the variable identifier and data type.
 * 
 * @param dest          Pointer to the astNode to initialize as a VAR node.
 * @param id            Identifier of the variable.
 * @param dataT         The data type of the variable.
 * @param symtableEntry Symbol table entry for the variable.
 */
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

/**
 * @brief Creates a new AST node for a function call.
 * 
 *        This function initializes a new "function call" node with the function's details and parameters.
 * 
 * @param dest          Pointer to the astNode to initialize as a FUNC_CALL node.
 * @param id            Identifier of the function.
 * @param retType       The return type of the function.
 * @param builtin       Flag indicating if the function is a built-in function.
 * @param symtableEntry Symbol table entry for the function.
 * @param paramExpr     Array of expressions for the function parameters.
 * @param paramNum      Number of parameters.
 * @param isNullable    Whether the return type is nullable.
 */
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

/**
 * @brief Creates a new AST node for an unused expression.
 * 
 *        This function initializes a new "unused expression" node.
 * 
 * @param dest Pointer to the astNode to initialize as an UNUSED node.
 * @param expr Expression that is unused.
 */
void createUnusedNode(astNode *dest, astNode *expr, astNode *parent){

    astUnused newUnused = {
        .expr = expr
    };

    dest->next               = NULL;
    dest->type               = AST_UNUSED;
    dest->parent             = parent;
    dest->nodeRep.unusedNode = newUnused;

}

/**
 * @brief Creates a new AST node for an expression.
 * 
 *        This function initializes a new "expression" node with the type, 
 *        expression tree, and whether it is nullable.
 * 
 * @param dest          Pointer to the astNode to initialize as an EXPR node.
 * @param type          The data type of the expression.
 * @param exprRoot      Root of the expression tree.
 * @param isNullable    Flag indicating if the expression is nullable.
 * @param duringCompile Flag indicating if the expression is known during compile time.
 */
void createExpressionNode(astNode *dest, dataType type, astNode *exprRoot, bool isNullable, bool duringCompile){
    astExpr newExpr = {
        .dataT = type,
        .exprTree = exprRoot,
        .isNullable = isNullable,
        .knownDuringCompile = duringCompile
    };

    dest->next               = NULL;
    dest->type               = AST_NODE_EXPR;
    dest->parent             = NULL;
    dest->nodeRep.exprNode   = newExpr;
}


/**
 * @brief  Creates a root AST node.
 * 
 *         This function initializes a new root node.
 *         Root node has only "next" valid. It is used
 *         as root of the whole AST, root of a body of function
 *         or body of while/ifElse statements.
 * 
 * @return Pointer to the created root AST node.
 */
astNode *createRootNode(){
    astNode *new = createAstNode();
    new->type = AST_NODE_ROOT;
    new->next = NULL;
    return new;
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

/**
 * @brief      Recursively frees nodes in AST.
 * 
 * @param node Node to free and recursively free its children.
 */
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
        case AST_UNUSED:
            freeASTNode(node->nodeRep.unusedNode.expr);
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



/************************************************************************************************************** 
                                         SECTION Debug print
                       These functions print out .dot representation of AST.
          They should SOLELY be used when debugging and should not be used in final product!
    Implementations of these functions were inspired by printing .dot format of BDDs from BuDDy library.
                       https://github.com/SSoelvsten/buddy/blob/main/src/bddio.c
*************************************************************************************************************/

/**
 * @brief Prints AST representation in .dot format.
 * 
 * @param file Pointer to an opened file to print to.
 * @param tree Pointer to the root of tree to print.
 */
void printASTree(FILE *file, astNode *tree){
        if(tree == NULL){
        return;
    }
    fprintf(file, "digraph G{\n");
    printASTNode(file, tree);
    fprintf(file, "}\n");
}

/**
 * @brief      Prints representation of a AST node.
 * 
 *             Firstly it prints the label of the node, than the edges.
 *             Subsequently based on the type of the node prints children 
 *             resursively as well.
 * 
 * @param file Pointer to an opened file to print to.
 * @param node Pointer to an astNode to print.
 */
void printASTNode(FILE *file, astNode *node){
    if(node == NULL){
        return;
    }
    printASTNodeLabel(file, node);
    printASTEdges(file, node);
    switch(node->type){
        case AST_NODE_DEFFUNC:
            printASTNode(file, node->nodeRep.defFuncNode.body);
            break;
        case AST_NODE_IFELSE:
            printASTNode(file, node->nodeRep.ifElseNode.ifPart);
            printASTNode(file, node->nodeRep.ifElseNode.elsePart);
            printASTNode(file, node->nodeRep.ifElseNode.condition);
            break;
        case AST_NODE_IF:            
            printASTNode(file, node->nodeRep.ifNode.body);
            break;
        case AST_NODE_ELSE:
            printASTNode(file, node->nodeRep.elseNode.body);
            break;
        case AST_NODE_WHILE:
            printASTNode(file, node->nodeRep.whileNode.body);
            printASTNode(file, node->nodeRep.whileNode.condition);
            break;
        case AST_NODE_EXPR:
            printASTNode(file, node->nodeRep.exprNode.exprTree);
            break;
        case AST_NODE_BINOP:
            printASTNode(file, node->nodeRep.binOpNode.left);
            printASTNode(file, node->nodeRep.binOpNode.right);
            break;
        case AST_NODE_ASSIGN:
            printASTNode(file, node->nodeRep.assignNode.expression);
            break;
        case AST_NODE_RETURN:
            printASTNode(file, node->nodeRep.returnNode.returnExp);
            break;
        case AST_NODE_FUNC_CALL:
            for(int param = 0; param < node->nodeRep.funcCallNode.paramNum; param++){
                printASTNode(file, node->nodeRep.funcCallNode.paramExpr[param]);
            }
            break;
        case AST_NODE_DEFVAR:
            printASTNode(file, node->nodeRep.defVarNode.initExpr);
            break;
        case AST_UNUSED:
            printASTNode(file, node->nodeRep.unusedNode.expr);
            break;
        default:
            break;
    }
    return;
}

/**
 * @brief      Prints the label of the node based on its type.
 * 
 *             Based on the type of the node also prints extra information
 *             that might be useful to visualise (eg. id_without_null for ifElse and while).
 * 
 * @param file Pointer to an opened file to print to.
 * @param node Pointer to an astNode to print label of. 
 */
void printASTNodeLabel(FILE *file, astNode *node){

    fprintf(file, "  node_%p [label=\"", (void *)node);
    switch (node->type) {
        case AST_NODE_WHILE:
            fprintf(file, "WHILE");
            printIdWithoutNull(file, node->nodeRep.whileNode.withNull, node->nodeRep.whileNode.id_without_null);
            break;
        case AST_NODE_IFELSE:
            fprintf(file, "IF_ELSE");
            printIdWithoutNull(file, node->nodeRep.ifElseNode.withNull, node->nodeRep.ifElseNode.ifPart->nodeRep.ifNode.id_without_null);
            break;
        case AST_NODE_IF:
            fprintf(file, "IF\", color=\"green\"];");
            return;
        case AST_NODE_ELSE:
            fprintf(file, "ELSE\", color=\"red\"];");
            return;
        case AST_NODE_ASSIGN:
            fprintf(file, "ASSIGN\\nID=%s", node->nodeRep.assignNode.id);
            break;
        case AST_NODE_EXPR:
            fprintf(file, "EXPR");
            break;
        case AST_NODE_BINOP:
            fprintf(file, "BINOP\\nOp=");
            printBinopType(file, node->nodeRep.binOpNode.op);
            break;
        case AST_NODE_LITERAL:
            fprintf(file, "LITERAL");
            printLiteralInfo(file, node->nodeRep.literalNode);
            break;
        case AST_NODE_VAR:
            fprintf(file, "VAR\\nID=%s", node->nodeRep.varNode.id);
            break;
        case AST_NODE_FUNC_CALL:
            fprintf(file, "FUNC_CALL\\nID=%s", node->nodeRep.funcCallNode.id);
            break;
        case AST_NODE_DEFVAR:
            fprintf(file, "DEFVAR\\nID=%s", node->nodeRep.defVarNode.id);
            printDefVarInfo(file, node->nodeRep.defVarNode);
            break;
        case AST_NODE_DEFFUNC:
            fprintf(file, "DEFFUNC\\nID=%s", node->nodeRep.defFuncNode.id);
            printDefFuncInfo(file, node->nodeRep.defFuncNode);
            fprintf(file, "\",shape=square, color=\"green\"];\n");
            return;
        case AST_NODE_RETURN:
            fprintf(file, "RETURN");
            break;
        case AST_NODE_ROOT:
            fprintf(file, "ROOT");
            break;
        case AST_UNUSED:
            fprintf(file, "_");
            break;
        default:
            fprintf(file, "UNKNOWN");
    }
    fprintf(file, "\"];\n");

}

/**
 * @brief Prints edges of the node with labels based on its type.
 * 
 * @param file Pointer to an opened file to print to.
 * @param node Pointer to an astNode to print edges of.
 */
void printASTEdges(FILE *file, astNode *node){
        switch (node->type) {
        case AST_NODE_WHILE:
            fprintf(file, "  node_%p -> node_%p [label=\"condition\"];\n", (void *)node, (void *)node->nodeRep.whileNode.condition);
            fprintf(file, "  node_%p -> node_%p [label=\"body\"];\n", (void *)node, (void *)node->nodeRep.whileNode.body);
            printASTNext(file, node);
            break;
        case AST_NODE_IFELSE:
            fprintf(file, "  node_%p -> node_%p [label=\"condition\"];\n", (void *)node, (void *)node->nodeRep.ifElseNode.condition);
            fprintf(file, "  node_%p -> node_%p [label=\"ifPart\", color=\"green\"];\n", (void *)node, (void *)node->nodeRep.ifElseNode.ifPart);
            fprintf(file, "  node_%p -> node_%p [label=\"elsePart\", color=\"red\"];\n", (void *)node, (void *)node->nodeRep.ifElseNode.elsePart);
            printASTNext(file, node);
            break;
        case AST_NODE_IF:
            fprintf(file, "  node_%p -> node_%p [label=\"body\"];\n", (void *)node, (void *)node->nodeRep.ifNode.body);
            break;
        case AST_NODE_ELSE:
            fprintf(file, "  node_%p -> node_%p [label=\"body\"];\n", (void *)node, (void *)node->nodeRep.elseNode.body);
            break;
        case AST_NODE_ASSIGN:
            fprintf(file, "  node_%p -> node_%p [label=\"expr\"];\n", (void *)node, (void *)node->nodeRep.assignNode.expression);
            printASTNext(file, node);
            break;
        case AST_NODE_EXPR:
            fprintf(file, "  node_%p -> node_%p [label=\"exprTree\"];\n", (void *)node, (void *)node->nodeRep.exprNode.exprTree);
            break;
        case AST_NODE_BINOP:
            fprintf(file, "  node_%p -> node_%p [label=\"left\"];\n", (void *)node, (void *)node->nodeRep.binOpNode.left);
            fprintf(file, "  node_%p -> node_%p [label=\"right\"];\n", (void *)node, (void *)node->nodeRep.binOpNode.right);
            break;
        case AST_NODE_LITERAL:
            break;
        case AST_NODE_VAR:
            break;
        case AST_NODE_FUNC_CALL:
            for(int param = 0; param < node->nodeRep.funcCallNode.paramNum; param++){
                fprintf(file, "  node_%p -> node_%p [label=\"param%d\"];\n", (void *)node, (void *)node->nodeRep.funcCallNode.paramExpr[param], param);
            }
            printASTNext(file, node);
            break;
        case AST_NODE_DEFVAR:
            fprintf(file, "  node_%p -> node_%p [label=\"initExpr\"];\n", (void *)node, (void *)node->nodeRep.defVarNode.initExpr);
            printASTNext(file, node);
            break;
        case AST_NODE_DEFFUNC:
            fprintf(file, "  node_%p -> node_%p [label=\"body\"];\n", (void *)node, (void *)node->nodeRep.defFuncNode.body);
            printASTNext(file, node);
            break;
        case AST_NODE_RETURN:
            if(node->nodeRep.returnNode.returnType != void_){
                fprintf(file, "  node_%p -> node_%p [label=\"returnExp\"];\n", (void *)node, (void *)node->nodeRep.returnNode.returnExp);
            }
            printASTNext(file, node);
            break;
        case AST_NODE_ROOT:
            printASTNext(file, node);
            break;
        case AST_UNUSED:
            fprintf(file, "  node_%p -> node_%p [label=\"expr\"];\n", (void *)node, (void *)node->nodeRep.unusedNode.expr);
            printASTNext(file, node);
            break;
        default:
            break;
    }
}


/**
 * @brief      Prints ID of id_without_null for ifElse and while statements.
 * 
 * @param file Pointer to an opened file to print to.
 * @param null Flag, whether id_without_null is present, print if true.
 * @param id   Actual ID of the constant without null.
 */
void printIdWithoutNull(FILE *file, bool null, char *id){
    if(null){
        fprintf(file ,"\n ID=%s ", id);
    }
    else{
        return;
    }
}

/**
 * @brief Prints additional information about literal.
 *        
 *        Type and value based on the representation.
 * 
 * @param file Pointer to an opened file to print to.
 * @param node astLiteral representation of the node containing information.
 */
void printLiteralInfo(FILE *file, astLiteral node){
    switch(node.dataT){
        case i32:
            fprintf(file, "\n type: i32\n Value: %d", node.value.intData);
            break;
        case f64:
            fprintf(file, "\n type: f64\n Value: %.2f", node.value.floatData);
            break;
        case string:
            fprintf(file, "\n type: string\n Value: %s", node.value.charData);
            break;
        default:
            break;
    }
    return;
}

/**
 * @brief      Prints additional information about variable definitions.
 * 
 * @param file Pointer to an opened file to print to.
 * @param node astDefVar representation of the node containing information.
 */

void printDefVarInfo(FILE *file, astDefVar node){
    if(node.symtableEntry->data.data.vData.isConst){
        fprintf(file, "\nconst\n");
    }
    else{
        fprintf(file, "var\n");
    }
    if(node.symtableEntry->data.data.vData.isNullable){
        fprintf(file, "?");
    }

    switch(node.symtableEntry->data.data.vData.type){
        case i32:
            fprintf(file, "i32\n");
            break;
        case f64:
            fprintf(file, "f64\n");
            break;
        case u8:
            fprintf(file, "u8\n");
            break;
        default:
            break;
    }
    
}


/**
 * @brief      Prints additional information about function definitions.
 * 
 * @param file Pointer to an opened file to print to.
 * @param node astDefFunc representation of the node.
 */

void printDefFuncInfo(FILE *file, astDefFunc node){

    fprintf(file, "\n RET: ");
    if(node.nullable){
        fprintf(file, "?");
    }
    switch(node.returnType){
        case i32:
            fprintf(file, "i32");
            break;
        case f64:
            fprintf(file, "f64");
            break;
        case u8:
            fprintf(file, "u8");
            break;
        case void_:
            fprintf(file, "void");
            break;
        default:
            break;
    }
}

/**
 * @brief      Prints the operator type for binary operations.
 * 
 *             This function prints the type of binary operator (e.g., `+`, `-`, `*`, `/`, `==`, `!=`) 
 *             based on the type of binary operation node.
 * 
 * @param file Pointer to an opened file to print to.
 * @param type The type of binary operator.
 */

void printBinopType(FILE *file, symbol_number type) {
    switch (type) {
        case MULTIPLICATION:
            fprintf(file, "*");
            break;
        case DIVISION:
            fprintf(file, "/");
            break;
        case ADDITION:
            fprintf(file, "+");
            break;
        case SUBSTRACTION:
            fprintf(file, "-");
            break;
        case EQUAL:
            fprintf(file, "==");
            break;
        case NOT_EQUAL:
            fprintf(file, "!=");
            break;
        case LOWER:
            fprintf(file, "<");
            break;
        case GREATER:
            fprintf(file, ">");
            break;
        case LOWER_OR_EQUAL:
            fprintf(file, "<=");
            break;
        case GREATER_OR_EQUAL:
            fprintf(file, ">=");
            break;
        case LBR:
            fprintf(file, "(");
            break;
        case RBR:
            fprintf(file, ")");
            break;
        default:
            fprintf(file, "UNKNOWN");
            break;
    }
    fprintf(file, "\n"); 
}

/**
 * @brief Prints the "next" edge of a node if it exists.
 * 
 *        Checks if the node has a "next" node.
 *        If the next node exists, it recursively prints it with a dotted edge
 *        to represent the next node in sequence.
 * 
 * @param file Pointer to an opened file to print to.
 * @param node Pointer to the astNode to print information about.
 */

void printASTNext(FILE *file, astNode *node){
    if(node->next != NULL){
        fprintf(file, "  node_%p -> node_%p [label=\"next\", style=dotted];\n", (void *)node, (void *)node->next);
        printASTNode(file, node->next);
        return;
    }
}