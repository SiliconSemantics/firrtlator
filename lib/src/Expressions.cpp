#include "IR.h"

#include <stdexcept>

namespace Firrtlator {

const bool PrimOp::lookup(std::string v, Operation &op) {
	static const std::map<std::string, Operation> map =	{
		{"add", ADD}, {"sub", SUB }, {"mul", MUL },
		{"div", DIV}, {"mod", MOD }, {"lt", LT },
		{"leq", LEQ}, {"gt", GT}, {"geq", GEQ }, {"eq", EQ},
	    {"neq", NEQ}, {"pad", PAD}, {"asUInt", ASUINT},
		{"asSInt", ASSINT}, {"asClock", ASCLOCK},
		{"shl", SHL}, {"shr", SHR}, {"dshl", DSHL},
		{"dshr", DSHR}, {"cvt", CVT}, {"neg", NEG},
		{"not", NOT}, {"and", AND}, {"or", OR},
		{"xor", XOR}, {"andr", ANDR}, {"orr", ORR},
		{"xorr", XORR}, {"cat", CAT}, {"bits", BITS},
		{"head", HEAD}, {"tail", TAIL}
	};

	if (map.find(v) == map.end()) {
		op = UNDEFINED;
		return false;
	}

	op = map.find(v)->second;
	return true;
}

PrimOp::PrimOp() : PrimOp(UNDEFINED, 0, 0) {}

PrimOp::PrimOp(Operation op, int numOps, int numParams) :
		Expression(MALE), mOp(op),
		mNumOperands(numOps), mNumParameters(numParams) {

}

std::shared_ptr<PrimOp> PrimOp::generate(const Operation &op) {
#define GENOP(x) case x: return std::make_shared<PrimOp##x>();
	switch (op) {
	GENOP(ADD);	GENOP(SUB);	GENOP(MUL);	GENOP(DIV);	GENOP(MOD);
	GENOP(LT); GENOP(LEQ); GENOP(GT); GENOP(GEQ); GENOP(EQ);
	GENOP(NEQ);	GENOP(PAD);	GENOP(ASUINT);GENOP(ASSINT); GENOP(ASCLOCK);
	GENOP(SHL);	GENOP(SHR);	GENOP(DSHL); GENOP(DSHR); GENOP(CVT);
	GENOP(NEG);	GENOP(NOT);	GENOP(AND);	GENOP(OR); GENOP(XOR);
	GENOP(ANDR); GENOP(ORR); GENOP(XORR); GENOP(CAT); GENOP(BITS);
	GENOP(HEAD); GENOP(TAIL);
	default:
		throw std::runtime_error("Unknown operation type");
	}
#undef GENOP
}

const std::string PrimOp::operationName(Operation op) {
#define GENOP(x,y) case x: return y;
	switch(op) {
	GENOP(ADD,"add"); GENOP(SUB,"sub"); GENOP(MUL,"mul"); GENOP(DIV,"div");
	GENOP(MOD,"mod"); GENOP(LT,"lt"); GENOP(LEQ,"leq"); GENOP(GT,"gt");
	GENOP(GEQ,"geq"); GENOP(EQ,"eq"); GENOP(NEQ,"neq"); GENOP(PAD,"pad");
	GENOP(ASUINT,"asUInt"); GENOP(ASSINT,"asSInt"); GENOP(ASCLOCK,"asClock");
	GENOP(SHL,"shl"); GENOP(SHR,"shr"); GENOP(DSHL,"dshl"); GENOP(DSHR,"dshr");
	GENOP(CVT,"cvt"); GENOP(NEG,"neg"); GENOP(NOT,"not"); GENOP(AND,"and");
	GENOP(OR,"or"); GENOP(XOR,"xor"); GENOP(ANDR,"andr"); GENOP(ORR,"orr");
	GENOP(XORR,"xorr"); GENOP(CAT,"cat"); GENOP(BITS,"bits");
	GENOP(HEAD,"head"); GENOP(TAIL,"tail");
	default:
		throw std::runtime_error("Unknown operation type");
	}
#undef GENOP
}

std::string PrimOp::operationName() {
	return operationName(mOp);
}

std::shared_ptr<PrimOp> PrimOp::generate(const std::string &s) {
	Operation op;
	if (!lookup(s, op)) {
		throw std::runtime_error("Unknown operation type");
	}

	return generate(op);
}

}
