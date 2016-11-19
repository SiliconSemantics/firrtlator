/*
 * Copyright (c) 2016 Stefan Wallentowitz <wallento@silicon-semantics.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "IR.h"
#include "IndentationBuffer.h"
#include "Util.h"

namespace Firrtlator {

Stmt::Stmt() : Stmt("") {}

Stmt::Stmt(std::string id) : IRNode(id) {}

Wire::Wire() : Wire("", nullptr) {}

Wire::Wire(std::string id, std::shared_ptr<Type> type)
: mType(type), Stmt(id) {}

Reg::Reg() : Reg("", nullptr, nullptr) {}

Reg::Reg(std::string id, std::shared_ptr<Type> type,
		std::shared_ptr<Expression> exp)
: mType(type), mExp(exp), Stmt(id) {}

Instance::Instance() : Instance("", "") {}

Instance::Instance(std::string id, std::string of)
: mOfIdentifier(of), Stmt(id) {}

Instance::Instance(std::string id, std::shared_ptr<Reference> of)
: mOf(of), Stmt(id) {}

Node::Node() : Node("", nullptr) {}

Node::Node(std::string id, std::shared_ptr<Expression> expr)
: mExpr(expr), Stmt(id) {}

Connect::Connect() : Connect(nullptr, nullptr) {}

Connect::Connect(std::shared_ptr<Expression> to,
		std::shared_ptr<Expression> from,
		bool partial)
: mTo(to), mFrom(from), mPartial(partial) {}

Invalid::Invalid() : Invalid(nullptr) {}

Invalid::Invalid(std::shared_ptr<Expression> exp)
: mExp(exp) {}

Conditional::Conditional() : Conditional(nullptr) {}

Conditional::Conditional(std::shared_ptr<Expression> cond)
: mCond(cond) {}

void Conditional::addIfStmt(std::shared_ptr<Stmt> stmt) {
	mIf.push_back(stmt);
}

void Conditional::addIfStmtList(std::vector<std::shared_ptr<Stmt> > &stmts) {
	mIf.insert(std::end(mIf), std::begin(stmts), std::end(stmts));
}

void Conditional::addElseStmt(std::shared_ptr<Stmt> stmt) {
	mElse.push_back(stmt);
}

void Conditional::addElseStmtList(std::vector<std::shared_ptr<Stmt> > &stmts) {
	mElse.insert(std::end(mIf), std::begin(stmts), std::end(stmts));
}
void Conditional::setElseInfo(std::shared_ptr<Info> info) {
	mElseInfo = info;
}

Stop::Stop() : Stop(nullptr, nullptr, -1) {}

Stop::Stop(std::shared_ptr<Expression> clock,
		std::shared_ptr<Expression> cond,
		int code)
: mClock(clock), mCond(cond), mCode(code) {}

Printf::Printf() : Printf(nullptr, nullptr, "") {}
Printf::Printf(std::shared_ptr<Expression> clock,
		std::shared_ptr<Expression> cond,
		std::string format)
: mClock(clock), mCond(cond), mFormat(format) {}
void Printf::addArgument(std::shared_ptr<Expression> arg) {
	mArguments.push_back(arg);
}

}
