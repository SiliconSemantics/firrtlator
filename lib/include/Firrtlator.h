#pragma once

#include <memory>

namespace Firrtlator {

class Firrtlator {
public:
	Firrtlator();
	~Firrtlator();
	bool parse(std::string::const_iterator begin,
			std::string::const_iterator end, std::string type = "");
	bool parseFile(std::string filename, std::string type = "");
	bool parseString(std::string string, std::string type = "");
private:
	class impl;
	std::unique_ptr<impl> pimpl;
};

}
