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
}

std::string const &Warlock::getName() const { return (_name);}
std::string const &Warlock::getTitle() const { return (_title);}

void Warlock::setTitle(std::string const &title) { _title = title;}

void Warlock::introduce() const { std::cout << _name << ": I am " << _name << ", " << _title << "!\n";}

void Warlock::learnSpell(ASpell *spell)
{
    _book.learnSpell(spell);
}

void Warlock::forgetSpell(std::string name)
{
    _book.forgetSpell(name);
}

void Warlock::launchSpell(std::string name, ATarget const &target)
{
    ASpell *temp = _book.createSpell(name);
    if (temp)
        temp->launch(target);
}
