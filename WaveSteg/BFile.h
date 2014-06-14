#pragma once
#include "commons.h"
#include "ErrLogger.h"

class BFile{
public:
	BFile(void);
	void openInFile(string fileName);
	void openOutFile(string fileName);
	void showLogs(bool show);
	void saveLogs(bool save);
	bool good();
	unsigned int getInSize();
	unsigned int getOutSize();
	void dispFile(int start, int length, int line);
	void seekg(streampos index);
	void copyBlock(int start);
	void copyBlock(int start, int length);
	virtual void initGet(string fileName)=0;
	virtual void initGet(string fileName, int setSampleSize)=0;
	virtual void initPut(string fileName)=0;
	virtual void initPut(string fileName, int setSampleSize)=0;
	virtual void initGetPut(string inName, string outName)=0;
	virtual void initGetPut(string inName, string outName, int setSampleSize)=0;
protected:
	void findInSize();//called from openInFile()
	string finName, foutName;
	int inSize, outSize;
	bool inGood, outGood;
	ifstream fin;
	ofstream fout;
	ErrLogger log;
	bool dispLog;
	bool saveLog;
};

