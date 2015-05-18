#include "pm_unit.h"

pm_unit::pm_unit(std::string s) //if this, then expect pattern of type: p1=4...8
{
    this->class_id = 2;
    int i = 1;
    this->mem_p = parseNum(&i, s); // SAVE ID: p1, p2, p3 etc.
    if(s[i] == '=') {
        i++;
        this->ind_s = parseNum(&i, s);
        i = i + 3;
        this->ind_e = parseNum(&i, s);
    }
}

int pm_unit::getStart() {
    return this->ind_s;
}

int pm_unit::getEnd() {
    return this->ind_e;
}

int pm_unit::getMemp() {
    return this->mem_p;
}

pm_unit::~pm_unit()
{
    //dtor
}
