#pragma once

#include <iostream>
#include "ASpell.hpp"
#include "ATarget.hpp"
#include <vector>

class Warlock
{
    private:
        std::string _name;
        std::string _title;

        Warlock();
        Warlock(Warlock const &other);
        Warlock &operator=(Warlock const &other);

        std::vector<ASpell *> _spells;
    public:
        Warlock(std::string const &name, std::string const &title);
        ~Warlock();

        std::string const &getName() const;
        std::string const &getTitle() const;

        void setTitle(std::string const &title);

        void introduce() const;

        void learnSpell(ASpell *spell);
        void forgetSpell(std::string name);
        void launchSpell(std::string name, ATarget const &target);
};
