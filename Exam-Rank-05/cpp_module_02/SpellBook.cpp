#include "SpellBook.hpp"

SpellBook::SpellBook() {}

SpellBook::~SpellBook()
{
    std::vector<ASpell *>::iterator it_begin = _spells.begin();
    std::vector<ASpell *>::iterator it_end = _spells.end();
    while (it_begin != it_end)
    {
        delete *it_begin;
        ++it_begin;
    }
    _spells.clear();
}

void SpellBook::learnSpell(ASpell *spell)
{
    if (spell)
    {
        for (std::vector<ASpell *>::iterator it = _spells.begin(); it != _spells.end(); ++it) {
            if ((*it)->getName() == spell->getName()) {
                return;
            }
        }
        _spells.push_back(spell->clone());
    }
}

void SpellBook::forgetSpell(std::string const &name)
{
    for (std::vector<ASpell *>::iterator it = _spells.begin(); it != _spells.end(); ++it) {
        if ((*it)->getName() == name) {
            delete *it;
            _spells.erase(it);
            return ;
        }
    }
}

ASpell* SpellBook::createSpell(std::string const &name)
{
    for (std::vector<ASpell *>::iterator it = _spells.begin(); it != _spells.end(); ++it) {
        if ((*it)->getName() == name) {
            return (*it);
        }
    }
    return (NULL);
}
