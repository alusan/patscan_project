#ifndef P_UNIT_H
#define P_UNIT_H

#include <Rule.h>


class p_unit : public Rule //if this, then expect pattern of type: ~p1 or ~p2 etc.
{
    public:
        p_unit(std::string s);
        int getMemp();
        bool getRevc();
    protected:
        int mem_p;
        bool revc;
    private:
};

#endif // P_UNIT_H
