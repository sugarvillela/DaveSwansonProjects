#pragma once
#include "waveitr.h"

const string magicWord = "{  }";//unlikely to occur naturally

class WaveSteg : public WaveItr{
public:
	WaveSteg(void);
	~WaveSteg(void);
	//Encode file to file
	void init_encFToF(string setHostName, string setParasiteName);
	void init_encFToF(string setHostName, string setParasiteName, string setDestName);
	void encFToF();
	bool verify(string fName1, string fName2, bool skipLSB, int verbosity);
	//Encode string to file
	void init_encSToF(string setHostName, string encodeMe);
	void init_encSToF(string setHostName, string encodeMe, string setDestName);
	void encSToF();
	//Decode to file or string
	void initDecode(string setHostName);
	void initDecode(string setHostName, string setDestName);
	void decode();

	//File fixers
	bool canonize(string setHostName, string setDestName);
	//getters
	int minHostSize();
	string getEncodeString();
private:
	//copy functions
	void copyHeader();
	void copyDataStream(int iGet, int iPut);
	void copyDataStream(int iGet, int iPut, int length);
	//Encode file to file
	void encParaStats(int iGet, int iPut);
	void encFToF_core();
	//Encode string to file
	void encSToF_core();
	//Decode to file
	void decParaStats(int iGet);
	void decToFile();
	//Decode to string
	void decToString();
	//utility
	bool checkSize();

	string hostName, paraName, destName, paraExt, encodeString;
	int paraSize;//size of parasite file
	int encSize;//paraSize*8;
	int minSize;//Minimum size host file needed to encode
	bool hangBit;//store leftover bit between encode/decode steps
	WaveItr host;
	FileItr parasite;
};