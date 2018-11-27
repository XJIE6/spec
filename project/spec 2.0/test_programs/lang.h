typedef struct _program_state {
	int * vars;
	char is_ret;
	int ret_val;
} ProgramState;

enum TExpr{TConst, TVar, TBinop, TCall, TRead};

typedef struct _expr {
	enum TExpr type;
	void * p;
} Expr;


typedef struct _par {
	Expr * e;
	struct _par * next;
} Par;

enum TStmt{TSkip, TAss, TSeq, TIf, TWhile, TRun, TReturn, TWrite};

typedef struct _stmt { 
	enum TStmt type;
	void * s;
} Stmt;

typedef struct _ass {
	int var;
	Expr * e;
} Ass;

typedef struct _seq {
	Stmt * l, * r;
} Seq;

typedef struct _if {
	Expr * e;
	Stmt * l, * r;
} If;

typedef struct _while {
	Expr * e;
	Stmt * s;
} While;

typedef struct _run {
	int fun;
	Par * params;
} Run;

typedef struct _write {
	Expr * e;
} Write;

typedef struct _return {
	Expr * e;
} Return;

typedef struct _const {
	int val;
} Const;

typedef struct _var {
	int var;
} Var;

enum opcode{Oadd, Osub, Omul, Odiv, Omod, Olt, Ole, Ogt, Oge, Oeq, One, Oand, Oor};

typedef struct _binop {
	enum opcode op;
	Expr * l, * r;
} Binop;

typedef struct _call {
	int fun;
	Par * params;
} Call;

typedef struct _arg {
	int var;
	struct _arg * next;
} Arg;

typedef struct _def {
	int name;
	Arg* args;
	Stmt* s;
	struct _def * next;
} Def;

typedef struct _program {
	Def* defs;
	Stmt* s;
} Program;
