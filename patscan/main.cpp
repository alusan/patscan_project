#include <iostream>
#include <cstring>
#include <fstream>
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

struct sav_pat {
    int st;
    int len;
};

struct result_box {
    bool fullHit = false;
    int* r;
};


int results[20];
int max_units;

void subChars(char* s, char* newString, int start, int length) {
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
amb modLevenshtein(char* s1, char* s2, int i, int ins, int del, int mis, int *result_len) {
    amb final_result;

    unsigned int x, y, s1len, s2len;
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

    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++) {
            int a = getSum(matrix[x-1][y]) + 1;
            int b = getSum(matrix[x][y-1]) + 1;
            int edit = (s1[y-1] == s2[x+i-1] ? 0 : 1);
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

bool runUnit(int unidex, int i, char* seq, Rule **units, sav_pat *memp, int *r) {
    if(unidex >= max_units) {
        //cout << "End: 1: " << unidex << endl;
        //cout << "End: 2" << endl;
        for (int j = 0; j < max_units; j++) {
            results[j] = r[j];
        }
        return true;
    }

    switch (units[unidex]->getID()) {
        case 1: {
            //cout << i << endl;
            Pattern *cur_unit;
            cur_unit = dynamic_cast<Pattern *> (units[unidex]);
            char* pat = cur_unit->getPattern();
            bool box = false;

            int mis = cur_unit->getMismatch();
            int ins = cur_unit->getInsert();
            int del = cur_unit->getDelet();
            if((mis+ins+del) == 0) {
                int counter = 0;
                int length = strlen(pat);
                while (((seq[i+counter] == pat[counter]) || (seq[i+counter] == 'N'))
                       && (counter < length)) {
                counter++;
                }
                if (counter >= length) {
                    r[unidex] = counter;
                    box = runUnit(unidex+1, i+length, seq, units, memp, r);
                }
            } else {
                int result_len = strlen(pat) + ins;
                amb amb_results = modLevenshtein(pat, seq, i, ins, del, mis, &result_len);
                if ((amb_results.del <= del) && (amb_results.ins <= ins) && (amb_results.mis <= mis)) {
                    cout << "Amb: " << amb_results.mis << " " << amb_results.ins << " " << amb_results.del << " - ";
                    r[unidex] = result_len;
                    box = runUnit(unidex+1, i+result_len, seq, units, memp, r);
                }
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
                sav_pat newp[5];
                for(int i = 0; i <= 5; i++) {
                    newp[mid].st = memp[mid].st;
                    newp[mid].len = memp[mid].len;
                }
                //cout << "Step: 4" << endl;
                int ren[max_units];
                for(int i = 0; i < max_units; i++)
                    ren[i] = r[i];

                //cout << "Step: 5" << endl;
                ren[unidex] = rs;
                newp[mid].len = rs;
                //cout << "id: " << mid << " length: " << newp[mid].len << endl;
                //cout << "Step: 6" << endl;
                //cout << unidex+1 << " " << i+rs  << " " <<  seq  << endl << newp[mid].st << " " << newp[mid].len << " " <<  re[unidex] << endl;
                box = runUnit(unidex+1, i+rs, seq, units, newp, ren);
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
                box = runUnit(unidex+1, i+rs, seq, units, memp, re);
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
            int length = memp[mid].len;
            //cout << "Step: 1: " << start << " " << length << endl;
            char* pat = (char *) malloc(length * sizeof(char));
            //cout << "Step: 1.5: " << seq << endl;
            //cout << pat << endl;
            //cout << start << endl;
            //cout << length << endl;
            subChars(seq, pat, start, length);
            //cout << "Step: 2" << endl;
            if (cur_unit->getRevc()) {
                revChar(pat);
            }
            //cout << "Step: 3" << endl;

            int counter = 0;
            while (((seq[i+counter] == pat[counter]) || (seq[i+counter] == 'N')) && (counter < length)) {
                counter++;
                //cout << "Count: " << counter << endl;
            }
            //cout << "Step: 4: " << counter << endl;
            bool box = false;
            //cout << box.fullHit << endl;
            if (counter >= length) {
                //cout << "Rev: " << pat << endl;
                r[unidex] = counter;
                //cout << "Step: " << counter << " " << length << endl;
                box = runUnit(unidex+1, i+length, seq, units, memp, r);

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

/*
void test1(int *m[], int counter) {
    cout << m[1][1] << endl;
    m[1][1] += 1;
    if (counter < 1) {
        test1(m, counter+1);
    }
    cout << m[1][1] << endl;
}
void test2(int *m[], int len) {
    m[2][len-1] = 25;
    cout << m[2][len-1] << endl;
}
*/



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
		cerr << "Usage: ./scan_for_matches patternfile genomefil e\n";
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
    string s4 = "AAANNBNAA[2,1,0]";
    string s5 = "ATTATACA[2,1,0]";*/

    clock_t ct;
    ct = clock();
    cout << "Clock start: " << ct << endl;

    max_units = 1;
    int len_lines = 3;

    /*
    Rule *units[max_units];
    units[0] = parsePatString("p1=4...7");
    units[1] = parsePatString("3...8");
    units[2] = parsePatString("~p1");*/

    Rule *units1[max_units];
    units1[0] = parsePatString("AATCAA[2,1,0]");

    char* genlines[3];
    genlines[0] = "TCCTGTCTCATTCTACTAATCAATTTAAAATTTTCATTCATTTGGTTATA";
    genlines[1] = "TAGCTCAAATCTTAAGTACAGTTTTTAGTCAATTGACAGCCATATAATTC";
    genlines[2] = "TTAGTTTTGTATACTGATATTCATTAGAAATTATAAACTTTAAAAAATAT";

    int line_length;

    /*
    subChars(genlines[0], memp[1].s, 3, 6);
    cout << memp[1].s << endl;
    */


    for (int line_i = 0; line_i < len_lines; line_i++) { // Loops through each sequence line
        line_length = strlen(genlines[line_i]);
        //cout << "line: " << line_i << ":";
        for (int s_i = 0; s_i < line_length; s_i++) { // Traverses each character in the sequence
            //cout << " " << s_i;
            sav_pat memp[5];
            int ren[max_units];

            bool foundHit = runUnit(0, s_i, genlines[line_i], units1, memp, ren);

            //cout << "Is there a hit: " << box.fullHit << endl;

            if (foundHit) {
                cout << "line: " << line_i << ": " << s_i << endl;
                for (int j = 0; j < max_units; j++) {
                    //cout << results[j] << " ";
                    int r_len = s_i + results[j];
                    while (s_i < r_len) {
                        cout << (genlines[line_i])[s_i];
                        s_i++;
                    }
                    cout << " ";
                }
                cout << endl;
            }
        }
        //cout << endl;
    }

    /*
    string t = "ATCGCACBTTATACATTATTATACAT";

    for (int i = 1; i < genome.size(); i++) {
        runTextline(match, genome[i]); // runs each textline and searches for matches.
    } */

    ct = clock() - ct;
    cout << "Clock end: " << ct << endl;

    return 0;
}
