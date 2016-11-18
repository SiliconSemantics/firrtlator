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

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

namespace Firrtlator {

class IRNode;
class Reference;
class Module;
class Stmt;
class Expression;
class Type;

class Info {
private:
	std::string mValue;
public:
	Info(std::string value = "") : mValue(value) {}
	void setValue(std::string value) { mValue = value; }
	std::string getValue() { return mValue; }
};

class IRNode {
public:
	virtual ~IRNode() {}
	IRNode() : IRNode("") {}
	IRNode(std::string id) : mId(id) {}

	std::string getId() {
		return mId;
	}

	void setId(std::string id) {
		mId = id;
	}

	std::shared_ptr<Info> getInfo() {
		return mInfo;
	}

	void setInfo(std::shared_ptr<Info> info) {
		mInfo = info;
	}

	bool isDeclaration() {
		return (mId.length() != 0);
	}

	inline void throwAssert(bool cond, std::string msg) {
		if (!cond)
			throw std::runtime_error(msg);
	}
protected:
	std::shared_ptr<Info> mInfo;
	std::string mId;
	std::vector<std::shared_ptr<IRNode> > mReferences;
};

class Circuit : public IRNode {
private:
	std::shared_ptr<Info> mInfo;
public:
	Circuit() {	}
	Circuit(std::string id)	: Circuit(id, nullptr) {}
	Circuit(std::string id, std::shared_ptr<Info> info)
	: IRNode(id), mInfo(info) {}
	void addModule(std::shared_ptr<Module> mod) {
		mModules.push_back(mod);
	}
	virtual void emit(std::ostream& os) const;
private:
	std::vector<std::shared_ptr<Module> > mModules;
};

class Port : public IRNode {
public:
	typedef enum { INPUT, OUTPUT } Direction;

	Port() : Port("", INPUT, nullptr) { }

	Port(std::string id, Direction dir, std::shared_ptr<Type> type)
	: IRNode(id), mDirection(dir), mType(type) {}

	void setDirection(Direction dir) {
		mDirection = dir;
	}
	Direction getDirection() {
		return mDirection;
	}
private:
	Direction mDirection;
	std::shared_ptr<Type> mType;
};

class Type : public IRNode {
public:
	typedef enum { INT, CLOCK, BUNDLE, VECTOR, UNDEFINED } Basetype;
	Type() : mBasetype(UNDEFINED) {}
	Type(Basetype type) : mBasetype(type) {}
protected:
	Basetype mBasetype;
};

class TypeInt : public Type {
public:
	TypeInt() : TypeInt(false) {}
	TypeInt(bool sign, int width = -1) : mWidth(width), mSigned(sign), Type(INT) {}

	void setWidth(int width) {
		mWidth = width;
	}
	int getWidth() {
		return mWidth;
	}

	bool getSigned() {
		return mSigned;
	}

	void setSigned(bool sign) {
		mSigned = sign;
	}
private:
	int mWidth;
	bool mSigned;
};

class TypeClock : public Type {
public:
	TypeClock() : Type(CLOCK) { }
};

class Field : public IRNode {
public:
	Field() : Field("", nullptr) {}
	Field(std::string id, std::shared_ptr<Type> type, bool flip = false)
	: mFlip(flip), mType(type), IRNode(id) {}

	void setType(std::shared_ptr<Type> t) {
		mType = t;
	}

