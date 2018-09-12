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
	rule(seq(token(regex("\\w+")), token(ch('(')), many(seq(token(ch(',')), expr0.getRef())), token(ch(')'))), [] (auto call) -> Expr* { 
		Expr * e = (Expr*) malloc(sizeof(Expr));
		Call * c = (Call*) malloc(sizeof(Call));
		e->type = TCall;
		e->p = c;
		c->fun = std::get<0>(call).to_string()[0]; //TO-DO
		auto& paramVec = std::get<2>(call);
		Param ** par = &(c->params);
		for (auto& param: paramVec) {
			*par = (Param*) malloc(sizeof(Param));
			(*par)->e = std::get<1>(param);
			par = &((*par)->next);
		}
		return e;
	}),
	rule(token(regex("\\w+")), [] (auto id) -> Expr* { 
		Expr * e = (Expr*) malloc(sizeof(Expr));
		Var * c = (Var*) malloc(sizeof(Var));
		e->type = TVar;
		e->p = c;
		c->var = id.to_string()[0]; //TO-DO
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

auto parser = bigstr(expr);

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
	std::cout << "Result = " << eval_expr(parseResult.getOutput(), &s) << "\n";
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