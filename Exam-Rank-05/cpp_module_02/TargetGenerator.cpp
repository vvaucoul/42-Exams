#include "TargetGenerator.hpp"

TargetGenerator::TargetGenerator()
{}

TargetGenerator::~TargetGenerator()
{
    for (std::vector<ATarget *>::iterator it = _targets.begin(); it != _targets.end(); ++it) {
        delete *it;
    }
    _targets.clear();
}

void TargetGenerator::learnTargetType(ATarget *target)
{
    if (target) {
        for (std::vector<ATarget *>::iterator it = _targets.begin(); it != _targets.end(); ++it) {
            if ((*it)->getType() == target->getType()) {
                return;
            }
        }
        _targets.push_back(target->clone());
    }
}

void TargetGenerator::forgetTargetType(std::string const &type)
{
    for (std::vector<ATarget *>::iterator it = _targets.begin(); it != _targets.end(); ++it) {
        if ((*it)->getType() == type) {
            delete *it;
            _targets.erase(it);
            return ;
        }
    }
}

ATarget* TargetGenerator::createTarget(std::string const &type)
{
    for (std::vector<ATarget *>::iterator it = _targets.begin(); it != _targets.end(); ++it) {
        if ((*it)->getType() == type) {
            return (*it);
        }
    }
    return (NULL);
}
