
#include "Firrtlator.h"
#include "FirrtlFrontend.h"

namespace Firrtlator {

Frontend::~Frontend() {

}

bool Firrtlator::parse(std::string::const_iterator begin,
		std::string::const_iterator end, std::string type) {
	if (type == "fir") {
		FirrtlFrontend frontend;
		return frontend.parseString(begin, end);
	}

	return false;
}

bool Firrtlator::parseFile(std::string filename, std::string type) {
    std::ifstream in(filename, std::ios_base::in);
    if (!in) {
        // TODO: log
        return false;
    }
    std::string str((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

    return parse(str.begin(), str.end(), type);
}

bool Firrtlator::parseString(std::string content, std::string type) {
	return parse(content.begin(), content.end(), type);
}

}
