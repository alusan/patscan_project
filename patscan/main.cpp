#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include "include/rule.h"
#include "include/pattern.h"
#include "include/pm_unit.h"
#include "include/p_unit.h"
#include "include/m_unit.h"
#include <vector>
#include <time.h>

using namespace std;

struct amb {
    int mis;
    int ins;
    int del;
};

struct dp {
    int sqline;
    int sp;
};

struct sav_pat {
    int st;
    int len;
};

struct result_box {
    bool fullHit = false;
    int* r;
};


//int m[10][50][20][50]; For DP programming
int currentLine; // For DP programming
int *results;
int max_units;
int current_line_length;
char lineComp[51];

void subChars(string s, char* newString, int start, int length) {
    for (int i = 0; i < length; i++) {
        newString[i] = s[start];
        start++;
    }
    newString[length] = '\0';
}

int getSum(amb s) {
    return s.mis + s.ins + s.del;
}


char comp(char c) { //complement function from scan_for_matches
    switch (c)
    {
      case 'a':
	return 't';
      case 'A':
	return 'T';

      case 'c':
	return 'g';
      case 'C':
	return 'G';

      case 'g':
	return 'c';
      case 'G':
	return 'C';

      case 't':
      case 'u':
	return 'a';
      case 'T':
      case 'U':
	return 'A';

      case 'm':
	return 'k';
      case 'M':
	return 'K';

      case 'r':
	return 'y';
      case 'R':
	return 'Y';

      case 'w':
	return 'w';
      case 'W':
	return 'W';

      case 's':
	return 'S';
      case 'S':
	return 'S';

      case 'y':
	return 'r';
      case 'Y':
	return 'R';

      case 'k':
	return 'm';
      case 'K':
	return 'M';

      case 'b':
	return 'v';
      case 'B':
	return 'V';

      case 'd':
	return 'h';
      case 'D':
	return 'H';

      case 'h':
	return 'd';
      case 'H':
	return 'D';

      case 'v':
	return 'b';
      case 'V':
	return 'B';

      case 'n':
	return 'n';
      case 'N':
	return 'N';

      default:
	return c;
    }
}

void revChar(char* str) { //modified reverse function from stackoverflow.
    const size_t len = strlen(str);
    const size_t half = len / 2;

    for(size_t i = 0; i < half; i++) {
        char temp = comp(str[i]);
        str[i] = comp(str[len-i-1]);
        str[len-i-1] = temp;
    }
    if (len % 2) { // reverse complment the middle char if length is odd
        str[half] = comp(str[half]);
    }
}

int getminimunLimit(Rule **units) {
    int minimumLimit = -1;
    int m[5];
    for (int i = 0; i < max_units; i++) {
        switch (units[i]->getID()) {
            case 1: {
                Pattern *cur_unit;
                cur_unit = dynamic_cast<Pattern *> (units[i]);
                char* pat = cur_unit->getPattern();
                minimumLimit += strlen(pat);
                break;
            }
            case 2: {
                pm_unit *cur_unit;
                cur_unit = dynamic_cast<pm_unit *> (units[i]);
                m[cur_unit->getMemp()] = cur_unit->getStart();
                minimumLimit += cur_unit->getStart();
                break;
            }
            case 3: {
                m_unit *cur_unit;
                cur_unit = dynamic_cast<m_unit *> (units[i]);
                minimumLimit += cur_unit->getStart();
                break;
            }
            case 4: {
                p_unit *cur_unit;
                cur_unit = dynamic_cast<p_unit *> (units[i]);
                minimumLimit += m[cur_unit->getMemp()];
                break;
            }
        }
    }
    return minimumLimit;
}

