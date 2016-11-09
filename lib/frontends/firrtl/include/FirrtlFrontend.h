#pragma once

#include "FirrtlatorFrontend.h"

namespace Firrtlator {

class FirrtlFrontend : public Frontend {
public:
	virtual bool parseString(std::string::const_iterator begin,
			std::string::const_iterator end);
	virtual std::shared_ptr<Circuit> getIR();
};

}
