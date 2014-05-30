#include "FileItr.h"
/*
Notes:
Char to int conversions are difficult because if the MSB is 1
(a negative number) the int result will carry the last to all
additional MoreSB's of the int, in keeping with 2's complement.  
Example: a char of 0x8A will come back as 0xFFFFFF8A because 
the sign bit in the 8 turns into F's all the way over.  
This is fine as long as a signed int is the destination.  For an
unsigned int, you would want the left-most bits to be zeroes.  Instead, 
the output is undefined, because a number like 0x7A would be 0x0000007A,  
while 0x8a would be 0xFFFFFF8A.
For unsigned ints it is necessary to mask all but the 
needed bits after any conversions and sometimes during operations.

Using multi-byte samples requires a different size mask, a number equal 
to the unsigned max for the sample size in use.

To use the biterator, init the class as shown in BFile notes.
Then call clearBiterator() to zero the bit counter and temp fields
Suggestion:  don't use the biterator with sample size other than 1.
Makes it tough to sort out the endian's

To use the iterator, init the class as shown in BFile notes.
Then call clearIterator() to reset the fstream object

*/

FileItr::FileItr(void) : samp_charArray(0), samp_int(0){deleteThisVariable=true;}
FileItr::~FileItr(void){
	if(samp_charArray){
		delete[] samp_charArray;
		//cout << "File itr deleted sampchararray" << endl;
	}
	//else{cout << "File itr sampchararray null" << endl;}
}
//Initialize fields
void FileItr::initGet(string fileName){
	initGet(fileName, 1);
}
void FileItr::initGet(string fileName, int setSampleSize){
	openInFile(fileName);
	if(setSampleSize>0 && inGood){
		sampleSize=setSampleSize;
		unsignedMax = power(2, sampleSize*8)-1;//highest number encodable in specified sample size
		signedMax = unsignedMax/2;//if data is signed
		samp_charArray = new char [sampleSize];//small array to hold single sample (1-2-3-4 bytes)
	}
	//cout << "unsignedMax=" << dec << unsignedMax << endl;
	//cout << "signedMax=" << signedMax << endl;
}
void FileItr::initPut(string fileName){
	initPut(fileName, 1);
}
void FileItr::initPut(string fileName, int setSampleSize){
	openOutFile(fileName);
	//cout << "init put????" << endl;
	if(setSampleSize>0 && outGood){
		sampleSize=setSampleSize;
		//cout << "init put sampleSize = " << sampleSize << endl;
		unsignedMax = power(2, sampleSize*8)-1;//highest number encodable in specified sample size
		signedMax = signedMax/2;//if data is signed
		samp_charArray = new char [sampleSize];//small array to hold single sample (1-2-3-4 bytes)
	}
}
void FileItr::initGetPut(string inName, string outName){
	initGet(inName);
	initPut(outName);
}
void FileItr::initGetPut(string inName, string outName, int setSampleSize){
	initGet(inName, setSampleSize);
	initPut(outName, setSampleSize);
}

