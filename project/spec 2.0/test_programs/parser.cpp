//#define TEST

#include <pcomb.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "lang.h"

using namespace pcomb;

void * start, * current;
void * cleverMalloc(int size) {
	void * last = current;
	current += size;
	return last;
}

int eval_prog(Program* prog, ProgramState* s);

std::string names [23];
std::string dy_names [3] = {"x", "y", "z"};
int last = 0;

auto expr0 = LazyParser<Expr*>();

auto id = rule(token(regex("\\w+")), [] (auto name) -> int {
	for (int j = 0; j < 3; ++j) {
		if (dy_names[j] == name) {
			return 23 + j;
		}
	}
	for (int i = 0; i < last; ++i) {
		if (names[i] == name.to_string()) {
			return i;
		}
	}
	names[last] = name.to_string();
	return last++;
});

auto param = alt
(
	rule(seq(token(ch('(')), expr0.getRef(), many(seq(token(ch(',')), expr0.getRef())), token(ch(')'))),
		[] (auto call) -> Par* {
			Par* res = (Par*) cleverMalloc(sizeof(Par));
			res->e = std::get<1>(call);
			res->next = NULL;
			Par* cur = res;
			for (auto& param: std::get<2>(call)) {
				cur->next = (Par*) cleverMalloc(sizeof(Par));
				cur = cur->next;
				cur->e = std::get<1>(param);
				cur->next = NULL;
			}
			return res;
		}
	),
	rule(seq(token(ch('(')), token(ch(')'))), [](auto nothing)-> Par * {return NULL;})
);

auto nexpr = alt
(
	rule(token(regex("[+-]?\\d+")), [] (auto n) -> Expr* { 
		Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
		Const * c = (Const*) cleverMalloc(sizeof(Const));
		e->type = TConst;
		e->p = c;
		c->val = std::stoul(n.to_string());
		return e;
	}),

	#ifdef TEST
	rule(seq(token(str("read")), token(ch('(')), token(ch(')'))), [] (auto read) -> Expr* {
		Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
		e->type = TRead;
	}),
	#endif

	rule(seq(id, param), [] (auto call) -> Expr* { 
		Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
		Call * c = (Call*) cleverMalloc(sizeof(Call));
		e->type = TCall;
		e->p = c;
		c->fun = std::get<0>(call);
		c->params = std::get<1>(call);
		return e;
	}),
	rule(id, [] (auto id) -> Expr* { 
		Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
		Var * c = (Var*) cleverMalloc(sizeof(Var));
		e->type = TVar;
		e->p = c;
		c->var = id;
		return e;
	}),
	rule
	(
		seq(token(ch('(')), expr0.getRef(), token(ch(')'))),
		[] (auto triple)
		{
			return std::get<1>(triple);
		}
	)
);

auto mul = rule
(
	seq(nexpr, many(seq(alt(token(ch('*')), token(ch('/')), token(ch('%'))), nexpr))),
	[] (auto pair) -> Expr*
	{
		Expr* res = std::get<0>(pair);
		auto& exprVec = std::get<1>(pair);

		for (auto& pair: exprVec) {
			Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
			Binop * c = (Binop*) cleverMalloc(sizeof(Binop));
			e->type = TBinop;
			e->p = c;
			c->l = res;
			c->r = std::get<1>(pair);
			switch (std::get<0>(pair)) {
				case '*':
					c->op = Omul;
					break;
				case '/':
					c->op = Odiv;
					break;
				case '%':
					c->op = Omod;
					break;
			}
			res = e;
		}
		return res;
	}
);

auto add = rule
(
	seq(mul, many(seq(alt(token(ch('+')), token(ch('-'))), mul))),
	[] (auto pair) -> Expr*
	{
		Expr* res = std::get<0>(pair);
		auto& exprVec = std::get<1>(pair);

		for (auto& pair: exprVec) {
			Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
			Binop * c = (Binop*) cleverMalloc(sizeof(Binop));
			e->type = TBinop;
			e->p = c;
			c->l = res;
			c->r = std::get<1>(pair);
			switch (std::get<0>(pair)) {
				case '+':
					c->op = Oadd;
					break;
				case '-':
					c->op = Osub;
					break;
			}
			res = e;
		}
		return res;
	}
);

