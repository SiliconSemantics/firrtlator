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

#pragma once

#include "Util.h"

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

namespace Firrtlator {

class Info;
class IRNode;
class Reference;
class Module;
class Stmt;
class Expression;
class Type;
class Visitor;

class Info {
private:
	std::string mValue;
public:
	Info(std::string value = "");
	void setValue(std::string value);
	std::string getValue();
	friend std::ostream& operator<< (std::ostream &out, const Info &Info);
};

class IRNode {
public:
	virtual ~IRNode();
	IRNode();
	IRNode(std::string id);
	std::string getId();
	void setId(std::string id);
	std::shared_ptr<Info> getInfo();
	void setInfo(std::shared_ptr<Info> info);
	bool isDeclaration();
	virtual void emit(std::ostream& os) const {/*TODO: =0 once implemented*/};
	virtual void accept(Visitor& v) = 0;
protected:
	std::shared_ptr<Info> mInfo;
	std::string mId;
	std::vector<std::shared_ptr<IRNode> > mReferences;
};

std::ostream& operator<< (std::ostream& os, const ::Firrtlator::IRNode& n);

class Circuit : public IRNode {
public:
	Circuit();
	Circuit(std::string id);
	Circuit(std::string id, std::shared_ptr<Info> info);
	void addModule(std::shared_ptr<Module> mod);
	virtual void emit(std::ostream& os) const;
	virtual void accept(Visitor& v);
private:
	std::vector<std::shared_ptr<Module> > mModules;
};

class Port : public IRNode {
public:
	typedef enum { INPUT, OUTPUT } Direction;
	Port();
	Port(std::string id, Direction dir, std::shared_ptr<Type> type);
	void setDirection(Direction dir);
	Direction getDirection();
	virtual void emit(std::ostream& os) const;
	virtual void accept(Visitor& v);
private:
	Direction mDirection;
	std::shared_ptr<Type> mType;
};

class Type : public IRNode {
public:
	typedef enum { INT, CLOCK, BUNDLE, VECTOR, UNDEFINED } Basetype;
	Type();
	Type(Basetype type);
	virtual void accept(Visitor& v) = 0;
protected:
	Basetype mBasetype;
};

class TypeInt : public Type {
public:
	TypeInt();
	TypeInt(bool sign, int width = -1);

	void setWidth(int width);
	int getWidth();

	bool getSigned();
	void setSigned(bool sign);
	virtual void accept(Visitor& v);
private:
	int mWidth;
	bool mSigned;
};

class TypeClock : public Type {
public:
	TypeClock();
	virtual void accept(Visitor& v);
};

class Field : public IRNode {
public:
	Field();
	Field(std::string id, std::shared_ptr<Type> type, bool flip = false);

	void setType(std::shared_ptr<Type> t);
	void setFlip(bool flip);
	virtual void accept(Visitor& v);
private:
	bool mFlip;
	std::shared_ptr<Type> mType;
};

class TypeBundle : public Type {
public:
	TypeBundle();
	void addField(std::shared_ptr<Field> field);
	virtual void accept(Visitor& v);
private:
	std::vector<std::shared_ptr<Field> > mFields;
};

class TypeVector : public Type {
public:
	TypeVector();
	void setSize(int size);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Type> mType;
	int mSize;
};

class Parameter : IRNode {
public:
	Parameter();
	virtual void accept(Visitor& v);
};

class Module : public IRNode {
public:
	Module();
	Module(std::string id, bool external = false);
	void addPort(std::shared_ptr<Port> port);
	void addStmt(std::shared_ptr<Stmt> stmt);
	void setDefname(std::string defname);
	void addParameter(std::shared_ptr<Parameter> param);
	virtual void emit(std::ostream& os) const;
	virtual void accept(Visitor& v);
private:
	bool mExternal;
	std::string mDefname;
	std::vector<std::shared_ptr<Port> > mPorts;
	std::vector<std::shared_ptr<Stmt> > mStmts;
	std::vector<std::shared_ptr<Parameter> > mParameters;
};

class Stmt : public IRNode {
public:
	Stmt();
	Stmt(std::string id);
	virtual void accept(Visitor& v) = 0;
};

class Wire : public Stmt {
public:
	Wire();
	Wire(std::string id, std::shared_ptr<Type> type);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Type> mType;
};

class Reg : public Stmt {
public:
	Reg();
	Reg(std::string id, std::shared_ptr<Type> type,
			std::shared_ptr<Expression> exp);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Type> mType;
	std::shared_ptr<Expression> mExp;
};

class Memory : public Stmt {
public:
	typedef enum { OLD, NEW, UNDEFINED } RuwFlag;
	Memory(std::string id);
	void setDType(std::shared_ptr<Type> type);
	void setDepth(int depth);
	void setReadLatency(int lat);
	void setWriteLatency(int lat);
	void setRuwFlag(RuwFlag flag);
	void addReader(std::string r);
	virtual void accept(Visitor& v);
protected:
	std::shared_ptr<Type> mDType = nullptr;
	std::shared_ptr<TypeBundle> mType = nullptr;
	std::vector<std::string> mReaders;
	std::vector<std::string> mWriters;
	std::vector<std::string> mReadWriters;
	int mDepth = -1;
	int mReadlatency = -1;
	int mWritelatency = -1;
	bool mRuwflagset = false;
	RuwFlag mRuwflag = UNDEFINED;

