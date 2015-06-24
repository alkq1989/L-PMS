/*
 * closestString.h
 *
 *  Created on: Jun 21, 2012
 *      Author: marius
 */

#ifndef CLOSESTSUB_H_
#define CLOSESTSUB_H_

#include "utils.h"
#include "MyString.h"
#include "StringSet.h"
#include "CompressedLmers.h"
#include "CompatiblePairs.h"
#include <cstdlib>
#include <cstring>
#include <set>
using namespace std;

#define DEBUG
template<bool compressLmers, class indexType>

class ClosestSub {
	typedef uint16 comprWordType;
	typedef uint16 comprPreprocType;
public:
#define INFINIT 0x1fffffff
#define MAX_COMPAT_STACK_SZ 1

	ClosestSub(MotifConfig *mc, int*rSize, indexType **rItem, char *minLmerPtr,
			CompressedLmers *compressedLmers,
			CompatiblePairs<indexType> *compatiblePairs) :
			n(mc->n), L(mc->L), d(mc->d), smallN(mc->nPrime), nMinusSmallN(
					n - smallN), firstThreshold(mc->firstThreshold), bruteForceThreshold(
					mc->bruteForceThreshold), minStackSize(mc->t), sigmaLen(
					mc->sigmaLen), twoD(2 * d), LminusD(L - d), stackSz(0), minLmerPtr(
					minLmerPtr), compressedLmers(compressedLmers), compatiblePairs(
					compatiblePairs) {
		x = new char*[n];

		if (compressLmers) {
			cLmer = new comprWordType[L];
		}
                
		initGood(n, rSize, rItem);

		candidateRowSizeBuffer = new int[n];
		candidateRowItemBuffer = new indexType*[n];
		allocateMemory();

		lastPreprocStackSz = 0;

	}

	~ClosestSub() {
		delete[] x;
		if (compressLmers)
			delete[] cLmer;
		deleteGood();
		delete[] candidateRowSizeBuffer;
		delete[] candidateRowItemBuffer;
		deAllocateMemory();
	}

	char *toNumerical(char *s, char *d, int n) {
		string sigma("ACGT");
		for (int i = 0; i < n; ++i)
			d[i] = sigma.find(s[i]);
		return d;
	}
        int i;
        int j;
        
        void handle(const char *sol) {
                string ss;
                string sigma="ACGT";
                ss=decodeString(sol,L,sigma);
		if (compressLmers) {
			compressedLmers->compressLmer(sol, cLmer);
      			if (checkMotif(cLmer, nCandidateRows, candidateRowSize,candidateRowItem)){
                                        motifs.insert(MyString(sol, L));                                
			}
		} else {
			if (checkMotif(cLmer, nCandidateRows, candidateRowSize,candidateRowItem)) {
				motifs.insert(MyString(sol, L));

			}
		}
	}
        
	set<MyString> getMotifs() {
		return motifs;
	}

	void processAllIndices() { //没有被调用到
		int nItems = rowSize[0][0];
		for (int i = 0; i < nItems; ++i)
			processIndex(i);
	}

	void processIndex(int i) {
		indexType *item = rowItem[0]; //指的是lmer indexType **rowItem;
		int ind = item[i]; //指向这行元素的第i个元素
		addString(minLmerPtr + ind);
		if (getValid(1, 0, compatiblePairs->pairOk[ind])) // int *p = pairOk[aInd]; **pairOk 是一个二维数组
			enumerateSubStrings(1);
		removeLastString();
	}

private:

	template<class K, class V>
	void insert(K *aKey, V *aVal, int l1, int l2) {
		K rk = aKey[l2];
		V rv = aVal[l2];
		int j;
		for (j = l2; j > l1 && aKey[j - 1] > rk; --j) {
			aKey[j] = aKey[j - 1];
			aVal[j] = aVal[j - 1];
		}
		aKey[j] = rk;
		aVal[j] = rv;
	}