	void setFlip(bool flip) {
		mFlip = flip;
	}
private:
	bool mFlip;
	std::shared_ptr<Type> mType;
};

class TypeBundle : public Type {
public:
	TypeBundle() : Type(BUNDLE) {}
	void addField(std::shared_ptr<Field> field) {
		mFields.push_back(field);
	}
private:
	std::vector<std::shared_ptr<Field> > mFields;
};

class TypeVector : public Type {
public:
	TypeVector() : mSize(0), Type(VECTOR) { }
	void setSize(int size) {
		mSize = size;
	}
private:
	std::shared_ptr<Type> mType;
	int mSize;
};

class Parameter : IRNode {
public:
	Parameter() {}
};

class Module : public IRNode {
public:
	Module() : Module("") {}
	Module(std::string id, bool external = false)
	: IRNode(id), mExternal(external) {}
	void addPort(std::shared_ptr<Port> port) {
		mPorts.push_back(port);
	}
	void addStmt(std::shared_ptr<Stmt> stmt) {
		throwAssert(!mExternal, "Cannot add statements to extmodule");

		mStmts.push_back(stmt);
	}
	void setDefname(std::string defname) {
		throwAssert(mExternal, "Cannot assign defname to module");

		mDefname = defname;
	}
	void addParameter(std::shared_ptr<Parameter> param) {
		throwAssert(mExternal, "Cannot add parameter to module");

		mParameters.push_back(param);
	}
private:
	bool mExternal;
	std::string mDefname;
	std::vector<std::shared_ptr<Port> > mPorts;
	std::vector<std::shared_ptr<Stmt> > mStmts;
	std::vector<std::shared_ptr<Parameter> > mParameters;
};

class Stmt : public IRNode {
public:
	Stmt() : Stmt("") {}
	Stmt(std::string id) : IRNode(id) {}
};

class Wire : public Stmt {
public:
	Wire() : Wire("", nullptr) {}
	Wire(std::string id, std::shared_ptr<Type> type)
	: mType(type), Stmt(id) {}
private:
	std::shared_ptr<Type> mType;
};

class Reg : public Stmt {
public:
	Reg() : Reg("", nullptr, nullptr) {}
	Reg(std::string id, std::shared_ptr<Type> type,
			std::shared_ptr<Expression> exp)
	: mType(type), mExp(exp), Stmt(id) {}
private:
	std::shared_ptr<Type> mType;
	std::shared_ptr<Expression> mExp;
};

class Memory : public Stmt {
public:
	typedef enum { OLD, NEW, UNDEFINED } RuwFlag;
	Memory(std::string id) : Stmt(id) {}
	void setDType(std::shared_ptr<Type> type) {
		throwAssert((type != nullptr), "Invalid memory type");
		throwAssert((mDType == nullptr), "Memory type already set");

		mDType = type;
	}
	void setDepth(int depth) {
		throwAssert((depth >= 0), "Invalid memory depth");
		throwAssert((mDepth == -1), "Memory depth already set");

		mDepth = depth;
	}
	void setReadLatency(int lat) {
		throwAssert((lat >= 0), "Invalid memory read latency");
		throwAssert((mReadlatency == -1), "Memory read latency already set");

		mDepth = lat;
	}
	void setWriteLatency(int lat) {
		throwAssert((lat >= 0), "Invalid memory write latency");
		throwAssert((mWritelatency == -1), "Memory write latency already set");

		mWritelatency = lat;
	}
	void setRuwFlag(RuwFlag flag) {
		throwAssert(!mRuwflagset, "Memory RUW flag already set");

		mRuwflagset = true;
		mRuwflag = flag;
	}
	void addReader(std::string r) {
		throwAssert((std::find(mReaders.begin(), mReaders.end(), (r))
			== mReaders.end()), "Reader already defined");

		if (!checkAndUpdateDeferedType())
			addReaderToType(r);

		mReaders.push_back(r);
	}
public:
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

	bool checkAndUpdateDeferedType() {
		// TODO: Add further defer reasons
		if (mDType == nullptr)
			return false;

		if (mType != nullptr)
			return true;

		mType = std::make_shared<TypeBundle>();

		for (auto r : mReaders)
			addReaderToType(r);
		for (auto w : mWriters)
			addWriterToType(w);
		for (auto rw : mReadWriters)
			addReadWriterToType(rw);

		return true;
	}

	void addReaderToType(std::string r) {
		std::shared_ptr<TypeBundle> bundle;
		bundle = std::make_shared<TypeBundle>();
		//bundle->addField(std::make_shared<Field>("data", TODO, true)); // Copy constructor
		//bundle->addField(std::make_shared<Field>("addr", TODO)); // Width inference
		bundle->addField(std::make_shared<Field>("en",
				std::make_shared<TypeInt>(false, 1)));
		bundle->addField(std::make_shared<Field>("clk",
				std::make_shared<TypeClock>()));
		mType->addField(std::make_shared<Field>(r, bundle));
	}

