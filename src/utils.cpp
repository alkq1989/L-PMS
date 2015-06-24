/*
 * utils.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: marius
 */

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <set>
using namespace std;
#include "utils.h"

string getAlphabet(vector<string>& strings) {
	char f[256];
	memset(f, 0, 256 * sizeof(char));
	for (unsigned i = 0; i < strings.size(); ++i) {
		string& s = strings[i];
		for (unsigned j = 0; j < s.length(); ++j)
			f[s[j]] = 1;
	}

	string sig;
	for (int i = 0; i < 256; ++i)
		if (f[i])
			sig.push_back((char) i);
	return sig;
}

string getAlphabetC(char** s, int n, int m) {
	char f[256];
	memset(f, 0, 256 * sizeof(char));
	for (int i = 0; i < n; ++i) {
		char *c = s[i];
		for (int j = 0; j < m; ++j)
			f[c[j]] = 1;
	}

	string sig;
	for (int i = 0; i < 256; ++i)
		if (f[i])
			sig.push_back((char) (i));
	return sig;
}

char** readInputC(char *fileName, int& n, int& L, int& d) {
	FILE *f = fopen(fileName, "r");
	if (f == NULL) {
		cout << "Could not open " << fileName << endl;
		exit(0);
	}
	char **c = new char*[n];
	char s[1024];
	for (int i = 0; i < n; ++i) {
		if (fscanf(f, "%s", s) > 0)
			c[i] = strdup(s);
		else {
			cout << "Not enough strings in the file" << endl;
			exit(0);
		}
	}
	fclose(f);
	return c;
}

int *allocate(int n) {
	return new int[n];
}

int **allocate(int n, int m) {
	int **a = new int*[n];
	for (int i = 0; i < n; ++i)
		a[i] = new int[m];
	return a;
}

int64 **allocate64(int n, int m) {
	int64 **a = new int64*[n];
	for (int i = 0; i < n; ++i)
		a[i] = new int64[m];
	return a;
}

int ***allocate(int n, int m, int k) {
	int ***a = new int**[n];
	for (int i = 0; i < n; ++i)
		a[i] = allocate(m, k);
	return a;
}

char **allocateChar(int n, int m) {
	char **a = new char*[n];
	for (int i = 0; i < n; ++i)
		a[i] = new char[m];
	return a;
}

char ***allocateChar(int n, int m, int k) {
	char ***a = new char**[n];
	for (int i = 0; i < n; ++i)
		a[i] = allocateChar(m, k);
	return a;
}

int charCmp(char *a, char *b, int n) {
	for (int i = 0; i < n; ++i)
		if (a[i] != b[i])
			return a[i] - b[i];
	return 0;
}

int HamDist(const char *a, const char *b, int n) {
	int d = 0;
	for (int i = 0; i < n; ++i)
		d += (a[i] != b[i]);
	return d;
}

bool isHamDistWithin(string a, string b, int L, int d) {

    int t=0;
    for(int i=0;i<3;i++){       
        if(a[i]==b[i]){
            continue;
        }else{
            return false;
        }
    }
    
    for(int i=L-5;i<L;i++){
        if(a[i]==b[i]){
            continue;
        }else{
            return false;
        }
    }
    
    for(int i=3;i<L-5;i++){   
        t+=(a[i] != b[i]);
    }
    
    if(t>d){
        return false; 
    }else{
        return true;
    }
}

char *getCopy(char *a, int n) {
	char *b = new char[n];
	memcpy(b, a, n * sizeof(char));
	return b;
}

void *buildMultiDArray(int dimensions, int n) {
	if (dimensions == 1)
		return new int[n];
	else {
		void **a = new void*[n];
		for (int i = 0; i < n; ++i)
			a[i] = buildMultiDArray(dimensions - 1, n);
		return a;
	}
}

void fillMultiDArray(void *a, int dimensions, int n, int val) {
	if (dimensions == 1) {
		int *aa = (int *) a;
		for (int i = 0; i < n; ++i)
			aa[i] = val;
	} else {
		void **aa = (void **) a;
		for (int i = 0; i < n; ++i)
			fillMultiDArray(aa[i], dimensions - 1, n, val);
	}
}

bool hasNegative(int *a, int n) {
	for (int i = 0; i < n; ++i)
		if (a[i] < 0)
			return true;
	return false;
}

void insertLeft(int *a, int pos) {
	int r = a[pos];
	int j;
	for (j = pos; j && a[j - 1] > r; --j)
		a[j] = a[j - 1];
	a[j] = r;
}

void insertSort(int *a, int n) {
	for (int i = 1; i < n; ++i)
		insertLeft(a, i);
}

void lexySmallestNeighbor(char *s, int L, int d) {
	for (int j = 0; j < L && d; ++j)
		if (s[j]) {
			s[j] = 0;
			--d;
		}
}

void printLmer(const char *x, int L) {
	char a[L + 1];
	a[L] = 0;
	for (int i = 0; i < L; ++i)
		a[i] = 'A' + x[i];
	cout << a << endl;
}

int maxFreq(int a, int b, int c) {
	if (a == b)
		return b == c ? 3 : 2;
	else if (a == c || b == c)
		return 2;
	else
		return 1;
}

int sumMaxFreq(char *a, char *b, char *c, int L) {
	int total = 0;
	for (int j = 0; j < L; ++j)
		total += maxFreq(a[j], b[j], c[j]);
	return total;
}