	template<class T>
	int getValid(int pos, int extraMaxFreqLB, T *pairOkLast) { // pairOkLast是矩阵中最上边的满足条件的元素
		int total = 0;
		int e = smallN + (pos <= firstThreshold) * nMinusSmallN; //pos可能不是子序列在长序列中的位置，可能是栈的大小
		for (int k = pos; k < e; ++k) {
			indexType *item = rowItem[k];
			int nItems = rowSize[k][pos - 1];

			int i = 0;
			int j = nItems - 1;
			do {
                                while (i <= j&& (isBitSet(pairOkLast, item[i])
                                                &&(pos <= 1|| extraMaxFreq(minLmerPtr + item[i])>= extraMaxFreqLB))) {
					++i;
				}
				while (i < j&& !(isBitSet(pairOkLast, item[j])
						&&(pos <= 1|| extraMaxFreq(minLmerPtr + item[j])>= extraMaxFreqLB))) {
					--j;
				}
				if (i < j) {
					swap(item, i, j);
					++i;
				}
				--j;
			} while (i <= j);

			if (i == 0)
				return 0;

			rowSize[k][pos] = i;
			total += i;
                        {
                        	int rk = i;
                                int *rs = rowSize[k];
                                indexType *rv = rowItem[k];
                                int j = k;
                                for (; j > pos && rowSize[j - 1][pos] > rk; --j) {
                                        rowSize[j] = rowSize[j - 1];
                                        rowItem[j] = rowItem[j - 1];
                                }
                                rowSize[j] = rs;
                                rowItem[j] = rv;
                        
                        }
                }
		return total;
	}

	bool hasSolution() {
		preprocessLowerBounds();
		return hasSolution(0, 0);
	}

	void findSolutions(int pos) {
		preprocessLowerBounds(); // lowerBounds 判断一个序列是不是一个集合的共有邻序列的最低限
		enumerateStrings(0, 0);
	}

	void enumerateSubStrings(int pos) {
		if (pos == firstThreshold) {
			for (int i = smallN; i < n; ++i)
				candidateRowSizeBuffer[i] = rowSize[i][pos]; //计算n‘到n的所有行的尺寸
		}

		int nItems = rowSize[pos][pos];
		indexType *item = rowItem[pos];
		if (pos + 1 == smallN || (pos + 1 >= minStackSize && nItems < bruteForceThreshold)) {
			
                        for (int i = stackSz + 1; i < smallN; ++i) //计算栈顶到n'的行的尺寸
                                candidateRowSizeBuffer[i] = rowSize[i][pos];
                        candidateRowSize = candidateRowSizeBuffer + stackSz + 1; //n条除了栈底到栈顶的行的尺寸
			candidateRowItem = rowItem + stackSz + 1; //候选行元素，剩下的行元素加上栈中存储的元素
			nCandidateRows = n - stackSz - 1;

			for (int j = 0; j < nItems; ++j) {
				addString(minLmerPtr + item[j]);
				findSolutions(pos);
				removeLastString();
			}
		} else {
			if (pos <= 2) {
				int threshold = (stackSz + 2) * LminusD;
				for (int j = 0; j < nItems; ++j) {
					addString(minLmerPtr + item[j]);
					if (getValid(pos + 1, threshold - addMaxFreq(),compatiblePairs->pairOk[item[j]]))
						enumerateSubStrings(pos + 1);
					removeLastString();
				}
			} else {
				for (int j = 0; j < nItems; ++j) {
					addString(minLmerPtr + item[j]);
					if (hasSolution())
						if (getValid(pos + 1, sumFreqLowerBound[0] + LminusD,compatiblePairs->pairOk[item[j]]))
							enumerateSubStrings(pos + 1);
					removeLastString();
				}
			}
		}
	}

