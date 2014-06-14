#include "FileItr.h"
/*
Since this class and its subclasses are designed to be instantiated
in other classes (composition) there's no point in opening the file
through the constructor.  An init method opens the files and sets most
of the fields.  So the way to set up the class is:
    FileItr f;
    f.initGet("nums.bin");

or use initGet("nums.bin", 2) if you want to manually set the sample 
size to 2.

Notes:
To use the biterator, init the class as shown in BFile notes.
Then call clearBiterator() to zero the bit counter and temp fields
Suggestion:  don't use the biterator with sample size other than 1.
Makes it tough to sort out the endian's

To use the iterator, init the class as shown in BFile notes.
Then call clearIterator() to reset the fstream object
*/

FileItr::FileItr(void) : samp_charArray(0), samp_int(0), BFile() {
}
FileItr::~FileItr(void){
    if(samp_charArray){
        delete[] samp_charArray;
        samp_charArray= 0;
    }
    if(dispLog){
        log.dispLog();
    }
    if(saveLog){
        log.logToFile();
    }
    log.clrLog();
}
//Initialize fields
void FileItr::initGet(string fileName){
    initGet(fileName, 1);
}
void FileItr::initGet(string fileName, int setSampleSize){
    openInFile(fileName);
    if(setSampleSize>0 && inGood){
        string op = "Opening in file: file name: " + fileName;//describes the operation commenced
        log.addToLog(__FUNCTION__, 0, op, false);
        sampleSize=setSampleSize;
        unsignedMax = power(2, sampleSize*8)-1;//highest number encodable in specified sample size
        signedMax = unsignedMax/2;//if data is signed
        samp_charArray = new char [sampleSize];//small array to hold single sample (1-2-3-4 bytes)
    }
    else{
        string op = "In File name: " + fileName;
        log.addToLog(__FUNCTION__, 1, op, true);		
    }
}
void FileItr::initPut(string fileName){
    initPut(fileName, 1);
}
void FileItr::initPut(string fileName, int setSampleSize){
    openOutFile(fileName);
    if(setSampleSize>0 && outGood){
        string op = "Opening out file: file name: " + fileName;//describes the operation commenced
        log.addToLog(__FUNCTION__, 0, op, false);
        sampleSize=setSampleSize;
        unsignedMax = power(2, sampleSize*8)-1;//highest number encodable in specified sample size
        signedMax = signedMax/2;//if data is signed
        samp_charArray = new char [sampleSize];//small array to hold single sample (1-2-3-4 bytes)
    }
    else{
        string op = "Out File name: " + fileName;
        log.addToLog(__FUNCTION__, 1, op, true);		
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
            int n;
            while(source.nextSample(n)){
                putSample(n);
            }
        }
    }
    bar();
}
void FileItr::putSample(int n){
    toCharArray(n, samp_charArray, 0, sampleSize);
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
    bar();
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
    bar();
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
    int n, sum = charArray[start+length-1];//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
    sum &= 0xFF;			//Unsigned int...Don't want sign bit set, so mask it
    for(int i=start+length-2;i>=start;i--){//will fall through if length = 1
        sum <<= 8;			//shift one byte left to make room for next
        n = charArray[i];		//same as first step, but use temp variable n
        n &= 0xFF;			//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
        sum += n;			//add to LSB's of sum
    }
    return sum;
}
int FileItr::toInt(char* charArray, int start, int length){//reads c-string start to start+length, returns as signed int
    int n, sum = charArray[start+length-1];//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
    for(int i=start+length-2;i>=start;i--){//will fall through if length = 1
        sum <<= 8;			//shift one byte left to make room for next
        n = charArray[i];		//same as first step, but use temp variable n
        n &= 0xFF;			//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
        sum += n;			//add to LSB's of sum
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
            //stringstream().swap(ss);//clears string stream
            i++;
        }
    }
    bar();
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
            ss.str("");
            ss.clear();
            //stringstream().swap(ss);//clears string stream
        }
    }
    bitCounter++;
    bar();
}
void FileItr::clearBiterator(){//reset class and ifstream counters
    clearIterator();
    bitCounter = 0;
    samp_int = 0;
}
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
    return more;
}
//biterator functions put bit
void FileItr::putBit_test(string* binNums, int size){//precondition: initPut()
    clearBiterator();
    for(int i=0;i<size;i++){
        string temp = binNums[i];
        int len = temp.length();
        for(int ii=len-1;ii>=0;ii--){
            bool bit = ( temp.at(ii) == '0' )? false : true;
            putBit(bit);
        }
    }
    putBit(0);//need last push to write last number, bit doesn't matter
    bar();
}
void FileItr::putBit(bool bit){
    if(bitCounter == 8*sampleSize){
        bitCounter=0;
        uintToCharArray((unsigned int)samp_int, samp_charArray, sampleSize);
        samp_int=0;
        fout.write (samp_charArray, sampleSize);
    }

    if( bit ){
        samp_int += power(2, bitCounter);
    }
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
    bar();
}
int FileItr::currentSample(){return currSample;}
bool FileItr::nextLSB(bool &lsb){//sets lsb 1 or 0, based on lsb of nextSample()
    int n;
    if(!nextSample(n)){return false;}//this bool is to pass file state
    else{currSample = n;}
    lsb = true & n;//this bool is the return
    return true;//this bool is to pass file state
}
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
    bar();
}
int FileItr::replaceLSB(int n, bool bit){
    n &= 0xFFFFFFFE;
    n+= bit;
    return n;
}