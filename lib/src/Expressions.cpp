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
}

std::shared_ptr<PrimOp> PrimOp::generate(const std::string &s) {
	Operation op;
	if (!lookup(s, op)) {
		throw std::runtime_error("Unknown operation type");
	}

	return generate(op);
}

}