	void addWriterToType(std::string w) {
		std::shared_ptr<TypeBundle> bundle;
		bundle = std::make_shared<TypeBundle>();
		//bundle->addField(std::make_shared<Field>("data", TODO, true)); // Copy constructor
		//bundle->addField(std::make_shared<Field>("mask", TODO, true)); // Inference
		//bundle->addField(std::make_shared<Field>("addr", TODO)); // Width inference
		bundle->addField(std::make_shared<Field>("en",
				std::make_shared<TypeInt>(false, 1)));
		bundle->addField(std::make_shared<Field>("clk",
				std::make_shared<TypeClock>()));
		mType->addField(std::make_shared<Field>(w, bundle));
	}

	void addReadWriterToType(std::string rw) {
		std::shared_ptr<TypeBundle> bundle;
		bundle = std::make_shared<TypeBundle>();
		//bundle->addField(std::make_shared<Field>("data", TODO, true)); // Copy constructor
		//bundle->addField(std::make_shared<Field>("mask", TODO, true)); // Inference
		//bundle->addField(std::make_shared<Field>("addr", TODO)); // Width inference
		bundle->addField(std::make_shared<Field>("en",
				std::make_shared<TypeInt>(false, 1)));
		bundle->addField(std::make_shared<Field>("clk",
				std::make_shared<TypeClock>()));
		bundle->addField(std::make_shared<Field>("wmode",
				std::make_shared<TypeInt>(false, 1)));
		mType->addField(std::make_shared<Field>(rw, bundle));
	}
};

class Instance : public Stmt {
public:
	Instance() : Instance("", "") {}
	Instance(std::string id, std::string of)
	: mOfIdentifier(of), Stmt(id) {}
	Instance(std::string id, std::shared_ptr<Reference> of)
	: mOf(of), Stmt(id) {}
private:
	std::string mOfIdentifier;
	std::shared_ptr<Reference> mOf;
};

class Node : public Stmt {
public:
	Node() : Node("", nullptr) {}
	Node(std::string id, std::shared_ptr<Expression> expr)
	: mExpr(expr), Stmt(id) {}
private:
	std::shared_ptr<Expression> mExpr;
};

class Connect : public Stmt {
public:
	Connect() : Connect(nullptr, nullptr) {}
	Connect(std::shared_ptr<Expression> to,
			std::shared_ptr<Expression> from,
			bool partial = false)
	: mTo(to), mFrom(from), mPartial(partial) {}
private:
	std::shared_ptr<Expression> mTo;
	std::shared_ptr<Expression> mFrom;
	bool mPartial;
};

class Invalid : public Stmt {
public:
	Invalid() : Invalid(nullptr) {}
	Invalid(std::shared_ptr<Expression> exp)
	: mExp(exp) {}
private:
	std::shared_ptr<Expression> mExp;
};

class Conditional : public Stmt {
public:
	Conditional() : Conditional(nullptr) {}
	Conditional(std::shared_ptr<Expression> cond)
	: mCond(cond) {}
	void addIfStmt(std::shared_ptr<Stmt> stmt) {
		mIf.push_back(stmt);
	}
	void addIfStmtList(std::vector<std::shared_ptr<Stmt> > &stmts) {
		mIf.insert(std::end(mIf), std::begin(stmts), std::end(stmts));
	}
	void addElseStmt(std::shared_ptr<Stmt> stmt) {
		mElse.push_back(stmt);
	}
	void addElseStmtList(std::vector<std::shared_ptr<Stmt> > &stmts) {
		mElse.insert(std::end(mIf), std::begin(stmts), std::end(stmts));
	}
	void setElseInfo(std::shared_ptr<Info> info) {
		mElseInfo = info;
	}
private:
	std::shared_ptr<Expression> mCond;
	std::vector<std::shared_ptr<Stmt> > mIf;
	std::vector<std::shared_ptr<Stmt> > mElse;
	std::shared_ptr<Info> mElseInfo;
};

class Stop : public Stmt {
public:
	Stop() : Stop(nullptr, nullptr, -1) {}
	Stop(std::shared_ptr<Expression> clock,
			std::shared_ptr<Expression> cond,
			int code)
	: mClock(clock), mCond(cond), mCode(code) {}

private:
	std::shared_ptr<Expression> mClock;
	std::shared_ptr<Expression> mCond;
	int mCode;
};

class Printf : public Stmt {
public:
	Printf() : Printf(nullptr, nullptr, "") {}
	Printf(std::shared_ptr<Expression> clock,
			std::shared_ptr<Expression> cond,
			std::string format)
	: mClock(clock), mCond(cond), mFormat(format) {}
	void addArgument(std::shared_ptr<Expression> arg) {
		mArguments.push_back(arg);
	}
private:
	std::shared_ptr<Expression> mClock;
	std::shared_ptr<Expression> mCond;
	std::string mFormat;
	std::vector<std::shared_ptr<Expression> > mArguments;
};

class Empty : public Stmt {

};

class Expression : public IRNode {
public:
	typedef enum { MALE, FEMALE, BI } Gender;