void FileItr::copyFile(string inName, string outName){
	initGetPut(inName, outName);
	clearIterator();
	copyBlock(0);
}
//iterators //Can iterate and return in multi-byte increments, determined by sampleSize...of course sampleSize>4 surpasses unsigned int max
void FileItr::copyTest(string sourceName){
	if(good()){
		cout << "this file good" << endl;
		FileItr source;
		source.initGet(sourceName, sampleSize);
		if(source.good()){
			cout << "composite file good" << endl;
			source.dispFile(0, 50, 5);
			source.clearIterator();
			//char* charArray = new char[sampleSize];
			int n;
			while(source.nextSample(n)){

				putSample(n);
				//toCharArray(n, charArray, 0, sampleSize);
				//cout << "writing " << hex << n << endl;
				//fout.write(charArray, sampleSize);
			}
			//delete[] charArray;
		}
	}
}
void FileItr::putSample(int n){
	//if(deleteThisVariable){
	//	cout << "putSample: sampleSize=" << sampleSize << endl;
	//	deleteThisVariable = false;
	//}
	toCharArray(n, samp_charArray, 0, sampleSize);
	//cout << "writing " << hex << n << endl;
	fout.write(samp_charArray, sampleSize);
}
void FileItr::nextSample_test(int limit){//Debug function:  displays next sample result in hex
	cout << "nextSample_test...File size=" << dec << inSize << "...sampleSize=" << sampleSize << endl;
	fin.clear();
	fin.seekg(0, ios::beg);
	int n, i = 0;
	while(nextSample(n) && limit > i++){
		cout << hex << n << "\t";
		if( (i)%10 == 0 ){
			cout << endl;
		}
	}
	cout << endl;
}
void FileItr::nextSample_test_unsigned(int limit){//Debug function:  displays next sample result in hex
	cout << "nextSample_test_unsigned...File size=" << dec << inSize << "...sampleSize=" << sampleSize << endl;
	fin.clear();
	fin.seekg(0, ios::beg);
	int i = 0;
	unsigned int n;
	while(nextSample(n) && limit > i++){
		cout << hex << n << "\t";
		if( (i)%10 == 0 ){
			cout << endl;
		}
	}
	cout << endl;
}
void FileItr::clearIterator(){//reset ifstream counters
	fin.clear();
	fin.seekg(0, ios::beg);
	fout.clear();
	fout.seekp(0, ios::beg);
}
bool FileItr::nextSample(unsigned int &n){//returns unsigned int and increments file pointer by sampleSize bytes
	fin.read (reinterpret_cast<char *>(samp_charArray), sampleSize);
	if(fin.eof()){return false;}
	n = toUnsignedInt(samp_charArray, 0, sampleSize);
	return true;
}
bool FileItr::nextSample(int &n){//returns signed int and increments file pointer by sampleSize bytes
	fin.read (reinterpret_cast<char *>(samp_charArray), sampleSize);
	if(fin.eof()){return false;}
	n = toInt(samp_charArray, 0, sampleSize);
	return true;
}
bool FileItr::nextSample(int &n, int interval){//returns signed int and increments by interval specified
	fin.read (reinterpret_cast<char *>(samp_charArray), sampleSize);
	if(fin.eof()){return false;}
	n = toInt(samp_charArray, 0, sampleSize);
	fin.seekg(interval, ios::cur);
	return true;
}
bool FileItr::nextSample(char &ch){//for single-byte, echoes protected ifstream object
	return fin.get(ch);
}



//char array to int, big endian interpretation: charArray[0]=MSB's
unsigned int FileItr::toUnsignedInt(char* charArray, int start, int length){//reads c-string start to start+length, returns as unsigned int
	int n, sum = charArray[start+length-1];	//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
	sum &= 0xFF;							//Unsigned int...Don't want sign bit set, so mask it
	for(int i=start+length-2;i>=start;i--){	//will fall through if length = 1
		sum <<= 8;							//shift one byte left to make room for next
		n = charArray[i];					//same as first step, but use temp variable n
		n &= 0xFF;							//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
		sum += n;							//add to LSB's of sum
	}
	return sum;
}
int FileItr::toInt(char* charArray, int start, int length){//reads c-string start to start+length, returns as signed int
	int n, sum = charArray[start+length-1];	//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
	for(int i=start+length-2;i>=start;i--){	//will fall through if length = 1
		sum <<= 8;							//shift one byte left to make room for next
		n = charArray[i];					//same as first step, but use temp variable n
		n &= 0xFF;							//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
		sum += n;							//add to LSB's of sum
	}
	return sum;//identical to toUnsignedInt except first operation's sign bit is allowed to carry over
}
//int to char array, big endian interpretation: charArray[0]=MSB's
void FileItr::toCharArray(int n, char * charArray, int start, int length){//BigEndian
	for(int i=start;i<start+length;i++){//Precondition: charArray is allocated to sufficient size
		charArray[i] = n & 0xFF;
		n>>=8;
	}
}
void FileItr::toCharArray(unsigned int n, char * charArray, int start, int length){//BigEndian
	for(int i=start;i<start+length;i++){//Precondition: charArray is allocated to sufficient size
		charArray[i] = n & 0xFF;
		n>>=8;
	}
}

