#ifndef PM_UNIT_H
#define PM_UNIT_H

#include <Rule.h>


class pm_unit : public Rule //if this, then expect pattern of type: p1=4...8
{
    public:
        pm_unit(std::string s);
        int getStart();
        int getEnd();
        int getMemp();
    protected:
        int mem_p; // pattern id used in memory
        int ind_s; // start of the matching range
        int ind_e; // end of the matching range
    private:
};

#endif // PM_UNIT_H
