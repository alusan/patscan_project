#include <iostream>
#include <string>
#include <ctype.h>
#include <fstream>
#include "include/rule.h"
#include "include/pattern.h"
#include <vector>
#include <time.h>
// Hej
using namespace std;

struct rules{
    int start;
    int finish;
    int revers;
    char* pattern = (char *)malloc(10 * sizeof(char));
    int mismatches;
    int insertions;
    int deletions;
};

struct amb {
    int mis;
    int ins;
    int del;
};

rules ids[20]; // Conaint a copy of a saved pattern. p1 becomes ids[1].

rules match1, match2, match3; // global match rules. Will later become an array of rule classes

int getSum(amb s) {
    return s.mis + s.ins + s.del;
}


bool ReverseComplement(char c) { // Unusable code for later use. Alphabet: atcgtuamkrywsbvdhn
    switch(c){
    case 0:
        return true;
        break;
    default :
       return false;
    }
}

int parseNum(int *index, string s) { // Updates index and returns string number
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

rules parseMatchingRule(string s) { //if this, then expect pattern of type: 4...8
    rules match;
    int i = 0;
    match.start = parseNum(&i, s);
    i = i + 3;
    match.finish = parseNum(&i, s);
    match.revers = 0;
    return match;
}

rules parseRevMatching(string s) { //if this, then expect pattern of type: ~p1 or ~p2 etc.
    rules match;
    int i = 1;
    if (s[i] == 'p') {
        i++;
        int id = parseNum(&i, s);
        if ((ids[id].start != NULL) && (ids[id].finish != NULL)) {
            match = ids[id];
            match.revers = 1;
        } else {
            cerr << "Error; Couldn't find pat ID";
            exit(1);
        }
    }
    return match;
}

rules parseRuleAndID(string s) { //if this, then expect pattern of type: p1=4...8
    rules match;
    int i = 1;
    int id = parseNum(&i, s); // SAVE ID: p1, p2, p3 etc.
    if(s[i] == '=') {
        i++;
        match.start = parseNum(&i, s);
        i = i + 3;
        match.finish = parseNum(&i, s);
        match.revers = 0;
        ids[id] = match;
    } else {
        cerr << "Error; Parsing failed";
        exit(1);
    }
    return match;
}

rules parsePattern(string s) {
    rules match;

    int i = 0;
    bool patternParsed = false;

    while (!patternParsed) {
        (match.pattern)[i] = s[i];
        i++;
        if ((s[i] == '[') || (s[i] == '\0')) {
            patternParsed = true;
        }
    }
    (match.pattern)[i] = '\0';
    if ((s[i] == '[')) {
        i++;
        match.mismatches = parseNum(&i, s);
        i++;
        match.insertions = parseNum(&i, s);
        i++;
        match.deletions = parseNum(&i, s);
    } else {
        match.mismatches = 0;
        match.insertions = 0;
        match.deletions = 0;
    }
    return match;
}

/*
Parse string pattern. Return match class. Can be modified to be be smaller by
if statements for char numbers.
*/
Rule parsePatString(string s) {

    if ((s[0] > 47) && (s[0] <= 57)) {
        //return parseMatchingRule(s);
    } else {
        if (s[0] == '~') {
            //return parseRevMatching(s);
        } else {
            if (s[0] == 'p') {
                //return parseRuleAndID(s);
            } else {
                Pattern match(s);
                return match;
            }
        }
    }
    //Pattern match(s5);
}

/*
finds the best levenshtein distance between min-max of 2nd string compared to the first.
Has separate tracking of miamatches, insertions and deletions.
*/
amb modLevenshtein(string s1, string s2, int ins, int del, int mis, int *result_len) {
    amb final_result;

    unsigned int x, y, s1len, s2len;
    s1len = s1.length();
    s2len = s2.length();
    amb matrix[s2len+1][s1len+1];
    matrix[0][0].mis = 0;
    matrix[0][0].ins = 0;
    matrix[0][0].del = 0;
    for (x = 1; x <= s2len; x++) {
        matrix[x][0].mis = 0;
        matrix[x][0].ins = matrix[x-1][0].ins + 1;
        matrix[x][0].del = 0;
        //cout << matrix[x][0].mis << endl;
        //cout << matrix[x][0].del << endl;
        //cout << matrix[x][0].ins << endl;
    }


    for (y = 1; y <= s1len; y++) {
        matrix[0][y].mis = 0;
        matrix[0][y].ins = 0;
        matrix[0][y].del = matrix[0][y-1].del + 1;
        //cout << matrix[0][y].mis << endl;
        //cout << matrix[0][y].del << endl;
        //cout << matrix[0][y].ins << endl;
    }

    //cout << "Test: " << matrix[0][1].mis << " " << matrix[0][1].del << " " << matrix[0][1].ins << endl;

    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++) {
            int a = getSum(matrix[x-1][y]) + 1;
            int b = getSum(matrix[x][y-1]) + 1;
            int edit = (s1[y-1] == s2[x-1] ? 0 : 1);
            int c = getSum(matrix[x-1][y-1]) + edit;
            //cout << a << " ";
            //cout << b << " ";
            //cout << c << endl;
            if (a < b) {
                if (a < c) {
                    matrix[x][y].del = matrix[x-1][y].del;
                    matrix[x][y].ins = matrix[x-1][y].ins + 1;
                    matrix[x][y].mis = matrix[x-1][y].mis;
                } else {
                    matrix[x][y].mis = matrix[x-1][y-1].mis + edit;
                    matrix[x][y].del = matrix[x-1][y-1].del;
                    matrix[x][y].ins = matrix[x-1][y-1].ins;
                }
            } else {
                if (b < c) {
                    matrix[x][y].del = matrix[x][y-1].del + 1;
                    matrix[x][y].ins = matrix[x][y-1].ins;
                    matrix[x][y].mis = matrix[x][y-1].mis;
                } else {
                    matrix[x][y].mis = matrix[x-1][y-1].mis + edit;
                    matrix[x][y].del = matrix[x-1][y-1].del;
                    matrix[x][y].ins = matrix[x-1][y-1].ins;
                }
            }
            // check matches between (len - del) and (ins + len). Return correct one
            if ((x == (s2len - ins)) ||
                ((x > (s2len - ins)) && (matrix[x][y].mis < final_result.mis) && (matrix[x][y].del < final_result.del) && (matrix[x][y].ins < final_result.ins)) ) {
                final_result.del = matrix[x][y].del;
                final_result.ins = matrix[x][y].ins;
                final_result.mis = matrix[x][y].mis;
                *result_len = x;
            }

        }

    return final_result;
}