//biterator functions get bit
void FileItr::nextBit_test(int limit){
	cout << "nextBit test...File size = " << dec << inSize << endl;
	clearBiterator();
	int i = 0;
	bool bit;
	stringstream ss;
	while(limit>i && nextBit(bit)){
		ss << bit;
		if(bitCounter == 8*sampleSize){
			string reverseMe = ss.str();
			cout << reverse(reverseMe) << endl;
			cout << reverse(reverseMe) << endl;
			stringstream().swap(ss);//clears string stream
			i++;
		}
	}
}
void FileItr::nextBit_test(string* binNums, int limit){//precondition: binNums is allocated, limit=size of array
	cout << "nextBit test...File size = " << dec << inSize << endl;
	clearBiterator();
	int i = 0;
	bool bit;
	stringstream ss;
	while(limit>i && nextBit(bit)){
		ss << bit;
		if(bitCounter == 8*sampleSize){
			string reverseMe = ss.str();
			binNums[i++] = reverse(reverseMe);
			stringstream().swap(ss);//clears string stream
		}
	}
	bitCounter++;
}
void FileItr::clearBiterator(){//reset class and ifstream counters
	clearIterator();
	bitCounter = 0;
	samp_int = 0;
}
//int FileItr::nextBit(){
//	if(bitCounter == 0){
//		nextSample(samp_int);
//	}
//	else if(bitCounter == 8*sampleSize){
//		bitCounter=0;
//		nextSample(samp_int);
//	}
//	int p = power(2, bitCounter++);
//	return (samp_int & p)/p;
//}
bool FileItr::nextBit(bool &bit){
	bool more = true;
	if(bitCounter == 0){
		more = nextSample(samp_int);
	}
	else if(bitCounter == 8*sampleSize){
		bitCounter=0;
		more = nextSample(samp_int);
	}
	int n = samp_int & 1;
	samp_int>>=1;
	bitCounter++;
	bit = (bool)n;
	//int p = power(2, bitCounter++);
	//return (samp_int & p)/p;
	return more;
}

//biterator functions put bit
void FileItr::putBit_test(string* binNums, int size){//precondition: initPut()
	//cout << "putBit_test" << endl;
	clearBiterator();
	for(int i=0;i<size;i++){
		string temp = binNums[i];
		int len = temp.length();
		//cout << "putBitTest string = " << temp << endl;
		for(int ii=len-1;ii>=0;ii--){
			bool bit = ( temp.at(ii) == '0' )? false : true;
			//cout << bit;
			putBit(bit);
		}
		//cout << "End word " << endl;
	}
	putBit(0);//need last push to write last number, bit doesn't matter
}
void FileItr::putBit(bool bit){
	if(bitCounter == 8*sampleSize){
		//cout << "\tput bit: reset bit counter, samp_int=" << hex << samp_int;
		bitCounter=0;
		uintToCharArray((unsigned int)samp_int, samp_charArray, sampleSize);
		samp_int=0;
		fout.write (samp_charArray, sampleSize);
	}
	
	if( bit ){
		samp_int += power(2, bitCounter);
	}
	//cout << "bitCounter=" << bitCounter << ", bit:" << bit << " * power:" << power(2, bitCounter) << ", samp_int=" << samp_int << endl;
	bitCounter++;
}

void FileItr::nextLSB_test(int limit){//Debug function:  displays lsb's in groups of eight until eor or limit
	cout << "nextLSB_test...File size = " << dec << inSize << endl;
	fin.clear();
	fin.seekg(0, ios::beg);
	int i = 0;
	bool lsb;
	while(nextLSB(lsb) && limit > i++){
		cout << lsb;
		if( (i)%8 == 0 ){
			cout << endl;
		}
	}
	cout << endl;
}
int FileItr::currentSample(){return currSample;}
bool FileItr::nextLSB(bool &lsb){//sets lsb 1 or 0, based on lsb of nextSample()
	int n;
	if(!nextSample(n)){return false;}//this bool is to pass file state
	else{currSample = n;}
	lsb = true & n;//this bool is the return
	return true;//this bool is to pass file state
}
//to subclass
void FileItr::replaceLSB_Test(){
	int n;
	fin.clear();
	fin.seekg (0, ios::beg);
	int i = 0;
	bool toggle = true;
	while(nextSample(n) && i<25){
		cout << "bit=" << toggle;
		cout << ", pre=" << hex << n;
		n = replaceLSB(n,toggle);
		cout << ", post=" << n << endl;
		i++;
		toggle = !toggle;
	}
}
int FileItr::replaceLSB(int n, bool bit){
	n &= 0xFFFFFFFE;
	
	//cout << ", p/mask=" << hex << n;
	n+= bit;
	return n;
}
//int FileItr::getLSD(char n){
//	return 0xF & n;
//}
//int FileItr::getMSD(char n){
//	return 0xFF & (0xF0 & n)/16;
//}