	Expression() : mGender(MALE) {}
	Expression(Gender g) : mGender(g) {}

	Gender getGender() {
		return mGender;
	}
private:
	Gender mGender;
};

class Reference : public Expression {
public:
	Reference() : Reference("") {}
	Reference(std::string id) : mId(id), mTo(nullptr) {}
	bool isResolved() {
		return (mTo != nullptr && mTo != 0);
	}
private:
	std::shared_ptr<IRNode> mTo;
	std::string mId;
};

class Constant : public Expression {
public:
	typedef enum { UNDEFINED, INT, STRING } GenerateHint;
	Constant() : Constant(nullptr, -1, UNDEFINED) {}
	Constant(std::shared_ptr<TypeInt> type, int val,
			GenerateHint hint = INT)
	: mType(type), mVal(val), mHint(hint) {}
	Constant(std::shared_ptr<TypeInt> type, std::string val,
			GenerateHint hint = STRING)
	: mType(type), mHint(hint) {
		mVal = 0; //TODO: convert
	}
private:
	std::shared_ptr<TypeInt> mType;
	int mVal;
	GenerateHint mHint;
};

class SubField : public Expression {
public:
	SubField() : SubField(nullptr, nullptr) {}
	SubField(std::shared_ptr<Reference> id, std::shared_ptr<Expression> of)
	: mId(id), mOf(of) {}
private:
	std::shared_ptr<Expression> mOf;
	std::shared_ptr<Reference> mId;
};

class SubIndex : public Expression {
public:
	SubIndex() : SubIndex(-1, nullptr) {}
	SubIndex(int index, std::shared_ptr<Expression> of)
	: mIndex(index), mOf(of) {}
private:
	std::shared_ptr<Expression> mOf;
	int mIndex;
};

class SubAccess : public Expression {
public:
	SubAccess() : SubAccess(nullptr, nullptr) {}
	SubAccess(std::shared_ptr<Reference> expr,
			std::shared_ptr<Expression> of)
	: mOf(of), mExp(expr) {}
private:
	std::shared_ptr<Expression> mOf;
	std::shared_ptr<Reference> mExp;
};

class Mux : public Expression {
public:
	Mux() : Mux(nullptr, nullptr, nullptr) {}
	Mux(std::shared_ptr<Expression> sel, std::shared_ptr<Expression> a,
			std::shared_ptr<Expression> b)
	: mSel(sel), mA(a), mB(b) {}
private:
	std::shared_ptr<Expression> mSel;
	std::shared_ptr<Expression> mA;
	std::shared_ptr<Expression> mB;
};

class CondValid : public Expression {
public:
	CondValid() : CondValid(nullptr, nullptr) {}
	CondValid(std::shared_ptr<Expression> sel,
			std::shared_ptr<Expression> a)
	: mSel(sel), mA(a) {}
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

	void addOperand(std::shared_ptr<Expression> o) {
		if (mOperands.size() == mNumOperands) {
			throw std::runtime_error("Too many operands");
		}
		mOperands.push_back(o);
	}
	void addParameter(int p) {
		if (mParameters.size() == mNumParameters) {
			throw std::runtime_error("Too many parameters");
		}
		mParameters.push_back(p);
	}

	int numOperands() { return mNumOperands; }
	int numParameters() {return mNumParameters; }
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