auto eq = rule
(
	seq(add, many(seq(alt(token(str("<=")), token(str("<")), token(str("==")), token(str("!=")), token(str(">=")), token(str(">"))), add))),
	[] (auto pair) -> Expr*
	{
		Expr* res = std::get<0>(pair);
		auto& exprVec = std::get<1>(pair);

		for (auto& pair: exprVec) {
			Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
			Binop * c = (Binop*) cleverMalloc(sizeof(Binop));
			e->type = TBinop;
			e->p = c;
			c->l = res;
			c->r = std::get<1>(pair);
			auto s = std::get<0>(pair);
			if (s == "<=") {
				c->op = Ole;
			} else if (s == "<") {
				c->op = Olt;
			} else if (s == "==") {
				c->op = Oeq;
			} else if (s == "!=") {
				c->op = One;
			} else if (s == ">=") {
				c->op = Oge;
			} else if (s == ">") {
				c->op = Ogt;				
			}
			res = e;
		}
		return res;
	}
);

auto andi = rule
(
	seq(eq, many(seq(token(str("&&")), eq))),
	[] (auto pair) -> Expr*
	{
		Expr* res = std::get<0>(pair);
		auto& exprVec = std::get<1>(pair);

		for (auto& pair: exprVec) {
			Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
			Binop * c = (Binop*) cleverMalloc(sizeof(Binop));
			e->type = TBinop;
			e->p = c;
			c->l = res;
			c->r = std::get<1>(pair);
			c->op = Oand;
			res = e;
		}
		return res;
	}
);

auto ori = rule
(
	seq(andi, many(seq(token(str("!!")), andi))),
	[] (auto pair) -> Expr*
	{
		Expr* res = std::get<0>(pair);
		auto& exprVec = std::get<1>(pair);

		for (auto& pair: exprVec) {
			Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
			Binop * c = (Binop*) cleverMalloc(sizeof(Binop));
			e->type = TBinop;
			e->p = c;
			c->l = res;
			c->r = std::get<1>(pair);
			c->op = Oor;
			res = e;
		}
		return res;
	}
);

auto expr = expr0.setParser(ori);

auto st0 = LazyParser<Stmt*>();

auto els = alt
(
	rule(seq(token(str("else")), st0.getRef(), token(str("fi"))), [](auto triple) -> Stmt* {
		return std::get<1>(triple);
	}),
	rule(token(str("fi")), [](auto single) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TSkip;
		return stmt;
	})
);

auto simple = alt
(
	#ifdef TEST
	rule (seq(token(str("write")), token(ch('(')), expr, token(ch(')'))), 
	[] (auto writestmt) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TWrite;
		Write* write = (Write *) cleverMalloc(sizeof(Write));
		stmt->s = write;
		write->e = std::get<2>(writestmt);
		return stmt;
	}),
	#endif
	rule (seq(id, param), [](auto pair) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TRun;
		Run* run = (Run *) cleverMalloc(sizeof(Run));
		stmt->s = run;
		run->fun = std::get<0>(pair);
		run->params = std::get<1>(pair);
		return stmt;
	}),
	rule (seq(id, token(str(":=")), expr), [](auto triple) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TAss;
		Ass* ass = (Ass *) cleverMalloc(sizeof(Ass));
		stmt->s = ass;
		ass->var = std::get<0>(triple);
		ass->e = std::get<2>(triple);
		return stmt;
	}),
	rule (token(str("skip")), [](auto single) -> Stmt*{
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TSkip;
		return stmt;
	}),
	rule (seq(token(str("return")), expr), [](auto pair) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TReturn;
		Return* ret = (Return *) cleverMalloc(sizeof(Return));
		stmt->s = ret;
		ret->e = std::get<1>(pair);
		return stmt;
	}),
	rule (seq(token(str("if")), expr, token(str("then")), st0.getRef(), 
		many(seq(token(str("elif")), expr, token(str("then")), st0.getRef())),
		els), [](auto ifstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TIf;
		If* iff = (If *) cleverMalloc(sizeof(If));
		stmt->s = iff;
		iff->e = std::get<1>(ifstmt);
		iff->l = std::get<3>(ifstmt);
		for (auto& elif: std::get<4>(ifstmt)) {
			iff->r = (Stmt *) cleverMalloc(sizeof(Stmt));
			iff->r->type = TIf;
			iff->r->s = (If *) cleverMalloc(sizeof(If));
			iff = (If *)iff->r->s;
			iff->e = std::get<1>(elif);
			iff->l = std::get<3>(elif);
		}
		iff->r = std::get<5>(ifstmt);
		return stmt;
	}),
	rule (seq(token(str("while")), expr, token(str("do")), st0.getRef(), token(str("od"))), 
		[](auto whlstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TWhile;
		While* whl = (While *) cleverMalloc(sizeof(While));
		stmt->s = whl;
		whl->e = std::get<1>(whlstmt);
		whl->s = std::get<3>(whlstmt);
		return stmt;
	}),
	rule (seq(token(str("repeat")), st0.getRef(), token(str("until")), expr),
		[](auto whlstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TSeq;
		Stmt* stmt1 = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt1->type = TWhile;
		Seq* s = (Seq *) cleverMalloc(sizeof(Seq));
		stmt->s = s;
		While* whl = (While *) cleverMalloc(sizeof(While));
		stmt1->s = whl;
		s->l = std::get<1>(whlstmt);
		s->r = stmt1;
		Expr * e = (Expr*) cleverMalloc(sizeof(Expr));
		Binop * c = (Binop*) cleverMalloc(sizeof(Binop));
		e->type = TBinop;
		e->p = c;
		c->l = std::get<3>(whlstmt);
		Expr * e1 = (Expr*) cleverMalloc(sizeof(Expr));
		Const * c1 = (Const*) cleverMalloc(sizeof(Const));
		e1->type = TConst;
		e1->p = c1;
		c1->val = 0;
		c->r = e1;
		c->op = Oeq;
		whl->e = e;
		whl->s = std::get<1>(whlstmt);
		return stmt;
	}),
	rule (seq(token(str("for")), st0.getRef(), token(ch(',')), expr, token(ch(',')), st0.getRef(), 
		token(str("do")), st0.getRef(), token(str("od"))),
		[](auto whlstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TSeq;
		Stmt* stmt1 = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt1->type = TWhile;
		Stmt* stmt2 = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt2->type = TSeq;
		Seq* s = (Seq *) cleverMalloc(sizeof(Seq));
		stmt->s = s;
		While* whl = (While *) cleverMalloc(sizeof(While));
		stmt1->s = whl;
		Seq* s2 = (Seq *) cleverMalloc(sizeof(Seq));
		stmt2->s = s2;
		s->l = std::get<1>(whlstmt);
		s->r = stmt1;
		whl->e = std::get<3>(whlstmt);
		whl->s = stmt2;
		s2->l = std::get<7>(whlstmt);
		s2->r = std::get<5>(whlstmt);
		return stmt;
	})
);

