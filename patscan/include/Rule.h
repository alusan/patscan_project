#ifndef RULE_H
#define RULE_H

#include <string>

class Rule
{
    public:
        Rule();
        bool getRevers();
        int parseNum(int *index, std::string s);
    protected:
        bool revers;
    private:
};

#endif // RULE_H