	void initGood(int n, int *rSize, indexType **rItem) {
		int total = 0;
		for (int i = 0; i < n; ++i)
			total += rSize[i]; //rSize表示每行lmer的数目
		goodMem = new indexType[total]; // indexType *goodMem; goodItem是长为total的一维数组，即长为所有lmer的数目
		allocate(rowSize, n, n);
		rowItem = new indexType*[n]; //	indexType **rowItem;

//		initialize first matrix
		int offset = 0;
		for (int i = 0; i < n; offset += rSize[i], ++i) {
			int nItems = rowSize[i][0] = rSize[i]; // int **rowSize;
			indexType *item = rowItem[i] = goodMem + offset;
			memcpy(item, rItem[i], nItems * sizeof(*item));
		}
	}

	void deleteGood() {
		delete[] goodMem;
		deAllocate(rowSize, n);
		delete[] rowItem;
	}

	bool checkMotif(const comprWordType *m, int n, int *goodSize, indexType **goodItem) {
		for (int i = 0; i < n;) {
			indexType *item = goodItem[i];
			for (int j = 0, nItems = goodSize[i]; j < nItems; ++j)
				if (compressedLmers->HamDistGeneratedVsStored(m, item[j]) <= d)
					goto _nextRowChkCompr;
			return false;
			_nextRowChkCompr: ++i;
		}
		return true;
	}

	bool checkMotif(const char *m, int n, int *goodSize, indexType **goodItem) {
		for (int i = 0; i < n;) {
			indexType *item = goodItem[i];
			for (int j = 0, nItems = goodSize[i]; j < nItems; ++j)
				if (HamDist(m, minLmerPtr + item[j], L) <= d)
					goto _nextRowChkUncomp;
			return false;
			_nextRowChkUncomp: ++i;
		}
		return true;
	}
        
	const int n, L, d, smallN, nMinusSmallN;
	const int firstThreshold;
	const int bruteForceThreshold;
	const int minStackSize;
	const int sigmaLen;
	const int twoD, LminusD;

	int stackSz; // number of strings
	int lastPreprocStackSz;
	int **colFreq;
	int *colMaxFreq;
	int **colMf;
	int **colS;

	char *lmer;
	int *sumFreqLowerBound;
	int **r;
	int ***dist; // dist[pos][i][j] = Hamming distance between suffixes of
// strings i and j starting at position pos

	int *candidateRowSizeBuffer;
	indexType **candidateRowItemBuffer;
	int *candidateRowSize;
	indexType **candidateRowItem;
	int nCandidateRows;

	int **rowSize;
	indexType **rowItem;
	indexType *goodMem; // array to store lmer indices for all matrices

	char **x;
	char *seen;
	set<MyString> motifs;

	char *minLmerPtr;

	CompressedLmers *compressedLmers;
	CompatiblePairs<indexType> *compatiblePairs;
	comprWordType *cLmer;

///////////// ******************** former ClosestStringGen *******************/ ///////////
	void addString(char *t) {
		x[stackSz] = t;
		int *mf = colMf[stackSz + 1];
		for (int j = 0; j < L; ++j) {
			char c = t[j];
			colS[j][stackSz] = c;
			mf[j] = colMaxFreq[j] + (colMaxFreq[j] == colFreq[j][(int) c]++);
		}
		colMaxFreq = mf;
		++stackSz;
	}

	void removeLastString() {
		lastPreprocStackSz -= lastPreprocStackSz == stackSz;
		--stackSz;
		colMaxFreq = colMf[stackSz];
		const char *t = x[stackSz];
		for (int j = 0; j < L; ++j)
			--colFreq[j][(unsigned char) t[j]];
	}

	int addMaxFreq() {
		int total = 0;
		for (int j = 0; j < L; ++j)
			total += colMaxFreq[j];
		return total;
	}

	int extraMaxFreq(const char *t) {
		int total = 0;
		for (int j = 0; j < L; ++j)
			total += (colMaxFreq[j] == colFreq[j][(int) t[j]]);
		return total;
	}

