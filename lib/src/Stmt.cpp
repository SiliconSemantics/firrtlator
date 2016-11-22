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

#include "../backends/generic/include/StreamIndentation.h"
#include "IR.h"
#include "Util.h"
#include "Visitor.h"

namespace Firrtlator {

Stmt::Stmt() : Stmt("") {}

Stmt::Stmt(std::string id) : IRNode(id) {}

StmtGroup::StmtGroup() {}

StmtGroup::StmtGroup(std::shared_ptr<Stmt> stmt) {
	mGroup.push_back(stmt);
}


StmtGroup::StmtGroup(std::vector<std::shared_ptr<Stmt> > group) {
	mGroup = group;
}

void StmtGroup::addStatement(std::shared_ptr<Stmt> stmt) {
	mGroup.push_back(stmt);
}

StmtGroup::iterator StmtGroup::begin() {
	return mGroup.begin();
}

StmtGroup::iterator StmtGroup::end() {
	return mGroup.end();
}

void StmtGroup::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	for (auto s : mGroup)
		s->accept(v);

	v.leave(*this);
}


Wire::Wire() : Wire("", nullptr) {}

Wire::Wire(std::string id, std::shared_ptr<Type> type)
: Stmt(id), mType(type) {}

std::shared_ptr<Type> Wire::getType() {
	return mType;
}

void Wire::accept(Visitor& v) {
	if(!v.visit(*this))
		return;

	mType->accept(v);

	v.leave(*this);
}

Reg::Reg() : Reg("", nullptr, nullptr) {}

Reg::Reg(std::string id, std::shared_ptr<Type> type,
		std::shared_ptr<Expression> clock)
: Stmt(id), mType(type), mClock(clock) {}

std::shared_ptr<Type> Reg::getType() {
	return mType;
}

std::shared_ptr<Expression> Reg::getClock() {
	return mClock;
}

void Reg::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mType->accept(v);
	mClock->accept(v);

	if (mResetTrigger && mResetValue) {
		mResetTrigger->accept(v);
		mResetValue->accept(v);
	}
}

void Reg::setResetTrigger(std::shared_ptr<Expression> trigger) {
	mResetTrigger = trigger;
}

void Reg::setResetValue(std::shared_ptr<Expression> value) {
	mResetValue = value;
}

std::shared_ptr<Expression> Reg::getResetTrigger() {
	return mResetTrigger;
}

std::shared_ptr<Expression> Reg::getResetValue() {
	return mResetValue;
}


Instance::Instance() : Instance("", nullptr) {}

Instance::Instance(std::string id, std::shared_ptr<Reference> of)
: Stmt(id), mOf(of) {}

std::shared_ptr<Reference> Instance::getOf() {
	return mOf;
}

void Instance::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mOf->accept(v);

	v.leave(*this);
}

Node::Node() : Node("", nullptr) {}

Node::Node(std::string id, std::shared_ptr<Expression> expr)
: Stmt(id), mExpr(expr) {}

std::shared_ptr<Expression> Node::getExpression() {
	return mExpr;
}

void Node::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mExpr->accept(v);

	v.leave(*this);
}

Connect::Connect() : Connect(nullptr, nullptr) {}

Connect::Connect(std::shared_ptr<Expression> to,
		std::shared_ptr<Expression> from,
		bool partial)
: mTo(to), mFrom(from), mPartial(partial) {}

bool Connect::getPartial() {
	return mPartial;
}

std::shared_ptr<Expression> Connect::getTo() {
	return mTo;
}

std::shared_ptr<Expression> Connect::getFrom() {
	return mFrom;
}

void Connect::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mTo->accept(v);
	mFrom->accept(v);

	v.leave(*this);
}

Invalid::Invalid() : Invalid(nullptr) {}

Invalid::Invalid(std::shared_ptr<Expression> exp)
: mExp(exp) {}

std::shared_ptr<Expression> Invalid::getExpr() {
	return mExp;
}

void Invalid::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mExp->accept(v);

	v.leave(*this);
}

Conditional::Conditional() : Conditional(nullptr) {}

Conditional::Conditional(std::shared_ptr<Expression> cond)
: mCond(cond) {}

void Conditional::setThen(std::shared_ptr<StmtGroup> stmt) {
	mThen = stmt;
}

void Conditional::setElse(std::shared_ptr<ConditionalElse> e) {
	mElse = e;
}

std::shared_ptr<Expression> Conditional::getCondition() {
	return mCond;
}

std::shared_ptr<StmtGroup> Conditional::getThen() {
	return mThen;
}

std::shared_ptr<ConditionalElse> Conditional::getElse() {
	return mElse;
}

void Conditional::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mCond->accept(v);

	mThen->accept(v);

	if (mElse)
		mElse->accept(v);

	v.leave(*this);
}

ConditionalElse::ConditionalElse() {}

ConditionalElse::ConditionalElse(std::shared_ptr<StmtGroup> stmts)
: mStmts(stmts) {}

void ConditionalElse::setStmts(std::shared_ptr<StmtGroup> stmt) {
	mStmts = stmt;
}

std::shared_ptr<StmtGroup> ConditionalElse::getStmts() {
	return mStmts;
}

void ConditionalElse::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mStmts->accept(v);

	v.leave(*this);
}

Stop::Stop() : Stop(nullptr, nullptr, -1) {}

Stop::Stop(std::shared_ptr<Expression> clock,
		std::shared_ptr<Expression> cond,
		int code)
: mClock(clock), mCond(cond), mCode(code) {}

std::shared_ptr<Expression> Stop::getClock() {
	return mClock;
}

std::shared_ptr<Expression> Stop::getCondition() {
	return mCond;
}

int Stop::getCode() {
	return mCode;
}

void Stop::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mClock->accept(v);
	mCond->accept(v);

	v.leave(*this);
}

Printf::Printf() : Printf(nullptr, nullptr, "") {}
Printf::Printf(std::shared_ptr<Expression> clock,
		std::shared_ptr<Expression> cond,
		std::string format)
: mClock(clock), mCond(cond), mFormat(format) {}
void Printf::addArgument(std::shared_ptr<Expression> arg) {
	mArguments.push_back(arg);
}

std::shared_ptr<Expression> Printf::getClock() {
	return mClock;
}

std::shared_ptr<Expression> Printf::getCondition() {
	return mCond;
}

std::string Printf::getFormat() {
	return mFormat;
}

std::vector<std::shared_ptr<Expression> > Printf::getArguments() {
	return mArguments;
}

void Printf::accept(Visitor& v) {
	if (!v.visit(*this))
		return;

	mClock->accept(v);
	mCond->accept(v);

	for (auto a : mArguments)
		a->accept(v);

	v.leave(*this);
}

void Empty::accept(Visitor& v) {
	v.visit(*this);
}


}