auto sequ = alt
(
	rule(seq(simple, token(ch(';')), st0.getRef()), [](auto triple) -> Stmt* {
		Stmt* stmt = (Stmt *) cleverMalloc(sizeof(Stmt));
		stmt->type = TSeq;
		Seq* s = (Seq *) cleverMalloc(sizeof(Seq));
		stmt->s = s;
		s->l = std::get<0>(triple);
		s->r = std::get<2>(triple);
		return stmt;
	}),
	simple
);

auto st = st0.setParser(sequ);

auto arg = alt
(
	rule(seq(token(ch('(')), id, many(seq(token(ch(',')), id)), token(ch(')'))),
		[] (auto call) -> Arg* {
			Arg* res = (Arg*) cleverMalloc(sizeof(Arg));
			res->var = std::get<1>(call);
			res->next = NULL;
			Arg* cur = res;
			for (auto& param: std::get<2>(call)) {
				cur->next = (Arg*) cleverMalloc(sizeof(Arg));
				cur = cur->next;
				cur->var = std::get<1>(param);
				cur->next = NULL;
			}
			return res;
		}
	),
	rule(seq(token(ch('(')), token(ch(')'))), [](auto nothing)-> Arg* {return NULL;})
);

auto program = rule(seq(many(seq(token(str("fun")), id, arg, token(str("begin")), st, token(str("end")))), st), 
	[](auto pair)->Program* {
		Program* prog = (Program *) cleverMalloc(sizeof(Program));
		prog->s = std::get<1>(pair);
		Def ** cur = &(prog->defs);
		for (auto& def: std::get<0>(pair)) {
			*cur = (Def *) cleverMalloc(sizeof(Def));
			(*cur)->name = std::get<1>(def);
			(*cur)->args = std::get<2>(def);
			(*cur)->s = std::get<4>(def);
			cur = &((*cur)->next);
		}
		(*cur) = NULL;
		return prog;
	});

auto parser = bigstr(program);

extern "C" void parseLine(char* str, void** ptr, int* len, Program ** p)
{
	start = malloc(1000000000);
	current = start;
	InputStream ss(str);
	auto parseResult = parser.parse(ss);
	if (parseResult.hasError())
	{
		auto remainingStream = parseResult.getInputStream();
		std::cout << "Parsing failed at line " << remainingStream.getLineNumber() << ", column " << remainingStream.getColumnNumber() << "\n";
		return;
	}
	*ptr = start;
	*len = (char *)current - (char *) start;
	(*p) = parseResult.getOutput();
	return;
}