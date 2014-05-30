#pragma once
#include "bfile.h"
#include "Biterator.h"
class FileItr :
	public BFile{
public:
	FileItr(void);
	~FileItr(void);
	//init
	void initGet(string filename);
	void initGet(string filename, int setsamplesize);
	void initPut(string filename);
	void initPut(string filename, int setsamplesize);
	void initGetPut(string inName, string outName);
	void initGetPut(string inName, string outName, int setSampleSize);

	void copyFile(string inName, string outName);
	//iterator functions
	void copyTest(string source);
	void nextSample_test(int limit);
	void nextSample_test_unsigned(int limit);
	void clearIterator();//call before starting any iterator task
	bool nextSample(unsigned int &n);
	bool nextSample(int &n);
	bool nextSample(int &n, int interval);
	bool nextSample(char &ch);
	void putSample(int samp);
	//biterator get functions
	void nextBit_test(int limit);//displays file in biterator-generated binary, up to limit
	void nextBit_test(string* binNums, int limit);//fills array with biterator-generated binary, up to limit
	void clearBiterator();//call before starting any biterator task
	bool nextBit(bool & bit);
	void nextLSB_test(int limit);

	int currentSample();
	bool nextLSB(bool &lsb);
	void replaceLSB_Test();
	int replaceLSB(int n, bool bit);
	//biterator put functions
	void putBit_test(string* binNums, int size);
	void putBit(bool bit);

	
protected:
	//utility functions
	unsigned int toUnsignedInt(char* chunk, int start, int length);
	int toInt(char* chunk, int start, int length);
	void toCharArray(int n, char * charArray, int start, int length);
	void toCharArray(unsigned int n, char * charArray, int start, int length);
	
	int sampleSize, samp_int, unsignedMax, signedMax, bitCounter;
	char* samp_charArray;
	bool deleteThisVariable;

	int currSample;
};

