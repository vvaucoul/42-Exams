#pragma once

#include "ATarget.hpp"
#include <vector>

class TargetGenerator
{
private:
    std::vector<ATarget *> _targets;

    TargetGenerator(TargetGenerator const &other);
    TargetGenerator &operator=(TargetGenerator const &other);

public:
    TargetGenerator();
    ~TargetGenerator();

    void learnTargetType(ATarget *target_ptr);
    void forgetTargetType(std::string const &type);
    ATarget* createTarget(std::string const &type);
};
