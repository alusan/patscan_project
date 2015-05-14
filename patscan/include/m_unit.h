#ifndef M_UNIT_H
#define M_UNIT_H

#include <Rule.h>


class m_unit : public Rule //if this, then expect pattern of type: 4...8
{
    public:
        m_unit(std::string s);
        int getStart();
        int getEnd();
    protected:
        int ind_s; // start of the matching range
        int ind_e; // end of the matching range
    private:
};

#endif // M_UNIT_H
