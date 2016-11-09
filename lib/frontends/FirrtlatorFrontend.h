#pragma once

#include <memory>
#include <string>
#include <fstream>

#include "IR.h"

namespace Firrtlator {

class Frontend {
public:
    virtual ~Frontend();
    virtual bool parseString(std::string::const_iterator begin,
                             std::string::const_iterator end) = 0;
    virtual std::shared_ptr<Circuit> getIR() = 0;
};

}
