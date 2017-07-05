#include "flowChart_parser.h"
#include "serealiser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int MAX_VAR_COUT = 100;
int MAX_LIST_LEN = 100;

int addVar(int param, char* var, char*** answer) {
    static char** result;
    static int len = 0;
    int f;
    switch (param) {
        case 0:
            result = malloc(sizeof(char*) * MAX_VAR_COUT);
            break;
        case 1:
            f = 1;
            for (int i = 0; i < len; ++i) {
                if (!strcmp(var, result[i])) {
                    f = 0;
                    break;
                }
            }
            if (f) {
                result[len] = var;
                len++;
            }
            break;
        case 2:
            *answer = result;
            break;
    }
    return len;
}

void skipT(char** source) {
    while (**source == ' ' || **source == '\n' || **source == '\t') {
        ++(*source);
    }
}

void skip(char** source, char* target) {
    skipT(source);
    while (*target != 0) {
        ++(*source);
        ++target;
    }
}

char* readUntil(char** source, char c) {
    skipT(source);
    int i = 0;
    while ((*source)[i] != c && (*source)[i] != 0) {
        ++i;
    }
    char* res = (char*) malloc(sizeof(char) * (i + 1));
    for (i = 0; **source != c && **source != 0; ++i, ++(*source)) {
        res[i] = **source;
    }
    ++(*source);
    res[i] = 0;
    return res;
}

int count(char* source, char c) {
    int res = 0;
    for (int i = 0; source[i] != 0; ++i) {
        if (source[i] == c) {
            ++res;
        }
    }
    return res;
}

Const parceConst(char** input) {
    //fprintf(stderr, "parceConst\n");
    skipT(input);
    Const res;
    if (**input == '(') {
        res.type = LIST;
        List* l =  malloc(sizeof(List));
        (*l).list = malloc(sizeof(Const) * MAX_LIST_LEN);
        int i = 0;
        skip(input, "(");
        while (**input != ')') {
            (*l).list[i] = parceConst(input);
            ++i;
            skipT(input);
        }
        skip(input, ")");
        (*l).listLen = i;
        res.expr = l;
        return res;
    }
    int* n = malloc(sizeof(int));
    *n = 0;
    while (**input >= '0' && **input <= '9') {
        *n = *n * 10;
        *n = *n + **input - '0';
        ++(*input);
    }
    res.type = NUMBER;
    res.expr = n;
    return res;
}

Bop parceBop(char**);
Uop parceUop(char**);

Expr parceExpr(char** input) {
    //fprintf(stderr, "parceExpr\n");
    Expr expr;
    skipT(input);
    if ((**input >= '0' && **input <= '9') || **input == '(') {
        expr.type = CONST;
        Const* res = (Const*) malloc(sizeof(Const));
        *res = parceConst(input);
        expr.expr = res;
    }
    else if (**input == '_') {
        expr.type = BOP;
        Bop* res = (Bop*) malloc(sizeof(Bop));
        *res = parceBop(input);
        expr.expr = res;
    }
    else if (**input == '^') {
        expr.type = UOP;
        Uop* res = (Uop*) malloc(sizeof(Uop));
        *res = parceUop(input);
        expr.expr = res;
    }
    else {
        expr.type = VAR;
        char* var = readUntil(input, ' ');
        expr.expr = var;
        addVar(1, var, NULL);
    }
    return expr;
}

Uop parceUop(char** input) {
    //fprintf(stderr, "parceBop\n");
    Uop uop;
    skipT(input);
    uop.op = readUntil(input, ' ');
    uop.left = parceExpr(input);
    return uop;
}

Bop parceBop(char** input) {
    //fprintf(stderr, "parceBop\n");
    Bop bop;
    skipT(input);
    bop.op = readUntil(input, ' ');
    bop.left = parceExpr(input);
    bop.right = parceExpr(input);
    return bop;
}

If parceIf(char** input) {
    //fprintf(stderr, "parceIf\n");
    If iF;
    skipT(input);
    iF.expr = parceExpr(input);
    skip(input, "goto");
    iF.ifTrue = atoi(readUntil(input, ' '));
    skip(input, "else");
    iF.ifFalse = atoi(readUntil(input, ' '));
    skip(input, ".");
    return iF;
}

Jump parceJump(char** input) {
    //fprintf(stderr, "parceJump\n");
    Jump jump;
    skipT(input);
    if (**input == 'g') {
        skip(input, "goto");
        jump.type = GOTO;
        Label* res = (Label*) malloc(sizeof(Label));
        *res = atoi(readUntil(input, '.'));
        jump.jump = res;
    }
    else if (**input == 'i') {
        skip(input, "if");
        jump.type = IF;
        If* res = (If*) malloc(sizeof(If));
        *res = parceIf(input);
        jump.jump = res;
    }
    else {
        skip(input, "return");
        jump.type = RETURN;
        Expr* res = (Expr*) malloc(sizeof(Expr));
        *res = parceExpr(input);
        jump.jump = res;
    }
    return jump;
}

Assignment parceAssignment(char** input) {
    //fprintf(stderr, "parceAssignment\n");
    Assignment assignment;
    skipT(input);
    char* curAssignment = readUntil(input, ';');
    char* var = readUntil(&curAssignment, ' ');
    assignment.var = var;
    addVar(1, var, NULL);
    skip(&curAssignment, ":=");
    assignment.expr = parceExpr(&curAssignment);
    return assignment;
}

BasicBlock parceBlock(char** input) {
    //fprintf(stderr, "parceBlock\n");
    BasicBlock block;
    skipT(input);
    char* curBlock = readUntil(input, '.');
    block.label = atoi(readUntil(&curBlock, ':'));
    block.assignmentCount = count(curBlock, ';');
    block.assignments = (Assignment *) malloc(sizeof(Assignment) * block.assignmentCount);

    for (int i = 0; i < block.assignmentCount; ++i) {
        block.assignments[i] = parceAssignment(&curBlock);
    }

    block.jump = parceJump(&curBlock);
    return block;
}

Program parceProgram(char** input) {
    //fprintf(stderr, "parceProgram\n");
    Program program;
    addVar(0, NULL, NULL);
    skipT(input);
    skip(input, "read");
    char* vars = readUntil(input, ';');
    program.inputCount = count(vars, ',') + 1;
    program.input = (Var*) malloc(sizeof(Var) * program.inputCount);
    for (int i = 0; i < program.inputCount; ++i) {
        char* var = readUntil(&vars, ',');
        program.input[i] = var;
        addVar(1, var, NULL);
    }
    program.blockCount = count(*input, '.');
    program.basicBlocks = (BasicBlock*) malloc(sizeof(BasicBlock) * program.blockCount);

    for (int i = 0; i < program.blockCount; ++i) {
        program.basicBlocks[i] = parceBlock(input);
    }
    program.varCount = addVar(2, NULL, &program.varNames);
    return program;
}

Program parce(char* input) {
    return parceProgram(&input);
}