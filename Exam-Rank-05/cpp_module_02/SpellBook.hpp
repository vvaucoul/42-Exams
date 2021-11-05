#pragma once

#include "ASpell.hpp"
#include <vector>

class SpellBook
{
private:
    std::vector<ASpell *> _spells;

    SpellBook(SpellBook const &other);
    SpellBook &operator=(SpellBook const &other);

public:
    SpellBook();
    ~SpellBook();

    void learnSpell(ASpell *spell);
    void forgetSpell(std::string const &name);
    ASpell* createSpell(std::string const &name);
};
