#include "Pattern.h"
#include "Rule.h"

Pattern::Pattern(std::string s) {
    this->class_id = 1;
    int i = 0;
    bool patternParsed = false;

    while (!patternParsed) {
        pattern[i] = s[i];
        i++;
        if ((s[i] == '[') || (s[i] == '\0')) {
            patternParsed = true;
        }
    }
    (pattern)[i] = '\0';
    if ((s[i] == '[')) {
        i++;
        mismatches = parseNum(&i, s);
        i++;
        insertions = parseNum(&i, s);
        i++;
        deletions = parseNum(&i, s);
    } else {
        mismatches = 0;
        insertions = 0;
        deletions = 0;
    }
}

int Pattern::getMismatch() {
    return mismatches;
}

int Pattern::getInsert() {
    return insertions;
}

int Pattern::getDelet() {
    return deletions;
}

void Pattern::getOverlap(int overlap[], int len) {
    overlap[0] = -1;
    char c;
    int v;
    for (int i = 1; i < len; i++) {
        char c = pattern[i];  // current character
        int v = overlap[i-1]; // ourrent overlap of earlier

        while ((pattern[v+1] != c) && (v != -1)) {
            v = overlap[v];
        }

        if (pattern[v+1] == c) {
            overlap[i] = v + 1;
        } else {
            overlap[i] = -1;
        }
    }
}

char* Pattern::getPattern() {
    return pattern;
}
