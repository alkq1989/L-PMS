/* 
 * MotifFinder.h
 * 
 *  Created on: Nov 20, 2012
 *      Author: marius
 */

#ifndef MOTIFFINDER_H_
#define MOTIFFINDER_H_
#include "CompressedLmers.h"
#include "CompatiblePairs.h"
#include "StringSet.h"
#include "ClosestSub.h"
//#include <omp.h>

template<bool compressLmers, class indexType>
class MotifFinder {
public:
	MotifFinder(StringSet& s, MotifConfig *mc) : // s 在MotifWorker中的loadStringsFromBuffer函数已经赋值
			mc(mc) {
		minLmerPtr = s.memStart;

		compressedLmers = NULL;
		if (compressLmers)
			compressedLmers = new CompressedLmers(minLmerPtr, s.totalLength,mc->L, mc->sigmaLen);

		int nLmers = s.totalLength - mc->L + 1; //这里s表示的是一个类，调用里面的参数
		rowSize = s.range; // 将每一条序列长度对应的放到rowSize数组中 rowSize为一维数组
		rowItem = createIndexMatrix(s); // rowItem为二维数组
		compatiblePairs = new CompatiblePairs<indexType>(mc->n, rowSize,rowItem, mc->L, 2 * mc->d, minLmerPtr, nLmers, compressedLmers);

                ms = new ClosestSub<compressLmers, indexType>(mc, rowSize, rowItem,minLmerPtr, compressedLmers, compatiblePairs);
	}

	void processIndex(int i) {
		ms->processIndex(i);
	}

	set<MyString> getMotifs() {
		return ms->getMotifs();
	}

	~MotifFinder() {
		if (compressLmers)
			delete compressedLmers;
		deAllocate(rowItem, mc->n);
		delete compatiblePairs;
		delete ms;
	}
private:
	indexType **createIndexMatrix(StringSet& s) { // 构造索引矩阵
		indexType **rowItem = new indexType*[s.n]; //rowItem为n行的二维数组
		for (int i = 0; i < s.n; ++i) {
			int nItems = s.range[i]; //nItem 为对应行lmer的数目
			indexType *item = new indexType[nItems]; //构造一个索引类型的数组长度为nItems的一维数组
			rowItem[i] = item; //将个行对应的数组赋给rowItem对应的行
			int l = s.s[i] - s.memStart;
			for (int j = 0; j < nItems; ++j)
				item[j] = l + j;
		}
		return rowItem; //rowItem存的是字符串的索引，并不是真实的字符串
	}

	MotifConfig *mc;
	char * minLmerPtr;
	int *rowSize;
	indexType **rowItem;
	CompatiblePairs<indexType> *compatiblePairs;
	ClosestSub<compressLmers, indexType> *ms;
	CompressedLmers *compressedLmers;
};

#endif /* MOTIFFINDER_H_ */
