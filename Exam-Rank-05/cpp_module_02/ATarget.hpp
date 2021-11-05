#pragma once

#include <iostream>

class ASpell;

class ATarget
{
    private:
        std::string _type;

    public:
        ATarget();
        ATarget(std::string const &type);
        ATarget(ATarget const &other);
        ATarget &operator=(ATarget const &other);
        virtual ~ATarget();

        std::string const &getType() const;

        void getHitBySpell(ASpell const &spell) const;

        virtual ATarget *clone() const = 0;
};

#include "ASpell.hpp"
