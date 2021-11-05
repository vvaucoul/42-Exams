#include "Warlock.hpp"

Warlock::Warlock(std::string const &name, std::string const &title)
{
    _name = name;
    _title = title;
    std::cout << _name << ": This looks like another boring day.\n";
}

Warlock::~Warlock()
{
    std::cout << _name << ": My job here is done!\n";
    for (std::vector<ASpell *>::iterator it = _spells.begin(); it != _spells.end(); ++it) {
        delete (*it);
    }
    _spells.clear();
}

std::string const &Warlock::getName() const { return (_name);}
std::string const &Warlock::getTitle() const { return (_title);}

void Warlock::setTitle(std::string const &title) { _title = title;}

void Warlock::introduce() const { std::cout << _name << ": I am " << _name << ", " << _title << "!\n";}

void Warlock::learnSpell(ASpell *spell)
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

void Warlock::forgetSpell(std::string name)
{
    for (std::vector<ASpell *>::iterator it = _spells.begin(); it != _spells.end(); ++it) {
        if ((*it)->getName() == name) {
            delete *it;
            _spells.erase(it);
            return ;
        }
    }
}

void Warlock::launchSpell(std::string name, ATarget const &target)
{
    for (std::vector<ASpell *>::iterator it = _spells.begin(); it != _spells.end(); ++it) {
        if ((*it)->getName() == name) {
            (*it)->launch(target);
            return ;
        }
    }
}