	void allocateMemory() {
		sumFreqLowerBound = allocate(L + 1);
		sumFreqLowerBound[L] = 0;

		dist = allocate(L + 1, smallN, smallN);
		fill(0, dist[L], smallN, smallN);

		lmer = new char[L];

		r = allocate(L + 1, smallN);
		for (int i = 0; i < smallN; ++i)
			r[0][i] = d;

		colMf = allocate(smallN + 1, L);
		memset(colMf[0], 0, L * sizeof(int));
		colMaxFreq = colMf[0];

		colFreq = allocate(L, sigmaLen);
		fill(0, colFreq, L, sigmaLen);

		colS = allocate(L, smallN);
	}

	void deAllocateMemory() {
		deAllocate(sumFreqLowerBound);
		deAllocate(dist, L + 1, smallN);
		deAllocate(lmer);
		deAllocate(r, L + 1);
		deAllocate(colMf, smallN + 1);
		deAllocate(colFreq, L);
		deAllocate(colS, L);
	}

	void preprocessLowerBounds() {
		for (int k = L - 1, cd = stackSz * LminusD; k >= 0; --k) {
			sumFreqLowerBound[k] = (cd -= colMaxFreq[k]);

			int **ds = dist[k];
			int **dsn = dist[k + 1];
			int *s = colS[k];
			for (int i = lastPreprocStackSz; i < stackSz; ++i) {
				char ci = s[i];
				for (int j = 0; j < i; ++j) {
					char cj = s[j];
					ds[i][j] = dsn[i][j] + (ci != cj);
				}
			}
		}
		lastPreprocStackSz = stackSz;
	}

	bool prunePassed(int pos) {
		int *rem = r[pos];
		int **ds = dist[pos];
		for (int i = 1; i < stackSz; ++i) {
			int *dsi = ds[i];
			for (int j = 0, ri = rem[i]; j < i; ++j)
				if (dsi[j] > ri + rem[j])
					return false;
		}
		return true;
	}

	bool updateRemainingDistances(int *colS, int a, int *oldRem, int *newRem) {
		for (int j = 0; j < stackSz; ++j)
			if ((newRem[j] = oldRem[j] - (a != colS[j])) < 0)
				return false;
		return true;
	}

	bool hasSolution(int pos, int sumFreq) {
		if (pos < L) {
			int *freq = colFreq[pos];
			int *s = colS[pos];
			int flb = sumFreqLowerBound[pos + 1] - sumFreq;
			for (int a = 0; a < sigmaLen; ++a) {
				int f = freq[a];
				if (f && f >= flb)
					if (updateRemainingDistances(s, a, r[pos], r[pos + 1]))
						if (prunePassed(pos + 1))
							if (hasSolution(pos + 1, sumFreq + f))
								return true;
			}
			return false;
		} else
			return true;
	}

	void enumerateStrings(int pos, int sumFreq) { // 构造栈中不同序列的共有子序列 ， 再去验证之 ， pos表示长为l的序列lmer的位置
		if (pos < L) {
			int *freq = colFreq[pos];
			int *s = colS[pos];
			int flb = sumFreqLowerBound[pos + 1] - sumFreq;
			for (int a = 0; a < sigmaLen; ++a) {
				int f = freq[a];
				if (f >= flb)
					if (updateRemainingDistances(s, a, r[pos], r[pos + 1]))
						if (prunePassed(pos + 1)) {
							lmer[pos] = a;
							enumerateStrings(pos + 1, sumFreq + f);
						}
			}
		} else {
			handle(lmer);
		}
	}

	bool canAddExact(char *t) {
		addString(t);
		bool sw = hasSolution();
		removeLastString();
		return sw;
	}

	bool canAddShallow(char *t) {
		int total = addMaxFreq() + extraMaxFreq(t);
		return ((stackSz + 1) * LminusD <= total);
	}

}
;

#endif
