#include <stdio.h>
#include "lang.h"

int eval_expr(Expr * e, State * s) {
	int l, r;
	switch(e->type) {
		case TConst:
			return ((Const *)e->p)->val;
			break;
		case TVar:
			return s->vars[((Var*)e->p)->var];
			break;
		case TBinop:
			l = eval_expr(((Binop *)e->p)->l, s);
			r = eval_expr(((Binop *)e->p)->r, s);
			switch(((Binop *)e->p)->op) {
				case add:
					return l + r;
				case sub:
					return l - r;
				case mul:
					return l * r;
				case div:
					return l / r;
			}
			break;
		case TCall:
			return 0; //to-do
			break;
	}
	return 0;
}

int eval_stmt(Stmt * stmt, State * s) {
	switch (stmt->type) {
		case TSkip:
			return 0;
			break;
		case TAss:
			s->vars[((Ass *) stmt->s)->var] = eval_expr(((Ass *) stmt->s)->e, s);
			break;
		case TSeq:
			eval_stmt(((Seq *) stmt->s)->l, s); //continue if return
			eval_stmt(((Seq *) stmt->s)->r, s);
			break;
		case TIf:
			if (eval_expr(((If *) stmt->s)->e, s)) {
				eval_stmt(((If *) stmt->s)->l, s);
			}
			else {
				eval_stmt(((If *) stmt->s)->r, s);
			}
			break;
		case TWhile:
			while (eval_expr(((While *) stmt->s)->e, s)) {
				eval_stmt(((While *) stmt->s)->s, s);
			}
			break;
		case TRun:
			return 0; //to-do
			break;
		case TReturn:
			return eval_expr(((Return *) stmt->s)->e, s);
			break;
	}

}


int main() {
	Program* p;
	ProgramState* ps;
	eval_prog(p, ps);
}