int sumMaxFreq(char *a, char *b, char *c, char *d, int L) {
	int total = 0;
	for (int j = 0; j < L; ++j) {
		int mf = 0;
		if (a[j] == b[j]) {
			if (b[j] == c[j]) {
				if (c[j] == d[j])
					mf = 4;
				else
					mf = 3;
			} else {
				if (a[j] == d[j])
					mf = 3;
				else
					mf = 2;
			}
		} else {
			if (a[j] == c[j]) {
				if (c[j] == d[j])
					mf = 3;
				else
					mf = 2;
			} else {
				if (b[j] == c[j]) {
					if (c[j] == d[j])
						mf = 3;
					else
						mf = 2;
				} else {
					if (a[j] == d[j] || b[j] == d[j] || c[j] == d[j])
						mf = 2;
					else
						mf = 1;
				}
			}
		}
		total += mf;
	}
	return total;
}

void encodeString(char *s, int m, string &sigma) {
	for (int j = 0; j < m; ++j)
		s[j] = sigma.find(s[j]);
}

void encodeStrings(char **s, int n, int m, string &sigma) {
	for (int i = 0; i < n; ++i)
		encodeString(s[i], m, sigma);
}

void encodeString(string& s, string &sigma) {
	for (unsigned j = 0; j < s.length(); ++j)
		s[j] = sigma.find(s[j]);
}

void encodeStrings(vector<string>& strings, string &sigma) {
	for (unsigned i = 0; i < strings.size(); ++i)
		encodeString(strings[i], sigma);
}

string decodeString(const char *s, int n, const string& sigma) {
	string t(n, 'x');
	for (int i = 0; i < n; ++i)
		t[i] = sigma[s[i]];
	return t;
}

void trim(string& s) {
	string delim = " \n\r\t";
	s.erase(s.find_last_not_of(delim) + 1);
	s.erase(0, s.find_first_not_of(delim));
}

bool readFasta(char *fileName, vector<string>& strings) {
	ifstream myfile(fileName);
	string data;
	if (myfile.is_open()) {
		string line;
		while (myfile.good()) {
			getline(myfile, line);
			trim(line);
			if (line[0] == '>') {
				if (data.size() > 0) {
					strings.push_back(data);
					data.clear();
				}
			} else {
				data.append(line);
			}
		}
		myfile.close();
		if (data.size() > 0)
			strings.push_back(data);
		return true;
	} else {
		return false;
	}
}

set<string> subString;
        bool readFastaOpposite(char *fileName,vector<string>& strings, int L){
                ifstream myfile(fileName);
                ofstream opp("/home/lin/Documents/subOpp.txt");
                string data;
                if(myfile.is_open()){
                        string line;
                        while(myfile.good()){
                                getline(myfile,line);
                                trim(line);
                                if(line[0]=='>'){
                                        if(data.size()>0){
                                                strings.push_back(data);
                                                data.clear();
                                        }
                                } else {
                                        data.append(line);
                                }
                        }
                        myfile.close();
                        if(data.size()>0)
                                strings.push_back(data);
                        for(vector<string>::iterator it=strings.begin(); it!=strings.end(); it++){
                                for(unsigned j=0;j<(*it).length()-L+1;j++){
                                        subString.insert((*it).substr(j,L));
                                }
                        }
                        for(set<string>::iterator it=subString.begin();it!=subString.end();it++){
                            opp<<*it<<endl;
                        }
                        return true;
                } else {
                        return false;
                }
        }       

set<string> getSubString(){
     return subString;
}
        
int k;
bool filterMotif(string ss, set<string> oppStrings, int L,int d){
     string sss;
     set<string>::iterator it=oppStrings.begin();
     while(it!=oppStrings.end()){
         sss=*it;
         if(isHamDistWithin(ss,sss,L,d)){
//             cout<<"相近的两个序列："<<ss<<" (ss)"<<endl;
//             cout<<"相近的两个序列："<<sss<<" (lmer)"<<  k++<<endl;
//             cout<<" "<<endl;
             return true;
         }else{
             it++;
         }
     }
     return false;
}

int sumLengths(vector<string>& v) {
	unsigned int n = v.size();
	int total = 0;
	for (unsigned i = 0; i < n; ++i)
		total += v[i].length();
	return total;
}

char **pack(vector<string>& strings) {
	int n = strings.size();
	char **s = new char*[n];
	char *c = new char[sumLengths(strings)];
	for (int i = 0; i < n; ++i) {
		int m = strings[i].length();
		memcpy(c, strings[i].c_str(), m * sizeof(char));
		s[i] = c;
		c += m;
	}
	return s;
}


int bitsFor(int n) {
	int p = 1;
	for (int b = 16, m = n; b; b >>= 1)
		if (m >= (1 << b)) {
			p += b;
			m >>= b;
		}
	return p;
}

int nextPow2(int n) {
	int p = bitsFor(n);
	if ((1 << (p - 1)) == n)
		--p;
	return 1 << p;
}

bool isBitSet(int64 *a, int index, int*bitMask) {
	return a[index >> 6] & bitMask[index & 63];
}

void setBit(int64 *a, int index, int*bitMask) {
	a[index >> 6] |= bitMask[index & 63];
}

bool isBitSet(int *a, int index) {
//	return a[index >> 5] & bitMask[index & 31];
	return a[index >> 5] & (1 << (index & 31));
}

void setBit(int *a, int index) {
//	a[index >> 5] |= bitMask[index & 31];
	a[index >> 5] |= (1 << (index & 31));
}

bool isBitSet(char *a, int index, int*bitMask) {
	return a[index >> 3] & bitMask[index & 7];
}

void setBit(char *a, int index, int*bitMask) {
	a[index >> 3] |= bitMask[index & 7];
}
