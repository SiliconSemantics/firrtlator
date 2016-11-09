#include <IR.h>
#include <Firrtlator.h>

#include <iostream>

int main() {
	//std::string test = "circuit test : @[\"abc.v 123\"]";

	Firrtlator::Firrtlator firrtlator;

	if (!firrtlator.parseFile("test.fir", "fir")) {
		std::cout << "Failed parsing" << std::endl;
	}
}
