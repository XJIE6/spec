typedef struct _state {
	int * vars;
} State;

enum TExpr{TConst, TVar, TBinop, TCall};

typedef struct _expr {
	TExpr type;
	void * p;
} Expr;


typedef struct _param {
	Expr * e;
	struct _param * next;
} Param;

typedef struct _stmt {
	enum TStmt{TSkip, TAss, TSeq, TIf, TWhile, TRun, TReturn} type;
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
	Param * params;
} Run;

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
	opcode op;
	Expr * l, * r;
} Binop;

typedef struct _call {
	int fun;
	Param * params;
} Call;