/*
Fuzzy stringsearch. Takes a pattern and a textline to find matches in. Traverses the text and tries to predict a match to
test with the modLevenshtein() algorithm. If correct match is found; index is jumped to the end of matched text.

Later iterations will have these parts in their respective classes and modified for multiple patterns.
The prediction algorithm is crude and gives errors with smaller patterns. This must be fixed.
*/
void runTextline(Pattern match, string t) {
    int len_t = t.length();
    string p = (match.getPattern());
    int len_p = p.length();

    int mis = match.getMismatch();
    int ins = match.getInsert();
    int del = match.getDelet();
    int lookAhead = mis + del;

    amb amb_results;

    int i = 0;
    while (i < (len_t - len_p + lookAhead)) { // traverse the text as far as possible
        int j = 0;
        while ((j < lookAhead) && (t[i] != p[j])) { // is there a match
            j++;
        }
        if (j < lookAhead) { // Did we find a match?
            if ((t[i+1] == p[j+1]) || (t[i+2] == p[j+2]) || (t[i+3] == p[j+3])) {
                int result_len = len_p + ins;
                amb_results = modLevenshtein(p, (t.substr(i-j, result_len)), ins, del, mis, &result_len);

                if ((amb_results.del <= del) && (amb_results.ins <= ins) && (amb_results.mis <= mis)) {
                    cout << "Found result at index " << i << " of " << t.substr(i-j, result_len) << " - " << "Mis: " << amb_results.mis << " - " << "Ins: " << amb_results.ins << " - " << "Del: " << amb_results.del << endl;
                    i += result_len - 1;
                }
            }
        } else {

        }
    i++;
    }
}


int main(int argc, char** argv) {

	string patFileName = "";
	string genomFileName = "";
	vector<string> genome;
	string pattern;
	string line;

	if ( argc == 3 ){
		patFileName = argv[1];
		genomFileName = argv[2];
	} else {
		cerr << "Usage: ./scan_for_matches file1 file2\n";
	}
	ifstream patFile;
	patFile.open(patFileName);

	if (patFile.is_open()){
		getline(patFile, line);
		pattern = line;
	}
	else {
		cerr << "Error; pattern file not open\n";
	}
	patFile.close();
	ifstream genomFile;
	genomFile.open(genomFileName);
	if (genomFile.is_open()){
        while (getline(genomFile, line)){
			genome.push_back(line);
		}

	}
	else {
        cerr << "Error; genome file not open\n";
	}
	genomFile.close();
	cout << pattern << endl;


    /*
    string s1 = "p1=4...8";
    string s2 = "~p1";
    string s3 = "245";
    string s4 = "AAANNBNAA[2,1,0]"; */
    string s5 = "ATTATACA[2,1,0]";
    Pattern match(pattern);
    /* match1 = parsePatString(s1);
    match2 = parsePatString(s2);
    match3 = parsePatString(s4);
    cout << "Hejsa derrrrrr" << endl;
    cout << match1.start << " : " << match1.finish << " Reverse Complement: " << match1.revers << endl;
    cout << match2.start << " : " << match2.finish << " Reverse Complement: " << match2.revers << endl;
    cout << match3.pattern << " - " << match3.mismatches << " " << match3.insertions << " " << match3.deletions << endl;

	cout << match.getMismatch() << endl;
	cout << (match.getPattern()).length() << " - " << match.getMismatch() << " " << match.getInsert() << " " << match.getDelet() << endl;



    amb result = modLevenshtein(t1, t2);
    cout << "Mis: " << result.mis << endl << "Ins: " << result.ins << endl << "Del: " << result.del << endl; */

    clock_t ct;
    ct = clock();
    cout << "Clock start: " << ct << endl;

    string t = "ATCGCACBTTATACATTATTATACAT";

    for (int i = 1; i < genome.size(); i++) {
        runTextline(match, genome[i]); // runs each textline and searches for matches.
    }

    ct = clock() - ct;
    cout << "Clock end: " << ct << endl;

    return 0;
}
