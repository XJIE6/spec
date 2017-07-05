#pragma once
typedef int Label;

typedef char* Var;
typedef int Const;

typedef struct _Expr {
    enum ExprType {CONST, VAR, BOP} type;
    void* expr;
} Expr;

typedef struct _Bop {
    char* op;
    Expr left, right;
} Bop;


typedef Label GoTo;
typedef Expr Returun;
typedef struct _If {
    Expr expr;
    Label ifTrue, ifFalse;
} If;

typedef struct _Jump {
    enum JumpType {GOTO, RETURN, IF} type;
    void* jump;
} Jump;

typedef struct _Assignment {
    Var var;
    Expr expr;
} Assignment;

typedef struct _BasicBlock {
    Label label;
    Assignment* assignments;
    int assignmentCount;
    Jump jump;
} BasicBlock;

typedef struct _Program {
    char** varNames;
    int varCount;
    Var* input;
    int inputCount;
    BasicBlock* basicBlocks;
    int blockCount;
} Program;