	bool checkAndUpdateDeferedType();
	void addReaderToType(std::string r);
	void addWriterToType(std::string w);
	void addReadWriterToType(std::string rw);
};

class Instance : public Stmt {
public:
	Instance();
	Instance(std::string id, std::string of);
	Instance(std::string id, std::shared_ptr<Reference> of);
	virtual void accept(Visitor& v);
private:
	std::string mOfIdentifier;
	std::shared_ptr<Reference> mOf;
};

class Node : public Stmt {
public:
	Node();
	Node(std::string id, std::shared_ptr<Expression> expr);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mExpr;
};

class Connect : public Stmt {
public:
	Connect();
	Connect(std::shared_ptr<Expression> to,
			std::shared_ptr<Expression> from,
			bool partial = false);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mTo;
	std::shared_ptr<Expression> mFrom;
	bool mPartial;
};

class Invalid : public Stmt {
public:
	Invalid();
	Invalid(std::shared_ptr<Expression> exp);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mExp;
};

class Conditional : public Stmt {
public:
	Conditional();
	Conditional(std::shared_ptr<Expression> cond);
	void addIfStmt(std::shared_ptr<Stmt> stmt);
	void addIfStmtList(std::vector<std::shared_ptr<Stmt> > &stmts);
	void addElseStmt(std::shared_ptr<Stmt> stmt);
	void addElseStmtList(std::vector<std::shared_ptr<Stmt> > &stmts);
	void setElseInfo(std::shared_ptr<Info> info);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mCond;
	std::vector<std::shared_ptr<Stmt> > mIf;
	std::vector<std::shared_ptr<Stmt> > mElse;
	std::shared_ptr<Info> mElseInfo;
};

class Stop : public Stmt {
public:
	Stop();
	Stop(std::shared_ptr<Expression> clock,
			std::shared_ptr<Expression> cond,
			int code);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mClock;
	std::shared_ptr<Expression> mCond;
	int mCode;
};

class Printf : public Stmt {
public:
	Printf();
	Printf(std::shared_ptr<Expression> clock,
			std::shared_ptr<Expression> cond,
			std::string format);
	void addArgument(std::shared_ptr<Expression> arg);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mClock;
	std::shared_ptr<Expression> mCond;
	std::string mFormat;
	std::vector<std::shared_ptr<Expression> > mArguments;
};

class Empty : public Stmt {
public:
	virtual void accept(Visitor& v);
};

class Expression : public IRNode {
public:
	typedef enum { MALE, FEMALE, BI } Gender;
	Expression();
	Expression(Gender g);
	Gender getGender();
	virtual void accept(Visitor& v) = 0;
private:
	Gender mGender;
};

class Reference : public Expression {
public:
	Reference();
	Reference(std::string id);
	bool isResolved();
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<IRNode> mTo;
	std::string mId;
};

class Constant : public Expression {
public:
	typedef enum { UNDEFINED, INT, STRING } GenerateHint;
	Constant();
	Constant(std::shared_ptr<TypeInt> type, int val,
			GenerateHint hint = INT);
	Constant(std::shared_ptr<TypeInt> type, std::string val,
			GenerateHint hint = STRING);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<TypeInt> mType;
	int mVal;
	GenerateHint mHint;
};

class SubField : public Expression {
public:
	SubField();
	SubField(std::shared_ptr<Reference> id, std::shared_ptr<Expression> of);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mOf;
	std::shared_ptr<Reference> mId;
};

class SubIndex : public Expression {
public:
	SubIndex();
	SubIndex(int index, std::shared_ptr<Expression> of);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mOf;
	int mIndex;
};

class SubAccess : public Expression {
public:
	SubAccess();
	SubAccess(std::shared_ptr<Reference> expr,
			std::shared_ptr<Expression> of);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mOf;
	std::shared_ptr<Reference> mExp;
};

class Mux : public Expression {
public:
	Mux();
	Mux(std::shared_ptr<Expression> sel, std::shared_ptr<Expression> a,
			std::shared_ptr<Expression> b);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mSel;
	std::shared_ptr<Expression> mA;
	std::shared_ptr<Expression> mB;
};

class CondValid : public Expression {
public:
	CondValid();
	CondValid(std::shared_ptr<Expression> sel,
			std::shared_ptr<Expression> a);
	virtual void accept(Visitor& v);
private:
	std::shared_ptr<Expression> mSel;
	std::shared_ptr<Expression> mA;
};


class PrimOp : public Expression {
public:
	typedef enum {
		ADD, SUB, MUL, DIV, MOD, LT, LEQ, GT, GEQ, EQ, NEQ, PAD, ASUINT,
		ASSINT, ASCLOCK, SHL, SHR, DSHL, DSHR, CVT, NEG, NOT, AND, OR,
		XOR, ANDR, ORR, XORR, CAT, BITS, HEAD, TAIL, UNDEFINED
	} Operation;

