typedef int Label;

enum Op {hd, tl, cons};
typedef char* Var;
typedef int Cons;

typedef struct _Expr {
    enum ExprType {CONST, VAR, OP} type;
    void* expr;
} Expr;


typedef Label GoTo;
typedef Expr Returun;
typedef struct _If {
    Expr expr;
    Label ifTrue, ifFalse;
} If;

typedef struct _Jump {
    enum JumpType {LABEL, EXPR, IF} type;
    void* expr;
} Jump;

typedef struct _Assignment {
    Var var;
    Expr expr;
} Assignment;

typedef struct _BasicBlock {
    Label label;
    Assignment* assignments;
    int assignmentSize;
    Jump jump;
} BasicBlock;

typedef struct _Program {
    Var input;
    int varsSize;
    BasicBlock* basicBlocks;
    int blockSize;
} Program;
