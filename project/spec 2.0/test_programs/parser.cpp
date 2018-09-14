// #include <stdio.h>

// char* s;

// int read(const char* a, int mode = 0) {
// 	while (isspece(*s)) {
// 		s++;
// 	}
// 	char* old = s;
// 	while (*a != 0) {
// 		if (*a != *b) {
// 			s = old;
// 			return 0;
// 		}
// 		a++;
// 		s++;
// 	}
// 	if (mode) {
// 		s = old;
// 	}
// 	return 1;
// }

// Stmt * read_stmt() {
// 	split
// }

// State* parse(char * program) {
// 	while (read("fun", 1)) {
// 		read_function();
// 	}
// 	read_stmt();
// }

#include <pcomb.h>

#include <iostream>
#include <stdexcept>

#include "lang.h"


// This files shows how to use pcomb to write a simple calculator

using namespace pcomb;

// AST definitions
// class Expr
// {
// public:
// 	virtual long eval() const = 0;
// 	virtual ~Expr() = default;
// };

// class NumExpr: public Expr
// {
// private:
// 	long num;
// public:
// 	NumExpr(long n): num(n) {}

// 	long eval() const override { return num; }
// };

// class BinExpr: public Expr
// {
// private:
// 	char opCode;
// 	ExprPtr lhs, rhs;
// public:
// 	BinExpr(char o, ExprPtr l, ExprPtr r): opCode(o), lhs(std::move(l)), rhs(std::move(r)) {}

// 	long eval() const override
// 	{
// 		assert(lhs && rhs);
// 		switch (opCode)
// 		{
// 			case '+':
// 				return lhs->eval() + rhs->eval();
// 			case '-':
// 				return lhs->eval() - rhs->eval();
// 			case '*':
// 				return lhs->eval() * rhs->eval();
// 			case '/':
// 				return lhs->eval() / rhs->eval();
// 			default:
// 				throw std::runtime_error("Invalid opcode");
// 		}
// 	}
// };

auto expr0 = LazyParser<Expr*>();

// Grammar of the calculator

auto id = rule(token(regex("\\w+")), [] (auto name) -> int {
	return name.to_string()[0]; // TODO
});

auto param = alt
(
	rule(seq(token(ch('(')), expr0.getRef(), many(seq(token(ch(',')), expr0.getRef())), token(ch(')'))),
		[] (auto call) -> Param* {
			Param* res = (Param*) malloc(sizeof(Param));
			res->e = std::get<1>(call);
			Param* cur = res;
			for (auto& param: std::get<2>(call)) {
				cur->next = (Param*) malloc(sizeof(Param));
				cur = cur->next;
				cur->e = std::get<1>(param);
			}
			return res;
		}
	),
	rule(seq(token(ch('(')), token(ch(')'))), [](auto nothing)-> Param * {return NULL;})
);

