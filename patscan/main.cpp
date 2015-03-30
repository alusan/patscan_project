#include <iostream>
#include <string>
#include <ctype.h>
#include <fstream>
#include "include\rule.h"
#include "include\pattern.h"
#include <vector>

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

rules ids[20]; // Conaint a copy of a saved pattern. p1 becomes ids[1].

rules match1, match2, match3; // global match rules. Will later become an array of rule classes


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


rules parsePatString(string s) { // Parse string pattern. Return match struct/class

    switch(s[0]){
    case '0':
        return parseMatchingRule(s);
        break;
    case '1':
        return parseMatchingRule(s);
        break;
    case '2':
        return parseMatchingRule(s);
        break;
    case '3':
        return parseMatchingRule(s);
        break;
    case '4':
        return parseMatchingRule(s);
        break;
    case '5':
        return parseMatchingRule(s);
        break;
    case '6':
        return parseMatchingRule(s);
        break;
    case '7':
        return parseMatchingRule(s);
        break;
    case '8':
        return parseMatchingRule(s);
        break;
    case '9':
        return parseMatchingRule(s);
        break;
    case '~':
        return parseRevMatching(s);
        break;
    case 'p':
        return parseRuleAndID(s);
        break;
    default : // expect pattern of alphabet: atcgumkrywsbdhnATCGUMKRYWSBDHN
       return parsePattern(s);
    }
}

int main(int argc, char** argv) {


	string patFileName = "";
	string genomFileName = "";
	vector<string> genome;
	string pattern;
	string line;
	int g;

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
        g = 0;
        while (getline(genomFile, line)){
			genome.push_back(line);
			cout << genome[g] << endl;
			g++;
		}

	}
	else {
        cerr << "Error; genome file not open\n";
	}
	genomFile.close();
	cout << pattern << endl;


    string s1 = "p1=4...8";
    string s2 = "~p1";
    string s3 = "245";
    string s4 = "AAANNBNAA[2,1,0]";
    string s5 = "ATTATACA[2,1,0]";
    match1 = parsePatString(s1);
    match2 = parsePatString(s2);
    match3 = parsePatString(s4);
    cout << "Hejsa derrrrrr" << endl;
    cout << match1.start << " : " << match1.finish << " Reverse Complement: " << match1.revers << endl;
    cout << match2.start << " : " << match2.finish << " Reverse Complement: " << match2.revers << endl;
    cout << match3.pattern << " - " << match3.mismatches << " " << match3.insertions << " " << match3.deletions << endl;

    Pattern match(s5);
	cout << match.getMismatch() << endl;
	cout << (match.getPattern()).length() << " - " << match.getMismatch() << " " << match.getInsert() << " " << match.getDelet() << endl;

    string t = "ATCGCACATTATACATTATTATACAT";
    int len_t = t.length();
    string p = (match.getPattern());
    int len_p = p.length();

    int overlap[len_p];
    match.getOverlap(overlap, len_p);

    for (int j = 0; j < len_p; j++) {
        cout << overlap[j] << endl;
    }


    int mis = match.getMismatch();
    int ins = match.getInsert();
    int del = match.getDelet();



    int i = 0;
    int j = 0;
    int k = 0;
    while ((len_t-k) > len_p) {
        //cout << "step 1: " << i << " " << j << " " << k << endl;
        //cout << "test 1" << endl;
        while ((j<len_p) && (t[i] == p[j])) {
            i++;
            j++;
            cout << "test 2" << endl;
        }
        cout << "step 2: " << i << " " << j << " " << k << endl;
        if (j >= len_p) {
            cout << "Match found at: " << k << endl;
        }
        //cout << "step 3: " << i << " " << j << " " << k << endl;
        if ((j>0) && (overlap[j-1] > (-1))) {
            //cout << "overlap k1: " << k << " " << overlap[j-1] << endl;
            k = i - overlap[j-1] - 1;
            //cout << "overlap k2: " << k << endl;
        } else {
            if (i == k) {
                i++;
            }
            k = i;
            //cout << "increment: " << k << endl;
        }
        //cout << "step 4: " << i << " " << j << " " << k << endl;
        if (j > 0) {
            //cout << "j > 0" << endl;
            j = overlap[j-1] + 1;
            //cout << "j: " << j << endl;
        }
        //cout << "overlap k: " << k << endl;
        //cout << "step 5: " << i << " " << j << " " << k << endl;

    }


    return 0;
}
