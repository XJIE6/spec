#define TEST

#include <stdlib.h>
#include "../my_malloc.c"
#include "lang.h"
#ifdef TEST
#include <stdio.h>
#endif

void eval_stmt(Def* def, Stmt* stmt, ProgramState* s);

int eval_expr(Def* def, Expr* e, ProgramState* s) {
	int * values = s->vars;
	int l, r;
	if (e->type == TConst) {
		return ((Const *)e->p)->val;
	}
	#ifdef TEST
	if (e->type == TRead) {
		int res;
		scanf("%d", &res);
		printf("> ");
		return res;
	}
	#endif
	else if (e->type == TVar) {
		return values[((Var*)e->p)->var];
	}
	else if (e->type == TBinop) {
		l = eval_expr(def, ((Binop *)e->p)->l, s);
		r = eval_expr(def, ((Binop *)e->p)->r, s);
		if (((Binop *)e->p)->op == Oadd) {
			return l + r;
		}
		if (((Binop *)e->p)->op == Osub) {
			return l - r;
		}
		if (((Binop *)e->p)->op == Omul) {
			return l * r;
		}
		if (((Binop *)e->p)->op == Odiv) {
			return l / r;
		}
		if (((Binop *)e->p)->op == Omod) {
			return l % r;
		}
		if (((Binop *)e->p)->op == Olt) {
			if (l < r) {
				return 1;
			}
			return 0;
		}
		if (((Binop *)e->p)->op == Ole) {
			if (l <= r) {
				return 1;
			}
			return 0;
		}
		if (((Binop *)e->p)->op == Ogt) {
			if (l > r) {
				return 1;
			}
			return 0;
		}
		if (((Binop *)e->p)->op == Oge) {
			if (l >= r) {
				return 1;
			}
			return 0;
		}
		if (((Binop *)e->p)->op == Oeq) {
			if (l == r) {
				return 1;
			}
			return 0;
		}
		if (((Binop *)e->p)->op == One) {
			if (l != r) {
				return 1;
			}
			return 0;
		}
		if (((Binop *)e->p)->op == Oand) {
			return l && r;
		}
		if (((Binop *)e->p)->op == Oor) {
			return l || r;
		}
	}
	else if (e->type == TCall) {
		ProgramState* new_s = (ProgramState*) my_malloc(sizeof(ProgramState));
		new_s->vars = (int*) my_malloc(sizeof(int) * 200);
		new_s->is_ret = 0;
		new_s->ret_val = 0;
		Def* copy = def;
		while (copy->name != ((Call *) e->p)->fun) { //no null check
			copy = copy->next;
		}
		Par* param = ((Call *) e->p)->params;
		Arg* arg = (copy)->args;
		while (param != NULL) {
			int * vars = new_s->vars;
			vars[arg->var] = eval_expr(def, param->e, s);
			arg = arg->next;
			param = param->next;
		}
		eval_stmt(def, copy->s, new_s);
		return new_s->ret_val;
	}
	return 0;
}

void eval_stmt(Def* def, Stmt* stmt, ProgramState* s) {
	int * values = s->vars;
	if (stmt->type == TSkip) {
	}
	else if (stmt->type == TAss) {
		Ass* ass = (Ass *) stmt->s;
		values[ass->var] = eval_expr(def, ass->e, s);
	}
	else if (stmt->type == TSeq) {
		eval_stmt(def, ((Seq *) stmt->s)->l, s);
		if (s->is_ret) {
			return;
		}
		eval_stmt(def, ((Seq *) stmt->s)->r, s);
	}
	else if (stmt->type == TIf) {
		int res = eval_expr(def, ((If *) stmt->s)->e, s);
		if (res) {
			eval_stmt(def, ((If *) stmt->s)->l, s);
		}
		else {
			eval_stmt(def, ((If *) stmt->s)->r, s);
		}
	}
	else if (stmt->type == TWhile) {
		while (eval_expr(def, ((While *) stmt->s)->e, s)) {
			eval_stmt(def, ((While *) stmt->s)->s, s);
		}
	}
	else if (stmt->type == TRun) {
		ProgramState* new_s = (ProgramState*) my_malloc(sizeof(ProgramState));
		new_s->vars = (int*) my_malloc(sizeof(int) * 200);
		new_s->is_ret = 0;
		new_s->ret_val = 0;
		Def* copy = def;
		while (copy->name != ((Run *) stmt->s)->fun) { //no null check
			copy = copy->next;
		}
		Par* param = ((Run *) stmt->s)->params;
		Arg* arg = (copy)->args;
		while (param != 0) {
			int* vars = new_s->vars;
			vars[arg->var] = eval_expr(def, param->e, s);
			arg = arg->next;
			param = param->next;
		}
		eval_stmt(def, copy->s, new_s);
	}
	#ifdef TEST
	else if (stmt->type == TWrite) {
		int res = eval_expr(def, ((Write *) stmt->s)->e, s);
		printf("%d\n", res);
	}
	#endif
	else if (stmt->type == TReturn) {
		s->is_ret = 1;
		s->ret_val = eval_expr(def, ((Return *) stmt->s)->e, s);
	}
}

int eval_prog(Program* prog, ProgramState* s) {
	eval_stmt(prog->defs, prog->s, s);
	return s->ret_val;
}