#include "Biterator.h"

/*
These classes break up a string, char or int into
a stream of bits or reshape them back to their original
form.

Bitwise manipulation brings up endian issues.
It's natural to want to parse strings left-to-right,
chars and int's right-to-left (lsd-to-msd)

The reshaper classes parse the same way.
Ints and chars are rebuilt right-to-left and strings 
are rebuilt left-to-right

The max field is in bits for int and char versions, but 
in bytes for the string versions
*/

//virtual base class
Biterator::Biterator(){
	reset();
}
Biterator::~Biterator(){}

void Biterator::setMax(int setMe){
	max = setMe;
}
void Biterator::reset(){
	end = false;
	bitCounter = 0;
}
bool Biterator::bitrEnd(){return end;}

//int biterator
Bitr_int::Bitr_int() : source(0){
	max = 32;
	reset();
}
Bitr_int::Bitr_int(int setMe) : source(setMe){
	max = 32;
	reset();
}
Bitr_int::~Bitr_int(){}
void Bitr_int::setSource(int setMe){
	source = setMe;
}
bool Bitr_int::nextBit(bool &bit){
	if(bitCounter == max){
		end = true;
		return false;
	}
	int n = (source & 1);
	bit = (bool)n;
	source>>=1;
	bitCounter++;
	return true;
}
//char biterator
Bitr_char::Bitr_char() : source(0){
	max = 8;
	reset();
}
Bitr_char::Bitr_char(char setMe) : source(setMe){
	max = 8;
	reset();
}
Bitr_char::~Bitr_char(){}
void Bitr_char::setSource(char setMe){
	source = setMe;
}
bool Bitr_char::nextBit(bool &bit){
	if(bitCounter == max){
		end = true;
		return false;
	}
	int n = source & 1;
	bit = (bool)n;
	source>>=1;
	bitCounter++;
	return true;
}
//string biterator
Bitr_str::Bitr_str() : source(0){
	max = 0;
	reset();
}
Bitr_str::Bitr_str(string setMe) : source(setMe){
	max = source.length();
	reset();
	kill = 0;
}
Bitr_str::~Bitr_str(){}
void Bitr_str::setSource(string setMe){
	source = setMe;
	max = source.length();
	kill = 0;
}
bool Bitr_str::nextBit(bool &bit){
	if(bitCounter >= max){					//case 3: end of string; wait for end of char to quit
		return ch.nextBit(bit);
	}
	if(bitCounter == 0 || !ch.nextBit(bit)){//case 1: initializing or case 2: end of char...both need reload
		ch.reset();							//zero ch object's bitCounter
		ch.setSource(source[bitCounter++]);	//give ch object new char from this string
		ch.nextBit(bit);		//if ch.nextBit returns false, no new bit was returned.  Get new bit now
		return true;						//for all cases but end of string
	}
	return true;							//for syntax; flow of control never reaches here
}

//virtual base class
Reshaper::Reshaper(){
}
Reshaper::~Reshaper(){}

void Reshaper::setMax(int setMe){
	max = setMe;
	//reset();
}
void Reshaper::reset(){
	end = false;
	bitCounter = 0;
}
bool Reshaper::bitrEnd(){return end;}

//int reshaper
Reshaper_int::Reshaper_int() : result(0){
	max = 32;//default val = 4 byte int
	mult = 1;
	reset();
}
Reshaper_int::Reshaper_int(int setMax) : result(0){
	max = setMax;
	mult = 1;
	reset();
}
Reshaper_int::~Reshaper_int(){}
void Reshaper_int::clrResult(){
	result = 0;
	mult = 1;
}
int Reshaper_int::getResult(){
	return result;
}
bool Reshaper_int::putBit(bool bit){
	if(bitCounter >= max){
		end = true;
		return false;
	}
	result += bit * mult;
	mult*= 2;
	bitCounter++;
	return true;//return a bool in case the calling class doesn't want to keep track
}

//char reshaper; just a wrapper for an int reshaper with max=8
Reshaper_char::Reshaper_char() : result(0){
	max = 8;
	r.setMax(8);
	reset();
}
Reshaper_char::~Reshaper_char(){}
void Reshaper_char::clrResult(){
	r.clrResult();
}
void Reshaper_char::reset(){
	r.reset();
}
char Reshaper_char::getResult(){
	return (char)r.getResult();
}
bool Reshaper_char::putBit(bool bit){
	return r.putBit(bit);
}

//string reshaper
Reshaper_str::Reshaper_str() : result(""){
	max = INT_MAX;//length to be determined by calling class
	reset();
	ch.reset();
}
Reshaper_str::Reshaper_str(int maxSet) : result(""){//string max is in bytes!!!
	max = maxSet;
	reset();
	ch.reset();
}
Reshaper_str::~Reshaper_str(){}
void Reshaper_str::clrResult(){
	result = "";
	ch.clrResult();
}
string Reshaper_str::getResult(){
	return result;
}
bool Reshaper_str::putBit(bool bit){
	if(!ch.putBit(bit)){
		char c = ch.getResult();
		result += c;
		ch.reset();
		ch.clrResult();
		bitCounter++;
		if(max <= bitCounter){return false;}
		return ch.putBit(bit);
	}
	return true;
}