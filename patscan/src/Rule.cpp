#include "Rule.h"

Rule::Rule()
{
    revers = false;
}

bool Rule::getRevers() {
    return revers;
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

