#ifndef PATTERN_H
#define PATTERN_H
#include <iostream>
#include "rule.h"
#include <string>

class Pattern : public Rule
{
    public:
        Pattern(std::string s);
        int getMismatch();
        int getInsert();
        int getDelet();
        void getOverlap(int overlap[], int len);
        char* getPattern();
    protected:
        char* pattern = (char *) malloc(10 * sizeof(char));
        int mismatches;
        int insertions;
        int deletions;
    private:
};

#endif // PATTERN_H