bool patCheck(string s) {
    if (s[0] == '~') {
        return true;
    }
    int len = s.length();
    if (s[0] == 'p') {
        if (len > 1) {
            int index = 1;
            if ((index < len) && isdigit(s[index])) {
                index++;
            } else {
                return false;
            }
            bool cont = true;
            while (cont) {
                if ((index < len)) {
                    if (isdigit(s[0])) {
                        index++;
                    } else {
                        return false;
                    }
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

/*
Parse string pattern. Return match class. Can be modified to be be smaller by
if statements for char numbers.
*/
Rule* parsePatString(string s) {
    if ((s[0] > 47) && (s[0] <= 57)) {
        m_unit* pat = new m_unit(s);
        return pat;
    } else {
        if (patCheck(s)) {
            p_unit* pat = new p_unit(s);
            return pat;
        } else {
            if (s[0] == 'p') {
                pm_unit* pat = new pm_unit(s);
                return pat;
            } else {
                Pattern* pat = new Pattern(s);
                return pat;
            }
        }
    }
}

/*
finds the best levenshtein distance between min-max of 2nd string compared to the first.
Has separate tracking of miamatches, insertions and deletions.
*/
amb modLevenshtein(char* s1, string s2, int i, int ins, int del, int mis, int *result_len) {
    /*amb final_result;
    final_result.mis = 9001;
    final_result.ins = 9001;
    final_result.del = 9001;*/
    int x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = *result_len;
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
    int rangeS = s1len - del; // range from the smallest possible word
    for (x = 1; x <= s2len; x++) {
        for (y = 1; y <= s1len; y++) {
            int a = getSum(matrix[x-1][y]) + 1;
            int b = getSum(matrix[x][y-1]) + 1;

            int edit;
            int s2i = x+i-1;
            if ((toupper(s1[y-1]) == (s2[s2i])) || (toupper(s2[s2i]) == 'N')) {
                edit = 0;
            } else {
                edit = 1;
            }
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
        }
        // check matches between (len - del) and (ins + len). Return correct one
        if ((x >= rangeS) && ((matrix[x][s1len].mis < mis) && (matrix[x][s1len].del < del) && (matrix[x][s1len].ins < ins))) {
            *result_len = x;
            return matrix[x][s1len];
        }
    }
    return matrix[s2len][s1len];
}

bool runUnit(int unidex, int i, string seq, Rule **units, sav_pat *memp, int *r, dp *m[]) {
    if(unidex >= max_units) {
        //cout << "End: 1: " << unidex << endl;
        results = r;
        return true;
    }

    switch (units[unidex]->getID()) {
        case 1: {
            if (m[unidex][i].sqline == currentLine) {
                return false;
            }
            //cout << i << endl;
            Pattern *cur_unit;
            cur_unit = dynamic_cast<Pattern *> (units[unidex]);
            char* pat = cur_unit->getPattern();
            bool box = false;

            int mis = cur_unit->getMismatch();
            int ins = cur_unit->getInsert();
            int del = cur_unit->getDelet();
            if ((ins == 0) && (del == 0)) { // faster search if ins=del=0
                int counter = 0;
                int length = strlen(pat);
                while (counter < length) {
                    if ((toupper(seq[i+counter]) == toupper(pat[counter])) || (toupper(seq[i+counter]) == 'N')) {
                        counter++;
                    } else {
                        if (mis > 0) {
                            mis--;
                            counter++;
                        } else {
                            break;
                        }
                    }
                }
                if (counter >= length) {
                    r[unidex] = counter;
                    box = runUnit(unidex+1, i+length, seq, units, memp, r, m);
                }
            } else {
                /*bool runLeven = false;
                if ((toupper(seq[i]) == toupper(pat[0]))) {

                }*/


                int result_len = strlen(pat) + ins;
                amb amb_results = modLevenshtein(pat, seq, i, ins, del, mis, &result_len);
                if ((amb_results.del <= del) && (amb_results.ins <= ins) && (amb_results.mis <= mis)) {
                    cout << "Amb: " << amb_results.mis << " " << amb_results.ins << " " << amb_results.del << " - ";
                    r[unidex] = result_len;
                    box = runUnit(unidex+1, i+result_len, seq, units, memp, r, m);
                }
            }

            if (box == false) {
                m[unidex][i].sqline = currentLine;
            }
            return box;
            break;
        }
        case 2: {
            //cout << "its a pm unit" << endl;
            pm_unit *cur_unit;
            cur_unit = dynamic_cast<pm_unit *> (units[unidex]);
            int rs = cur_unit->getStart();
            int re = cur_unit->getEnd();
            int mid = cur_unit->getMemp();
            //cout << "id: " << mid << endl;
            memp[mid].st = i;
            //cout << "Step: 1" << endl;
            bool box;
            //cout << "Step: 2" << endl;
            while (rs <= re) {
                //cout << "Step: 3" << endl;
                //cout << "Step: 4" << endl;

                //cout << "Step: 5" << endl;
                r[unidex] = rs;
                memp[mid].len = rs;
                //cout << "id: " << mid << " length: " << newp[mid].len << endl;
                //cout << "Step: 6" << endl;
                //cout << unidex+1 << " " << i+rs  << " " <<  seq  << endl << newp[mid].st << " " << newp[mid].len << " " <<  re[unidex] << endl;
                box = runUnit(unidex+1, i+rs, seq, units, memp, r, m);
                //cout << "bool is: " << box.fullHit << endl;
                //cout << "PM: 1" << endl;
                //cout << "Step: 7" << endl;
                if (box == true) {
                    //cout << "PM: Success" << endl;
                    return box;
                }
                rs++;
            }
            return false;
            break;
        }
        case 3: {
            //cout << "its a m unit" << endl;
            m_unit *cur_unit;
            cur_unit = dynamic_cast<m_unit *> (units[unidex]);
            int rs = cur_unit->getStart();
            int re = cur_unit->getEnd();
            bool box = false;

            while (rs <= re) {
                int re[max_units];
                for(int i = 0; i < max_units; i++)
                    re[i] = r[i];

                re[unidex] = rs;
                //cout << unidex+1 << " " << i+rs  << " " <<  seq  << endl << re[unidex] << endl;
                box = runUnit(unidex+1, i+rs, seq, units, memp, re, m);
                //cout << "M: 1" << endl;
                if (box == true) {
                    //cout << "M: Success" << endl;
                    return box;
                }
                rs++;
            }

            //cout << "bool is: " << box.fullHit << endl;
            return false;

            break;
        }
        case 4: {
            //cout << "its a p unit" << endl;
            p_unit *cur_unit;
            cur_unit = dynamic_cast<p_unit *> (units[unidex]);
            int mid = cur_unit->getMemp();
            int start = memp[mid].st;

            if ((m[unidex][i].sqline == currentLine) && (m[unidex][i].sp == start)) {
                return false;
            }
            int length = memp[mid].len;

            //cout << "Step: 1: " << start << " " << length << endl;
            //cout << "Step: 1.5: " << seq << endl;
            //cout << pat << endl;
            //cout << start << endl;
            //cout << length << endl;
            //cout << "Step: 2" << endl;
            //cout << "Step: 3" << endl;
            int counter = 0;
            if (cur_unit->getRevc()) {
                int offset = current_line_length - length - start;
                while (((toupper(seq[i+counter]) == toupper(lineComp[counter+offset])) ||
                        (toupper(seq[i+counter]) == 'N')) && (counter < length)) {
                    counter++;
                }
            } else {
                while (((toupper(seq[i+counter]) == toupper(seq[counter+start])) ||
                        (toupper(seq[i+counter]) == 'N')) && (counter < length)) {
                    counter++;
                }
            }

            //cout << "Step: 4: " << counter << endl;
            bool box = false;
            //cout << box.fullHit << endl;
            if (counter >= length) {
                //cout << "Rev: " << pat << endl;
                r[unidex] = counter;
                //cout << "Step: " << counter << " " << length << endl;
                box = runUnit(unidex+1, i+length, seq, units, memp, r, m);
            } else {
                m[unidex][i].sqline = currentLine;
                m[unidex][i].sp = start;
            }
            //cout << "Step: 5" << endl;
            //cout << "bool is: " << box.fullHit << endl;

            return box;
            //cout << "Step: 6" << endl;
            break;
        }
    }
    return false;
}

void rundpProcess(int start, int len_lines, vector<string> &genome, int seq_limit, Rule **units) {
    int line_length;
    sav_pat memp[5];
    dp **m = new dp *[3];
    for(int i = 0; i < 3; i++)
        m[i] = new dp[60];

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 60; j++) {
            m[i][j].sqline = -1;
            m[i][j].sp = -1;
        }
    }
    int *ren = new int[max_units];
    for (int line_i = 0; line_i < len_lines; line_i++) { // Loops through each sequence line
        //if (!((genome[line_i][0] == 'N') && (genome[line_i][1] == 'N'))) { // To avoid computing NNNNN sequences
        string seq = genome[line_i];
        current_line_length = seq.size();
        line_length = current_line_length - seq_limit;
        currentLine = line_i;

        seq.copy(lineComp, current_line_length);
        lineComp[current_line_length] = '\0';
        revChar(lineComp);
        //cout << "line: " << line_i << ":";
        int inx = 0;
        while (toupper(seq[inx]) == 'N')
            inx++;

        for (int s_i = inx; s_i < line_length; s_i++) { // Traverses each character in the sequence
            //cout << "Checksum: " << (line_i * 100) + s_i << endl;
            //cout << " " << s_i;
            bool foundHit = runUnit(0, s_i, seq, units, memp, ren, m);
            //cout << "Is there a hit: " << box.fullHit << endl;
            //cout << ren[1] << endl;

            if (foundHit) {
                cout << "line: " << line_i << ": " << s_i+1 << endl;
                for (int j = 0; j < max_units; j++) {
                    //cout << results[j] << " ";
                    int r_len = s_i + results[j];
                    while (s_i < r_len) {
                        cout << seq[s_i];
                        s_i++;
                    }
                    cout << " ";
                }
                cout << endl;
            }
        }
        //cout << genome[line_i] << endl;
        //cout << lineComp << endl;
    }
}


int main(int argc, char** argv) {
    clock_t ct;
    ct = clock();
    cout << "Clock start: " << ct << endl;

	string patFileName = "";
	string genomFileName = "";
	vector<string> genome;
    string pattern;
	string line;

	if ( argc == 3 ){
		patFileName = argv[1];
		genomFileName = argv[2];
	} else {
		cerr << "Usage: ./patscan patternfile genomefile\n";
		return -1;
	}
	ifstream patFile;
	patFile.open(patFileName);
	if (patFile.is_open()){
		getline(patFile, line);
		pattern = line;
	}
	else {
		cerr << "Error; pattern file not open\n";
		return -1;
	}
	int len_lines = 0;
	patFile.close();
	ifstream genomFile;
	genomFile.open(genomFileName);
	if (genomFile.is_open()){
        getline(genomFile, line);
        if ((line[0] == '>') && (line.size() > 1)) {
            cout << "Running sequence file: " << line << endl;
        } else {
            cerr << "Error; genome file is not a compatible .fa format\n";
            return -1;
        }

        while (getline(genomFile, line)){
			genome.push_back(line);
			len_lines++;
		}

	}
	else {
        cerr << "Error; genome file not open\n";
	}
	genomFile.close();

    max_units = 0;
    string buffer;
    stringstream ss(pattern);
    vector<string> pats; // Create vector to hold each pattern unit

    while (ss >> buffer) {
        pats.push_back(buffer);
        max_units++;
    }

    cout << "Running pattern: ";
    Rule *units[max_units];
    for (int i = 0; i < max_units; i++) {
        cout << pats[i] << " ";
        units[i] = parsePatString(pats[i]);
    }
    cout << endl;
    //cout << "Max Units: " << max_units << endl;
    //cout << "Lenght of Lines: " << len_lines << endl;

    int seq_limit = getminimunLimit(units);

    Pattern *cur_unit;
    bool exactMatching = false;
    if ((max_units==1) && (units[0]->getID() == 1)) {
            cur_unit = dynamic_cast<Pattern *> (units[0]);
            if ((cur_unit->getInsert()+cur_unit->getMismatch()+cur_unit->getDelet()) == 0)
                exactMatching = true;
    }

    if (exactMatching) {
        char* p = cur_unit->getPattern();
        int len_p = strlen(p);
        int len_t;
        int overlap[len_p];
        cur_unit->getOverlap(overlap, len_p);
        int i; // sequence index
        int j; // pattern index
        int k; // startindex for matching
        for (int line_i = 0; line_i < len_lines; line_i++) { // Loops through each sequence line
            string t = genome[line_i];
            int inx = 0;
            while (toupper(t[inx]) == 'N')
                inx++;

        //if (!((genome[line_i][0] == 'N') && (genome[line_i][1] == 'N') && (genome[line_i][2] == 'N'))) { // To avoid computing NNNNN sequences

            len_t = t.size();
            i = inx; // sequence index
            j = inx; // pattern index
            k = inx; // startindex for matching
            while ((len_t-k) > len_p) { //Running Knuth-Morris-Pratt for exact matches
                while ((j<len_p) && ((toupper(t[i]) == toupper(p[j])) || ((toupper(t[i]) == 'N')))) {
                    i++;
                    j++;
                }
                if (j >= len_p) {
                    cout << "line: " << line_i << ": " << k << endl;
                    int l_end = len_p + k;
                    for (int c = k; c < l_end; c++) {
                        cout << t[c];
                    }
                    cout << endl;
                    j = 0; // Remove this line to have overlapping hits
                }
                if ((j>0) && (overlap[j-1] > (-1))) {
                    k = i - overlap[j-1] - 1;
                } else {
                    if (i == k) {
                        i++;
                    }
                    k = i;
                }
                if (j > 0) {
                    j = overlap[j-1] + 1;
                }
            }
        //}
        }

    } else {
        // set up as function for multithreading when implemented by dividing the sequences to work on
        rundpProcess(0, len_lines, genome, seq_limit, units);
    } // end of ELSE
    ct = clock() - ct;
    cout << "Clock end: " << ct << endl;

    return 0;
}