auto nexpr = alt
(
	rule(token(regex("[+-]?\\d+")), [] (auto n) -> Expr* { 
		Expr * e = (Expr*) malloc(sizeof(Expr));
		Const * c = (Const*) malloc(sizeof(Const));
		e->type = TConst;
		e->p = c;
		c->val = std::stoul(n.to_string());
		return e;
	}),
	rule(seq(id, param), [] (auto call) -> Expr* { 
		Expr * e = (Expr*) malloc(sizeof(Expr));
		Call * c = (Call*) malloc(sizeof(Call));
		e->type = TCall;
		e->p = c;
		c->fun = std::get<0>(call);
		c->params = std::get<1>(call);
		return e;
	}),
	rule(id, [] (auto id) -> Expr* { 
		Expr * e = (Expr*) malloc(sizeof(Expr));
		Var * c = (Var*) malloc(sizeof(Var));
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
			Expr * e = (Expr*) malloc(sizeof(Expr));
			Binop * c = (Binop*) malloc(sizeof(Binop));
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
			Expr * e = (Expr*) malloc(sizeof(Expr));
			Binop * c = (Binop*) malloc(sizeof(Binop));
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
			Expr * e = (Expr*) malloc(sizeof(Expr));
			Binop * c = (Binop*) malloc(sizeof(Binop));
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
			Expr * e = (Expr*) malloc(sizeof(Expr));
			Binop * c = (Binop*) malloc(sizeof(Binop));
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
	seq(andi, many(seq(token(str("||")), andi))),
	[] (auto pair) -> Expr*
	{
		Expr* res = std::get<0>(pair);
		auto& exprVec = std::get<1>(pair);

		for (auto& pair: exprVec) {
			Expr * e = (Expr*) malloc(sizeof(Expr));
			Binop * c = (Binop*) malloc(sizeof(Binop));
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
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TSkip;
		return stmt;
	})
);

auto simple = alt
(
	rule (seq(id, param), [](auto pair) -> Stmt* {
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TRun;
		Run* run = (Run *) malloc(sizeof(Run));
		stmt->s = run;
		run->fun = std::get<0>(pair);
		run->params = std::get<1>(pair);
		return stmt;
	}),
	rule (seq(id, token(str(":=")), expr), [](auto triple) -> Stmt* {
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TAss;
		Ass* ass = (Ass *) malloc(sizeof(Ass));
		stmt->s = ass;
		ass->var = std::get<0>(triple);
		ass->e = std::get<2>(triple);
		return stmt;
	}),
	rule (token(str("skip")), [](auto single) -> Stmt*{
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TSkip;
		return stmt;
	}),
	rule (seq(token(str("return")), expr), [](auto pair) -> Stmt* {
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TReturn;
		Return* ret = (Return *) malloc(sizeof(Return));
		stmt->s = ret;
		ret->e = std::get<1>(pair);
		return stmt;
	}),
	rule (seq(token(str("if")), expr, token(str("then")), st0.getRef(), 
		many(seq(token(str("elif")), expr, token(str("then")), st0.getRef())),
		els), [](auto ifstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TIf;
		If* iff = (If *) malloc(sizeof(If));
		stmt->s = iff;
		iff->e = std::get<1>(ifstmt);
		iff->l = std::get<3>(ifstmt);
		for (auto& elif: std::get<4>(ifstmt)) {
			iff->r = (Stmt *) malloc(sizeof(Stmt));
			iff->r->type = TIf;
			iff->r->s = (If *) malloc(sizeof(If));
			iff = (If *)iff->r->s;
			iff->e = std::get<1>(elif);
			iff->l = std::get<3>(elif);
		}
		iff->r = std::get<5>(ifstmt);
		return stmt;
	}),
	rule (seq(token(str("while")), expr, token(str("do")), st0.getRef(), token(str("od"))), 
		[](auto whlstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TWhile;
		While* whl = (While *) malloc(sizeof(While));
		stmt->s = whl;
		whl->e = std::get<1>(whlstmt);
		whl->s = std::get<3>(whlstmt);
		return stmt;
	}),
	rule (seq(token(str("repeat")), st0.getRef(), token(str("until")), expr),
		[](auto whlstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TSeq;
		Stmt* stmt1 = (Stmt *) malloc(sizeof(Stmt));
		stmt1->type = TWhile;
		Seq* s = (Seq *) malloc(sizeof(Seq));
		stmt->s = s;
		While* whl = (While *) malloc(sizeof(While));
		stmt1->s = whl;
		s->l = std::get<1>(whlstmt);
		s->r = stmt1;
		whl->e = std::get<3>(whlstmt);
		whl->s = std::get<1>(whlstmt);
		return stmt;
	}),
	rule (seq(token(str("for")), st0.getRef(), token(ch(',')), expr, token(ch(',')), st0.getRef(), 
		token(str("do")), st0.getRef(), token(str("od"))),
		[](auto whlstmt) -> Stmt* {
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TSeq;
		Stmt* stmt1 = (Stmt *) malloc(sizeof(Stmt));
		stmt1->type = TWhile;
		Stmt* stmt2 = (Stmt *) malloc(sizeof(Stmt));
		stmt2->type = TSeq;
		Seq* s = (Seq *) malloc(sizeof(Seq));
		stmt->s = s;
		While* whl = (While *) malloc(sizeof(While));
		stmt1->s = whl;
		Seq* s2 = (Seq *) malloc(sizeof(Seq));
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
		Stmt* stmt = (Stmt *) malloc(sizeof(Stmt));
		stmt->type = TSeq;
		Seq* s = (Seq *) malloc(sizeof(Seq));
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
			Arg* res = (Arg*) malloc(sizeof(Arg));
			res->var = std::get<1>(call);
			Arg* cur = res;
			for (auto& param: std::get<2>(call)) {
				cur->next = (Arg*) malloc(sizeof(Arg));
				cur = cur->next;
				cur->var = std::get<1>(param);
			}
			return res;
		}
	),
	rule(seq(token(ch('(')), token(ch(')'))), [](auto nothing)-> Arg* {return NULL;})
);

auto program = rule(seq(many(seq(token(str("fun")), id, arg, token(str("begin")), st, token(str("end")))), st), 
	[](auto pair)->Program* {
		Program* prog = (Program *) malloc(sizeof(Program));
		prog->s = std::get<1>(pair);
		Def ** cur = &(prog->defs);
		for (auto& def: std::get<0>(pair)) {
			*cur = (Def *) malloc(sizeof(Def));
			(*cur)->name = std::get<1>(def);
			(*cur)->args = std::get<2>(def);
			(*cur)->s = std::get<4>(def);
			cur = &((*cur)->next);
		}
		return prog;
	});

auto parser = bigstr(program);

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
				case Oadd:
					return l + r;
				case Osub:
					return l - r;
				case Omul:
					return l * r;
				case Odiv:
					return l / r;
				case Omod:
					return l % r;
				case Olt:
					return l < r;
				case Ole:
					return l <= r;
				case Ogt:
					return l > r;
				case Oge:
					return l >= r;
				case Oeq:
					return l == r;
				case One:
					return l != r;
				case Oand:
					return l && r;
				case Oor:
					return l || r;
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

int eval_prog(Program* prog, State* s) {
	return eval_stmt(prog->s, s);
}

void parseLine(const std::string& lineStr)
{
	InputStream ss(lineStr);
	auto parseResult = parser.parse(ss);
	if (parseResult.hasError())
	{
		auto remainingStream = parseResult.getInputStream();
		std::cout << "Parsing failed at line " << remainingStream.getLineNumber() << ", column " << remainingStream.getColumnNumber() << "\n";
		return;
	}
	State s;
	int vars[100];
	vars[97] = 3;
	s.vars = vars;
	std::cout << "Result = " << eval_prog(parseResult.getOutput(), &s) << "\n";
}

int main()
{
	std::cout << "Simple calculator powered by pcomb\n";
	while (true)
	{
		std::cout << "> ";

		auto lineStr = std::string();
		std::getline(std::cin, lineStr);

		if (lineStr.empty())
			break;

		parseLine(lineStr);
	}
	std::cout << "Bye bye!" << std::endl;
}