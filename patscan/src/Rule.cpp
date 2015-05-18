#include "Rule.h"

Rule::Rule()
{
}

int Rule::parseNum(int *index, std::string s) { // Updates index and returns string number
    int formerIndex = *index;
    int length = 0;
    bool finished = true;
    while(finished) {
        if(isdigit(s[*index])) {
            length++;
            (*index)++;
        } else {
            finished = false;
        }
    }
    return stoi((s.substr(formerIndex, length)), nullptr, 10);
}

int Rule::getID() {
    return this->class_id;
}

Rule::~Rule()
{
    //dtor
}

