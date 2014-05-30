#pragma once
#include "FileItr.h"
#include "WaveForm.h"

struct headerData{//keep less relevant data out of the way in this structure
	unsigned int fileLen;//true length-8
	unsigned int formatLen, formatTag, bytes_sec, blockAlign;//not important; for display
	unsigned int numChannels, sampleRate, bitDepth;//more important
};

class WaveItr : public FileItr {
public:
	WaveItr(void);
	~WaveItr(void);
	//init
	void initGet(string filename);
	void initGet(string filename, int setsamplesize);
	//displays
	void dispHeader();
	void dispHeaderHex();
	void dispDataHex(int limit);
	void dispWaveForm(int limit);
	int numZeroes(bool verbose);
	//iterator help
	void seekDataStart();
	//getters
	char* getHeader();
	char* getHeaderFixed();
	int getHeadSize();
	int getSampleSize();
	int getDataStart();
	int get_iatOneSec();
	headerData* getHeaderData();
	unsigned int getDataStreamLength();
	bool isWave();
	bool isCanon();
protected:
	//init
	bool readHeader();
	unsigned int findDataStart();
	unsigned int findStreamLength();
	//utility
	char *fileToChArray(streampos start, streampos size);
	void find_iatOneSec();
	
	
	unsigned int streamLen, iData, iatOneSec;//set by findDataStart(), findStreamLength(), find_iatOneSec()
	int headSize;
	char* header;
	headerData hData;
};

