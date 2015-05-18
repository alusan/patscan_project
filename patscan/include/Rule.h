#ifndef RULE_H
#define RULE_H

#include <string>

class Rule
{
    public:
        Rule();
        int parseNum(int *index, std::string s);
        int getID();
        virtual ~Rule();
    protected:
        int class_id; //ID of the class type
    private:
};

#endif // RULE_H
