#include "m_unit.h"

m_unit::m_unit(std::string s) //if this, then expect pattern of type: 4...8
{
    this->class_id = 3;
    int i = 0;
    ind_s = parseNum(&i, s);
    i = i + 3;
    ind_e = parseNum(&i, s);
}

int m_unit::getStart() {
    return this->ind_s;
}

int m_unit::getEnd() {
    return this->ind_e;
}
