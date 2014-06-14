#pragma once
#include "commons.h"

const string msgs[] = {
	"Commence operation",//0
	"Failed to open file",
	"FileName needs a valid extension (.txt etc)",
	"Host file too small to encode.  Choose a larger file",
	"No watermark found in host file",
	"Watermark found in host file",//5
	"Cancelling operation",
	"File size is zero",
	"String length is zero",
	"File sizes do not match",
	"File contents do not match",//10
	"Match file size and contents",
	"Open file(s)",
	"Fun fact"
};

struct Fact{
public:
	Fact();
	Fact(string setTime, string setFunct, int setMCode, string setMComment, bool setErr);

	int mCode;
	string yr, time, funct, mComment;
	unsigned int uiTime;
	bool err;
};

class ErrLogger{
public:
	ErrLogger(void);
	~ErrLogger(void);
	void setClient(string setMe);
	void addToLog(string setFunct, int setMCode, string setMComment, bool setErr);
	void sortByTime();
	void sortByErr();
	void dispLog();
	void logToFile();
	void clrLog();
private:
	string client;
	vector<Fact*> v;
};

