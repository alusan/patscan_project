#include "p_unit.h"

p_unit::p_unit(std::string s) //if this, then expect pattern of type: ~p1 or ~p2 etc.
{
    this->class_id = 4;
    int i = 0;
    if (s[i] == '~') {
        revc = true;
        i++;
    } else {
        revc = false;
    }
    if (s[i] == 'p') {
        i++;
        mem_p = parseNum(&i, s);
    }
}

int p_unit::getMemp() {
    return this->mem_p;
}

bool p_unit::getRevc() {
    return this->revc;
}