	PrimOp();
	PrimOp(Operation op, int numOp, int numParam);
	virtual ~PrimOp() {}

	static const bool lookup(std::string v, Operation &op);
	std::string operationName();
	static const std::string operationName(Operation op);

	static std::shared_ptr<PrimOp> generate(const Operation &op);
	static std::shared_ptr<PrimOp> generate(const std::string &op);

	void addOperand(std::shared_ptr<Expression> o);
	void addParameter(int p);

	int numOperands() { return mNumOperands; }
	int numParameters() {return mNumParameters; }
	virtual void accept(Visitor& v);
protected:
	Operation mOp;
	std::vector<std::shared_ptr<Expression> > mOperands;
	std::vector<int> mParameters;

	int mNumOperands;
	int mNumParameters;
};

class PrimOpADD : public PrimOp {
public:
	PrimOpADD() : PrimOp(ADD, 2, 0) {}
};

class PrimOpSUB : public PrimOp {
public:
	PrimOpSUB() : PrimOp(SUB, 2, 0) {}
};

class PrimOpMUL : public PrimOp {
public:
	PrimOpMUL() : PrimOp(MUL, 2, 0) {}
};

class PrimOpDIV : public PrimOp {
public:
	PrimOpDIV() : PrimOp(DIV, 2, 0) {}
};

class PrimOpMOD : public PrimOp {
public:
	PrimOpMOD() : PrimOp(MOD, 2, 0) {}
};

class PrimOpLT : public PrimOp {
public:
	PrimOpLT() : PrimOp(LT, 2, 0) {}
};

class PrimOpLEQ : public PrimOp {
public:
	PrimOpLEQ() : PrimOp(LEQ, 2, 0) {}
};

class PrimOpGT : public PrimOp {
public:
	PrimOpGT() : PrimOp(GT, 2, 0) {}
};

class PrimOpGEQ : public PrimOp {
public:
	PrimOpGEQ() : PrimOp(GEQ, 2, 0) {}
};

class PrimOpEQ : public PrimOp {
public:
	PrimOpEQ() : PrimOp(EQ, 2, 0) {}
};

class PrimOpNEQ : public PrimOp {
public:
	PrimOpNEQ() : PrimOp(NEQ, 2, 0) {}
};

class PrimOpPAD : public PrimOp {
public:
	PrimOpPAD() : PrimOp(ADD, 1, 1) {}
};

class PrimOpASUINT : public PrimOp {
public:
	PrimOpASUINT() : PrimOp(ASUINT, 1, 0) {}
};

class PrimOpASSINT : public PrimOp {
public:
	PrimOpASSINT() : PrimOp(ASSINT, 1, 0) {}
};

class PrimOpASCLOCK : public PrimOp {
public:
	PrimOpASCLOCK() : PrimOp(ASCLOCK, 1, 0) {}
};

class PrimOpSHL : public PrimOp {
public:
	PrimOpSHL() : PrimOp(SHL, 1, 1) {}
};

class PrimOpSHR : public PrimOp {
public:
	PrimOpSHR() : PrimOp(SHR, 1, 1) {}
};

class PrimOpDSHL : public PrimOp {
public:
	PrimOpDSHL() : PrimOp(DSHL, 2, 0) {}
};

class PrimOpDSHR : public PrimOp {
public:
	PrimOpDSHR() : PrimOp(DSHR, 2, 0) {}
};

class PrimOpCVT : public PrimOp {
public:
	PrimOpCVT() : PrimOp(CVT, 1, 0) {}
};

class PrimOpNEG : public PrimOp {
public:
	PrimOpNEG() : PrimOp(NEG, 1, 0) {}
};

class PrimOpNOT : public PrimOp {
public:
	PrimOpNOT() : PrimOp(NOT, 1, 0) {}
};

class PrimOpAND : public PrimOp {
public:
	PrimOpAND() : PrimOp(ADD, 2, 0) {}
};

class PrimOpOR : public PrimOp {
public:
	PrimOpOR() : PrimOp(OR, 2, 0) {}
};

class PrimOpXOR : public PrimOp {
public:
	PrimOpXOR() : PrimOp(XOR, 2, 0) {}
};

class PrimOpANDR : public PrimOp {
public:
	PrimOpANDR() : PrimOp(ANDR, 1, 0) {}
};

class PrimOpORR : public PrimOp {
public:
	PrimOpORR() : PrimOp(ORR, 1, 0) {}
};

class PrimOpXORR : public PrimOp {
public:
	PrimOpXORR() : PrimOp(XORR, 1, 0) {}
};

class PrimOpCAT : public PrimOp {
public:
	PrimOpCAT() : PrimOp(CAT, 2, 0) {}
};

class PrimOpBITS : public PrimOp {
public:
	PrimOpBITS() : PrimOp(BITS, 1, 2) {}
};

class PrimOpHEAD : public PrimOp {
public:
	PrimOpHEAD() : PrimOp(HEAD, 1, 1) {}
};

class PrimOpTAIL : public PrimOp {
public:
	PrimOpTAIL() : PrimOp(TAIL, 1, 1) {}
};

}
