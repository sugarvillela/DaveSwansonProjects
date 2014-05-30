#include "BFile.h"
/*
Notes:
This is the base class for a file iteration family of classes.  It
handles read and write opening, closing, status and a hex file display.

Since this class and its subclasses are designed to be instantiated
in other classes (composition) there's no point in opening the file
through the constructor.  An init method opens the files and sets most
of the fields.  So the way to set up the class is:
	FileItr f;
	f.initGet("nums.bin");

or use initGet("nums.bin", 2) if you want to manually set the sample 
size to 2.
*/

BFile::BFile(void)  : inSize(0), outSize(0), inGood(false), outGood(false), finName(""), foutName("") {
	//cout << "BFile constructor" << endl;
}
BFile::~BFile(void){}

void BFile::openInFile(string fileName){
	//cout << "open file insize:  " << inSize << endl;
	const char* fName = fileName.c_str();
	inSize = (unsigned int)fin.tellg();
    fin.open(fileName, ios::in|ios::binary|ios::ate);
	inGood = fin.is_open();
	if(inGood){
		findInSize();
	}
}
void BFile::openOutFile(string fileName){
	const char* fName = fileName.c_str();
    fout.open(fileName, ios::out | ios::binary | ios::ate);
	outGood = fout.is_open();
}

//setter getters
void BFile::findInSize(){//preconditions: file is open //called from openInFile() //protected function
	fin.clear();
	fin.seekg(0, ios::end);
	inSize = fin.tellg();
	//cout << "findInSize insize:  " << inSize << endl;
}
unsigned int BFile::getInSize(){//Public get
	return inSize;
}
unsigned int BFile::getOutSize(){//Returns meaningful data (non-zero) only after a write
	return outSize;
}
bool BFile::good(){//using one or the other, so whichever is good returns true
	return inGood || outGood;
}

//echo command to protected member
//bool BFile::eofg(){
//	return fin.eof();
//}
//bool BFile::eofp(){
//	return fout.eof();
//}
void BFile::backOne(){
	//cout << "backOne:  pre tellg=" << fin.tellg() << endl;
	fin.seekg(-1, ios::cur);
	//cout << "backOne:  post tellg=" << fin.tellg() << endl;
}
void BFile::seekg(streampos index){
	fin.seekg(index, ios::beg);
}
//debug displays
void BFile::dispFile(int start, int length, int line){//Public, for displaying part or all of a file
	cout << "Display File:" << endl;
	cout << "Size = " << dec << inSize << endl;
	char ch;
	fin.clear();
	fin.seekg (start, ios::beg);
	while (fin.get(ch) && start++ < length){
		int n = ch;
		//n = n & 0xFF;
		//char negTest = charMask(8);
		//if(n & negTest == negTest){cout << "-";}
		cout << hex << n << "\t";
		if( (start)%line == 0 ){
			cout << endl;
		}
	}
	cout << endl;
}
void BFile::copyBlock(int start){
	fin.seekg(start, ios::beg);
	char ch;
	while (fin.get(ch)) {
		fout.put(ch);
	}
	//fout.put(ch);
}
void BFile::copyBlock(int start, int length){
	int end = start+length;
	fin.seekg(start, ios::beg);
	char ch;
	while (end>start++ && fin.get(ch)) {
		fout.put(ch);
	}
}