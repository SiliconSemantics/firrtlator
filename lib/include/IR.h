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

namespace Firrtlator {

class AstNode;
class Declaration;
class Module;

class Info {
private:
	std::string mValue;
public:
	Info(std::string value = "") : mValue(value) {}
	void setValue(std::string value) { mValue = value; }
	std::string getValue() { return mValue; }
};

class AstNode {
protected:
	std::shared_ptr<Info> mInfo;
public:
	std::shared_ptr<Info> getInfo() {
		return mInfo;
	}

	void setInfo(std::shared_ptr<Info> info) {
		mInfo = info;
	}
};

class Declaration : public AstNode {
protected:
	std::string mId;
	std::vector<std::shared_ptr<AstNode> > mReferences;
public:
	Declaration() {	}
	Declaration(std::string id) : mId(id) {}

	std::string getId() {
		return mId;
	}

	void setId(std::string id) {
		mId = id;
	}
};

class Circuit : public Declaration {
private:
	std::shared_ptr<Info> mInfo;
public:
	Circuit() {	}
	Circuit(std::string id)	: Declaration(id), mInfo() {

	}
	Circuit(std::string id, std::shared_ptr<Info> info)
	: Declaration(id), mInfo(info) {

	}
	void addModule(std::shared_ptr<Module> mod) {

	}
};

class Port : public Declaration {
public:
	typedef enum { INPUT, OUTPUT } Direction;

	Port() { mDirection = INPUT; }

	Port(std::string id, Direction dir)
	: Declaration(id), mDirection(dir) {}

	Port(Direction dir)
	: mDirection(dir) {

	}

	void setDirection(Direction dir) {
		mDirection = dir;
	}
	Direction getDirection() {
		return mDirection;
	}
private:
	Direction mDirection;
};

class Type : public AstNode {
public:
	typedef enum { INT, CLOCK, BUNDLE, VECTOR, UNDEFINED } Basetype;
	Type() : mBasetype(UNDEFINED) {}
	Type(Basetype type) : mBasetype(type) {}
protected:
	Basetype mBasetype;
};

class TypeInt : public Type {
public:
	TypeInt() : mWidth(32), mSigned(true), Type(INT) { }
	TypeInt(int width) : mWidth(width), mSigned(true), Type(INT) {}
	TypeInt(bool sign) : mWidth(1), mSigned(sign), Type(INT) {}

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

class Field : public Declaration {
public:
	Field() : mFlip(false) {}
	Field(bool flip) : mFlip(flip) {}
	Field(std::shared_ptr<Type> t) : mFlip(false), mType(t) {}

	void setType(std::shared_ptr<Type> t) {
		mType = t;
	}

	void setFlip(bool flip) {
		mFlip = flip;
	}
private:
	bool mFlip;
	std::string mId;
	std::shared_ptr<Type> mType;
};

class TypeBundle : public Type {
public:
	TypeBundle() : Type(BUNDLE) {}
	TypeBundle(std::shared_ptr<Field> field) : Type(BUNDLE) { mFields.push_back(field); }
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

class Parameter : AstNode {
public:
	Parameter() {}
};

class Module : public Declaration {
public:
	Module() : mExternal(true) {}
	Module(std::string id, bool external = false)
	: Declaration(id), mExternal(external) {}
	void addPort(std::shared_ptr<Port> port) {
		mPorts.push_back(port);
	}
private:
	bool mExternal;
	std::vector<std::shared_ptr<Port> > mPorts;
};

class Stmt : public AstNode {
public:
	bool isValid() {
		return mValid;
	}
private:
	bool mValid;
};

class Wire : public Stmt {

};

class Reg : public Stmt {

};

struct MemoryCharacteristics {
	std::shared_ptr<Type> type;
	int depth;
	int readlatency;
	int writelatency;
	typedef enum { OLD, NEW, UNDEFINED } RuwFlag;
	RuwFlag ruwflag;
};

class Memory : public Stmt {
public:

};

class Instance : public Stmt {

};

class Node : public Stmt {

};

class Connect : public Stmt {
private:
	bool mPartial;
};

class Invalid : public Stmt {

};

class Conditional : public Stmt {

};

class Stop : public Stmt {

};

class Printf : public Stmt {

};

class Empty : public Stmt {

};

class StmtGroup : public Stmt {
private:
	std::vector<std::shared_ptr<Stmt> > mGroup;
};

class Expression : public AstNode {
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

class Int : public Expression {
private:
	bool mSigned;
	int mWidth;
	int value;
};

class Reference : public Expression {
private:
	bool mResolved;
	std::shared_ptr<Declaration> mTo;
	std::string mId;
};

class SubField : public Expression {
private:
	std::shared_ptr<Expression> mOf;
	std::string mId;
};

class SubIndex : public Expression {
private:
	std::shared_ptr<Expression> mOf;
	int mIndex;
};

class SubAccess : public Expression {
private:
	std::shared_ptr<Expression> mOf;
	std::shared_ptr<Expression> mExp;
};

class Mux : public Expression {
private:
	std::shared_ptr<Expression> mSel;
	std::shared_ptr<Expression> mA;
	std::shared_ptr<Expression> mB;
};

class CondValid : public Expression {
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

	static const bool lookup(std::string v, Operation &op);

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
