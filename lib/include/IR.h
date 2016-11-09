#pragma once

#include <string>
#include <memory>
#include <vector>

namespace Firrtlator {

class Vertex;
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

class Vertex {
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

class Declaration : public Vertex {
protected:
	std::string mId;
	std::vector<std::shared_ptr<Vertex> > mReferences;
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

class Type : public Vertex {
public:
	typedef enum { INT, CLOCK, BUNDLE, VECTOR } Basetype;
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

class Field : public Vertex {
public:
	Field() : mFlip(false) {}
	Field(bool flip) : mFlip(flip) {}
	Field(std::shared_ptr<Type> t) : mFlip(false), mType(t) {}

	void setType(std::shared_ptr<Type> t) {
		mType = t;
	}
private:
	bool mFlip;
	std::string mId;
	std::shared_ptr<Type> mType;
};

class TypeBundle : public Type {
public:
	TypeBundle() : Type(BUNDLE) {}
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

class Stmt : public Vertex {
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

class Memory : public Stmt {
public:
	enum { OLD, NEW, UNDEFINED } RuwFlag;
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

class Expression : public Vertex {
	typedef enum { MALE, FEMALE, BI } Gender;

	Expression() : mGender(BI) {}

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
		XOR, ANDR, ORR, XORR, CAT, BITS, HEAD, TAIL
	} Operation;
private:
	Operation mOp;
	std::vector<std::shared_ptr<Expression> > mExps;
	std::vector<int> mInts;
};

}
