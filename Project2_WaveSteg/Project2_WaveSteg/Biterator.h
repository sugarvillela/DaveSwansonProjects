#pragma once
#include "commons.h"

class Biterator{
public:
	Biterator();
	~Biterator();
	virtual bool nextBit(bool &bit)=0;
	void reset();
	void setMax(int setMe);
	bool bitrEnd();//just because it's funny
protected:
	int bitCounter, max;//max is in bits
	bool end;
};

class Bitr_int : public Biterator{
public:
	Bitr_int();
	Bitr_int(int setMe);//setMe sets source
	~Bitr_int();
	void setSource(int setMe);
	bool nextBit(bool &bit);
private:
	int source;
};

class Bitr_char : public Biterator{
public:
	Bitr_char();
	Bitr_char(char setMe);
	~Bitr_char();
	void setSource(char setMe);
	bool nextBit(bool &bit);
private:
	char source;
};
class Bitr_str : public Biterator{
public:
	Bitr_str();
	Bitr_str(string setMe);
	~Bitr_str();
	void setSource(string setMe);
	bool nextBit(bool &bit);
private:
	string source;
	Bitr_char ch;
	int kill;
};

class Reshaper{
public:
	Reshaper();
	~Reshaper();
	virtual bool putBit(bool bit)=0;
	virtual void clrResult()=0;
	void reset();
	void setMax(int setMe);
	bool bitrEnd();
protected:
	int bitCounter, max;
	bool end;
};

class Reshaper_int : public Reshaper{
public:
	Reshaper_int();
	Reshaper_int(int max);
	~Reshaper_int();
	int getResult();
	bool putBit(bool bit);
	void clrResult();
private:
	int result;
	unsigned int mult;
};

class Reshaper_char : public Reshaper{
public:
	Reshaper_char();
	~Reshaper_char();
	char getResult();
	bool putBit(bool bit);
	void clrResult();
	void reset();
private:
	char result;
	Reshaper_int r;
	//unsigned int mult;
};

class Reshaper_str : public Reshaper{
public:
	Reshaper_str();
	Reshaper_str(int max);
	~Reshaper_str();
	void clrResult();
	string getResult();
	bool putBit(bool bit);
private:
	string result;
	Reshaper_char ch;
};