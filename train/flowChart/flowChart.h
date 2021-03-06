#pragma once
typedef int Label;

typedef char* Var;
typedef int Number;
struct _Const;
typedef struct _Node {
    struct _Node* next;
    struct _Const* val;
} Node;
typedef Node* List;
typedef struct _Const {
    enum ConstType {NUMBER, LIST} type;
    void* expr;
} Const;

typedef struct _Expr {
    enum ExprType {CONST, VAR, BOP, UOP} type;
    void* expr;
} Expr;

typedef struct _Bop {
    char* op;
    Expr left, right;
} Bop;

typedef struct _Uop {
    char* op;
    Expr left;
} Uop;

typedef Label GoTo;
typedef Expr Return;
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
    Var* varNames;
    int varCount;
    Var* input;
    int inputCount;
    BasicBlock* basicBlocks;
    int blockCount;
} Program;
